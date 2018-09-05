// ProcMapGeneration.cpp: Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "irrlicht.h"
#include <vector>
#include <map>
#include <functional>
#include <future>
#include <thread>
#include <mutex>
#include <queue>
#include "MapControlEventReceiver.hpp"
#include "TerrainShaderCallback.h"
#include "TerrainGenerator.hpp"

#include "Utils.h"

using namespace irr;
using namespace irr::scene;
using namespace irr::video;


int main(int argc, char** argv) {
    auto eventReceiver = new MapControlEventReceiver();

    auto params = []() -> const SIrrlichtCreationParameters {
        SIrrlichtCreationParameters params;
        params.AntiAlias = 4;
        params.DriverType = video::EDT_OPENGL;
        params.WindowSize = core::dimension2d<u32>(1920*0.75, 1080*0.75);
        params.Fullscreen = false;
        params.Vsync = true;
        params.WindowId = nullptr;
        params.DriverMultithreaded = true;
        return params;
    }();
    
    IrrlichtDevice *device = irr::createDeviceEx(params);
    device->setEventReceiver(eventReceiver);
    
	irr::core::stringw str = L"Procedural Terrain Generation";
	device->setWindowCaption(str.c_str());
	
	IVideoDriver* video = device->getVideoDriver();
    video->getMaterial2D().AntiAliasing=video::EAAM_FULL_BASIC;

    
    ISceneManager* smgr = device->getSceneManager();
    
    float chunkSizeAB = 128.f;
    auto ter2 = TerrainGenerator(irr::core::dimension2du{chunkSizeAB,chunkSizeAB}, 90.0, device);

	auto mainScene = smgr->addEmptySceneNode();

	std::map<irr::core::vector2di, irr::scene::IMeshSceneNode*> chunks;
    
    auto player = smgr->addCubeSceneNode(1);
	player->setScale(irr::core::vector3df{1, 1, 1});
	player->setPosition(irr::core::vector3df{32,32,5});
    
	ICameraSceneNode *cam2 = smgr->addCameraSceneNode();
	cam2->setPosition(irr::core::vector3df(0, 0, -10));
	mainScene->addChild(cam2);

    auto light = smgr->addLightSceneNode();
    light->setRadius(100.f);
	light->enableCastShadow();
    light->setLightType(video::ELT_DIRECTIONAL);
	light->setRotation(irr::core::vector3df(90, 0, 0));
    player->addChild(light);
  
    tom::setupShader(device);

	int viewDistance = 4; // view distance as radius in chunks

    u32 then = device->getTimer()->getTime();
	while(device->run() && device) {
        // Work out a frame delta time.
        const u32 now = device->getTimer()->getTime();
        f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
        then = now;
        
        frameDeltaTime = 0.005f;
        
        tom::manageMainthreadCallbacks();
        
		auto x = eventReceiver->mouseInformation().x;
		auto screenW = (1920 * 0.75f);
		auto perc = x / screenW;
		auto angle = 360.f*2 * -perc;
		float yVal = sin(irr::core::degToRad(angle));
		float xVal = cos(irr::core::degToRad(angle));

        float value = 100.f;
		if (eventReceiver->keyPressed(irr::KEY_RIGHT) || eventReceiver->keyPressed(irr::KEY_KEY_D)) player->setPosition(player->getPosition() + irr::core::vector3df(value*cos(irr::core::degToRad(angle - 90.f)), 0.f, value*sin(irr::core::degToRad(angle - 90.f)))*frameDeltaTime);
		if (eventReceiver->keyPressed(irr::KEY_LEFT) || eventReceiver->keyPressed(irr::KEY_KEY_A)) player->setPosition(player->getPosition() + irr::core::vector3df(value*cos(irr::core::degToRad(angle + 90.f)), 0.f, value*sin(irr::core::degToRad(angle  +90.f)))*frameDeltaTime);
		if (eventReceiver->keyPressed(irr::KEY_UP) || eventReceiver->keyPressed(irr::KEY_KEY_W)) player->setPosition(player->getPosition() + irr::core::vector3df(value*xVal*(frameDeltaTime), 0.f, value*yVal*(frameDeltaTime)));
		if (eventReceiver->keyPressed(irr::KEY_DOWN) || eventReceiver->keyPressed(irr::KEY_KEY_S)) player->setPosition(player->getPosition() + irr::core::vector3df(value*cos(irr::core::degToRad(angle + 180.f)), 0.f, value*sin(irr::core::degToRad(angle + 180.f)))*frameDeltaTime);
		if (eventReceiver->keyPressed(irr::KEY_SPACE)) player->setPosition(player->getPosition() + irr::core::vector3df(0,1,0));
		if (eventReceiver->keyPressed(irr::KEY_KEY_X)) player->setPosition(player->getPosition() + irr::core::vector3df(0,-1,0));

		cam2->setTarget(player->getPosition());

		player->setRotation(irr::core::vector3df(0,-angle,0));

		auto newPosition = player->getPosition() + irr::core::vector3df(-10 * xVal, 10, -10 * yVal);
		float lowpassfilterFactor = .045f;
		newPosition = (newPosition * lowpassfilterFactor) + (cam2->getPosition() * (1.0 - lowpassfilterFactor));
		cam2->setPosition(newPosition);

		auto playerChunkLoc = irr::core::vector2di(player->getPosition().X / chunkSizeAB, player->getPosition().Z / chunkSizeAB);

		for (auto ting : chunks) ting.second->setVisible(false);
		for (int y = -viewDistance; y <= viewDistance; y++) {
			for (int x = -viewDistance; x <= viewDistance; x++) {

				auto key = irr::core::vector2di(x, y) + playerChunkLoc;
				bool hasKey = false;
				for (auto ting : chunks) if (key.equals(ting.first)) hasKey = true;
				if (hasKey && chunks[key]) chunks[key]->setVisible(true);
				else {
                    chunks[key] = smgr->addCubeSceneNode();
                    tom::onSeparateThread([ter2 = std::ref(ter2), mainScene = mainScene, chunks = std::ref(chunks), key = key]() mutable {
                        ter2.get().getMeshAt(key, [mainScene = mainScene, chunks = std::ref(chunks), key = key](irr::scene::IMeshSceneNode* m) mutable {
                            mainScene->addChild(m);
                            chunks.get()[key]->remove();
                            chunks.get()[key] = m;
                        });
                    });
				}
			}
		}
		
        video->beginScene(true, true, video::SColor(255,173,241,255));
        smgr->drawAll();
		video->endScene();
	}
	
	device->drop();
	
	return 0;
}

