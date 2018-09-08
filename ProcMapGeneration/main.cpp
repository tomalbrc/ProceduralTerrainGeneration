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

#include "post.h"

using namespace irr;
using namespace irr::scene;
using namespace irr::video;

void manageInput(MapControlEventReceiver *eventReceiver, irr::scene::ISceneNode *player, irr::scene::ICameraSceneNode *cam2) {
    auto x = eventReceiver->mouseInformation().x;
    auto screenW = (1920 * 0.75f);
    auto perc = x / screenW;
    auto angle = 360.f*2 * -perc;
    float yVal = sin(irr::core::degToRad(angle));
    float xVal = cos(irr::core::degToRad(angle));
    
    float value = 100.f;
    if (eventReceiver->keyPressed(irr::KEY_RIGHT) || eventReceiver->keyPressed(irr::KEY_KEY_D)) player->setPosition(player->getPosition() + irr::core::vector3df(value*cos(irr::core::degToRad(angle - 90.f)), 0.f, value*sin(irr::core::degToRad(angle - 90.f))));
    if (eventReceiver->keyPressed(irr::KEY_LEFT) || eventReceiver->keyPressed(irr::KEY_KEY_A)) player->setPosition(player->getPosition() + irr::core::vector3df(value*cos(irr::core::degToRad(angle + 90.f)), 0.f, value*sin(irr::core::degToRad(angle  +90.f))));
    if (eventReceiver->keyPressed(irr::KEY_UP) || eventReceiver->keyPressed(irr::KEY_KEY_W)) player->setPosition(player->getPosition() + irr::core::vector3df(value*xVal, 0.f, value*yVal));
    if (eventReceiver->keyPressed(irr::KEY_DOWN) || eventReceiver->keyPressed(irr::KEY_KEY_S)) player->setPosition(player->getPosition() + irr::core::vector3df(value*cos(irr::core::degToRad(angle + 180.f)), 0.f, value*sin(irr::core::degToRad(angle + 180.f))));
    if (eventReceiver->keyPressed(irr::KEY_SPACE)) player->setPosition(player->getPosition() + irr::core::vector3df(0,1,0));
    if (eventReceiver->keyPressed(irr::KEY_KEY_X)) player->setPosition(player->getPosition() + irr::core::vector3df(0,-1,0));
    
    cam2->setTarget(player->getPosition());
    
    player->setRotation(irr::core::vector3df(0,-angle,0));
    
    auto newPosition = player->getPosition() + irr::core::vector3df(-10 * xVal, 4.f, -10 * yVal);
    float lowpassfilterFactor = .045f;
    newPosition = (newPosition * lowpassfilterFactor) + (cam2->getPosition() * (1.0 - lowpassfilterFactor));
    cam2->setPosition(newPosition);

}

