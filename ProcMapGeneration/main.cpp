// ProcMapGeneration.cpp: Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "irrlicht.h"
#include <vector>
#include <map>
#include "MapControlEventReceiver.hpp"
#include "TerrainGenerator.hpp"

using namespace irr;
using namespace irr::scene;
using namespace irr::video;

const SIrrlichtCreationParameters CreationParameters() {
    SIrrlichtCreationParameters params;
    params.AntiAlias = true;
    params.DriverType = video::EDT_OPENGL;
    params.WindowSize = core::dimension2d<u32>(1920*0.75, 1080*0.75);
    params.Fullscreen = false;
    params.Vsync = true;
    params.WindowId = NULL;
    return params;
}

int main(int argc, char** argv) {
    auto eventReceiver = new MapControlEventReceiver();
    
	auto params = CreationParameters();
	IrrlichtDevice *device = createDeviceEx(params);
    device->setEventReceiver(eventReceiver);
    
	irr::core::stringw str = L"Procedural Terrain Generation";
	device->setWindowCaption(str.c_str());
	
	IVideoDriver* video = device->getVideoDriver();
    video->getMaterial2D().TextureLayer[0].BilinearFilter = true;
    video->getMaterial2D().AntiAliasing=video::EAAM_FULL_BASIC;
    
    ISceneManager* smgr = device->getSceneManager();
    
    float maxTerrainHeight = 20.f;
    float terrainQuadScaleX = .5f;
    float terrainQuadScaleY = .5f;
    int terrainWidth = 255;
    int terrainHeight = 255;
    bool terrainDebugBorders = true;
    
	//auto mainScene = smgr->addEmptySceneNode();
    
    
    auto ter2 = TerrainGenerator(irr::core::dimension2du{64,64}, 256.0, device);

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
    light->setLightType(video::ELT_POINT);
    player->addChild(light);
  



	while(device->run() && device) {
		auto x = eventReceiver->mouseInformation().x;
		auto screenW = (1920 * 0.75f);
		auto perc = x / screenW;
		auto angle = 360.f*2 * perc;
		float yVal = sin(irr::core::degToRad(angle));
		float xVal = cos(irr::core::degToRad(angle));

		if (eventReceiver->keyPressed(irr::KEY_RIGHT)) player->setPosition(player->getPosition() + irr::core::vector3df(cos(irr::core::degToRad(angle - 90.f)), 0.f, sin(irr::core::degToRad(angle - 90.f))));
		if (eventReceiver->keyPressed(irr::KEY_LEFT)) player->setPosition(player->getPosition() + irr::core::vector3df(cos(irr::core::degToRad(angle + 90.f)), 0.f, sin(irr::core::degToRad(angle  +90.f))));
		if (eventReceiver->keyPressed(irr::KEY_UP)) player->setPosition(player->getPosition() + irr::core::vector3df(xVal*1.f, 0.f, yVal*1.f));
		if (eventReceiver->keyPressed(irr::KEY_DOWN)) player->setPosition(player->getPosition() + irr::core::vector3df(cos(irr::core::degToRad(angle + 180.f)), 0.f, sin(irr::core::degToRad(angle + 180.f))));
		if (eventReceiver->keyPressed(irr::KEY_SPACE)) player->setPosition(player->getPosition() + irr::core::vector3df(0,1,0));
		if (eventReceiver->keyPressed(irr::KEY_KEY_X)) player->setPosition(player->getPosition() + irr::core::vector3df(0,-1,0));

		cam2->setTarget(player->getPosition());

		player->setRotation(irr::core::vector3df(0,-angle,0));
		cam2->setPosition(player->getPosition() + irr::core::vector3df(-10*xVal, 5, -10*yVal));

		auto playerChunkLoc = irr::core::vector2di(player->getPosition().X / 64.f / 6.f, player->getPosition().Z / 64.f / 6.f);

		for (auto ting : chunks) ting.second->setVisible(false);
		for (int y = -2; y <= 2; y++) {
			for (int x = -2; x <= 2; x++) {


				auto key = irr::core::vector2di(x, y) + playerChunkLoc;
				bool hasKey = false;
				for (auto ting : chunks) if (key.equals(ting.first)) hasKey = true;
				if (hasKey) chunks[key]->setVisible(true);
				else {
					auto m = ter2.getMeshAt(key);
					mainScene->addChild(m);
					chunks[key] = m;
				}

			}
		}

		video->beginScene(true, true, video::SColor(255,200,200,200));

        smgr->drawAll();
        
		video->endScene();
	}
	
	device->drop();
	
	return 0;
}