int main(int argc, char** argv) {
    auto eventReceiver = new MapControlEventReceiver();

    auto params = []() -> const SIrrlichtCreationParameters {
        SIrrlichtCreationParameters params;
        params.AntiAlias = 2;
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
    ISceneManager* smgr = device->getSceneManager();
    
    float chunkSizeAB = 32.f;
    auto ter2 = TerrainGenerator(irr::core::dimension2du{chunkSizeAB,chunkSizeAB}, 175.0, device);
    chunkSizeAB *= 6.f;
    
	auto mainScene = smgr->addEmptySceneNode();
    
    // chunk cache
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
  
    
    auto shaderMaterialIDS = tom::setupShader(device);
    
	int viewDistance = 8; // view distance as radius in chunks
    eventReceiver->setPressedKeyHandler([&viewDistance](irr::EKEY_CODE kc){
        if (kc == irr::KEY_KEY_N) {
            viewDistance++;
        } else if (kc == irr::KEY_KEY_M) {
            viewDistance--;
        }
    });
    
    u32 then = device->getTimer()->getTime();
	while(device->run() && device) {
        // Work out a frame delta time.
        const u32 now = device->getTimer()->getTime();
        f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
        then = now;
        
        frameDeltaTime = 0.02f;
        
        tom::threading::manageMainthreadCallbacks();
        
        manageInput(eventReceiver, player, cam2);

        auto playerChunkLoc = irr::core::vector2di(player->getPosition().X / (chunkSizeAB-1), player->getPosition().Z / (chunkSizeAB-1));

		for (auto ting : chunks) ting.second->setVisible(false);
		for (int y = -viewDistance; y <= viewDistance; y++) {
			for (int x = -viewDistance; x <= viewDistance; x++) {
                if (sqrt(pow(x, 2.f)+pow(y, 2.f)) > (float)viewDistance)  continue;

				auto key = irr::core::vector2di(x, y) + playerChunkLoc;
				bool hasKey = false;
				for (auto ting : chunks) if (key.equals(ting.first)) hasKey = true;
				if (hasKey && chunks[key]) chunks[key]->setVisible(true);
				else {
                    chunks[key] = smgr->addCubeSceneNode();
                    tom::threading::onSeparateThread([video=video, smgr = smgr/* <-use in main only! */, ter2 = std::ref(ter2), mainScene = mainScene, chunks = std::ref(chunks), key = key, shaderMaterialIDS = shaderMaterialIDS]() mutable {
                        ter2.get().getMeshAt(key, [video=video, smgr = smgr/* <-use in main only! */, mainScene = mainScene, chunks = std::ref(chunks), key = key, shaderMaterialIDS = shaderMaterialIDS](irr::scene::IMeshSceneNode* m) mutable {
                            mainScene->addChild(m);
                            m->setMaterialType((video::E_MATERIAL_TYPE)shaderMaterialIDS.front());
                            chunks.get()[key]->remove();
                            chunks.get()[key] = m;
                            
                            auto mesh = smgr->getMesh("BigTreeWithLeaves.obj");
                            auto mesh2 = smgr->getMesh("SmallTreeWithLeave.obj");
                            auto meshBush = smgr->getMesh("BigBush.obj");
                            auto meshCloud1 = smgr->getMesh("Cloud1.obj");
                            auto meshCloud2 = smgr->getMesh("Cloud2.obj");
                            auto meshCloud3 = smgr->getMesh("Cloud3.obj");

                            for (auto i = 0; i < m->getMesh()->getMeshBuffer(0)->getVertexCount(); i++) {
                                auto vertexPos = m->getMesh()->getMeshBuffer(0)->getPosition(i);
                                
                                // BigTreeWithLeaves.obj
                                if (rand()%100 == 1 && vertexPos.Y > 10.f && vertexPos.Y < 80.f) {
                                    auto ran = rand()%3;
                                    auto meshScene = smgr->addMeshSceneNode(ran == 0 ? mesh : ran==1 ? mesh2 : meshBush);
                                    meshScene->setPosition(vertexPos);
                                    meshScene->setScale(irr::core::vector3df{6.f});
                                    
                                    meshScene->setMaterialType((video::E_MATERIAL_TYPE)shaderMaterialIDS.at(1));
                                    meshScene->setRotation(irr::core::vector3df{0.f,(float)(rand()%360),0.f});
                                    
                                    auto img = video->createImageFromFile(ran > 1 ? "BushTexture.png" : "TreeTexture.png");
                                    meshScene->setMaterialTexture(0, video->addTexture(ran > 1 ? "BushTexture" : "TreeTexture", img));
                                    
                                    m->addChild(meshScene);
                                }
                                
                                if (rand()%10000 == 123) {
                                    auto ran = rand()%3;
                                    auto meshScene = smgr->addMeshSceneNode(ran == 0 ? meshCloud1 : ran==1 ? meshCloud2 : meshCloud3);
                                    vertexPos.Y = 600.f;
                                    meshScene->setPosition(vertexPos);
                                    meshScene->setScale(irr::core::vector3df{70.f});
                                    meshScene->setRotation(irr::core::vector3df{0.f,(float)(rand()%360),0.f});
                                    meshScene->setMaterialFlag(video::EMF_GOURAUD_SHADING, false);
                                    meshScene->setMaterialType((video::E_MATERIAL_TYPE)shaderMaterialIDS.back());
                                    auto img = video->createImageFromFile("white.png");
                                    meshScene->setMaterialTexture(0, video->addTexture("white", img));
                                    m->addChild(meshScene);
                                }
                                
                            }
                            
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

