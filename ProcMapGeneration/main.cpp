// ProcMapGeneration.cpp: Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "irrlicht.h"
#include <vector>
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
    auto m1 = ter2.getMeshAt(irr::core::vector2di{0,0});
	auto tt1 = video->addTexture("myTeeext1", ter2._heightmap);
	auto itt1 = video->addTexture("myTeeext1", ter2._image);


    auto m2 = ter2.getMeshAt(irr::core::vector2di{1,0});
	auto tt2 = video->addTexture("myTeeext2", ter2._heightmap);
	auto itt2 = video->addTexture("myTeeext2", ter2._image);

    auto m3 = ter2.getMeshAt(irr::core::vector2di{-1,0});
	auto tt3 = video->addTexture("myTeeext3", ter2._heightmap);
	auto itt3 = video->addTexture("myTeeext3", ter2._image);

    auto m4 = ter2.getMeshAt(irr::core::vector2di{0,1});
	auto tt4 = video->addTexture("myTeeext4", ter2._heightmap);
	auto itt4 = video->addTexture("myTeeext4", ter2._image);

    auto m5 = ter2.getMeshAt(irr::core::vector2di{0,-1});
	auto tt5 = video->addTexture("myTeeext5", ter2._heightmap);
	auto itt5 = video->addTexture("myTeeext5", ter2._image);

    
    auto mainScene = smgr->addEmptySceneNode();
    mainScene->addChild(m1);
    mainScene->addChild(m2);
    mainScene->addChild(m3);
    mainScene->addChild(m4);
    mainScene->addChild(m5);

    std::vector<IMeshSceneNode*> meshScenes{m1,m2,m3,m4,m5};
    
    auto player = smgr->addCubeSceneNode(1);
	player->setScale(irr::core::vector3df{1, 1, 1});
	player->setPosition(irr::core::vector3df{32,32,5});
    
    
    // camera
    /*ICameraSceneNode *cam = smgr->addCameraSceneNodeMaya();
    cam->setPosition(irr::core::vector3df(0, 50, 10));
    cam->setTarget(m1->getBoundingBox().getCenter());
	mainScene->addChild(cam);
	*/
	ICameraSceneNode *cam2 = smgr->addCameraSceneNode();
	cam2->setPosition(irr::core::vector3df(0, 0, -10));
	mainScene->addChild(cam2);


    auto light = smgr->addLightSceneNode();
    light->setRadius(60.f);
    light->enableCastShadow();
    light->setLightType(video::ELT_POINT);
    player->addChild(light);
    


    // smoking hot lambda
    eventReceiver->setPressedKeyHandler([&](irr::EKEY_CODE kc){
        if (kc == irr::KEY_COMMA) {
            for (auto scene : meshScenes) {
                scene->setPosition(scene->getPosition()*3);
                scene->setScale(irr::core::vector3df{3.0});
            }
        } else if (kc == irr::KEY_DIVIDE) {
            for (auto scene : meshScenes) {
                scene->setPosition(scene->getPosition()/3);
                scene->setScale( irr::core::vector3df{1.0});
            }
        }
    });
    

	while(device->run() && device) {
		//manageInput(video, params.EventReceiver, cube);
		auto x = eventReceiver->mouseInformation().x;
		auto screenW = (1920 * 0.75f);
		auto perc = x / screenW;
		auto angle = 360.f * perc;
		float yVal = sin(irr::core::degToRad(angle));
		float xVal = cos(irr::core::degToRad(angle));

		if (eventReceiver->keyPressed(irr::KEY_RIGHT)) player->setPosition(player->getPosition() + irr::core::vector3df(cos(irr::core::degToRad(angle - 90.f)), 0.f, sin(irr::core::degToRad(angle - 90.f))));
		if (eventReceiver->keyPressed(irr::KEY_LEFT)) player->setPosition(player->getPosition() + irr::core::vector3df(cos(irr::core::degToRad(angle + 90.f)), 0.f, sin(irr::core::degToRad(angle  +90.f))));
		if (eventReceiver->keyPressed(irr::KEY_UP)) player->setPosition(player->getPosition() + irr::core::vector3df(xVal*1.f, 0.f, yVal*1.f));
		if (eventReceiver->keyPressed(irr::KEY_DOWN)) player->setPosition(player->getPosition() + irr::core::vector3df(cos(irr::core::degToRad(angle + 180.f)), 0.f, sin(irr::core::degToRad(angle + 180.f))));
		if (eventReceiver->keyPressed(irr::KEY_SPACE)) player->setPosition(player->getPosition() + irr::core::vector3df(0,1,0));
		if (eventReceiver->keyPressed(irr::KEY_KEY_X)) player->setPosition(player->getPosition() + irr::core::vector3df(0,-1,0));

		auto vect = player->getBoundingBox().getCenter();
		player->getAbsoluteTransformation().transformVect(vect);
		cam2->setTarget(vect);

		player->setRotation(irr::core::vector3df(0,-angle,0));
		cam2->setPosition(vect + irr::core::vector3df(-10*xVal, 5, -10*yVal));


		video->beginScene(false, true, video::SColor(255,200,200,200));

		video->draw2DImage(tt5, core::vector2d<s32>{64, 0});
		video->draw2DImage(tt3, core::vector2d<s32>{0, 64});
		video->draw2DImage(tt1, core::vector2d<s32>{64, 64});
		video->draw2DImage(tt2, core::vector2d<s32>{128, 64});
		video->draw2DImage(tt4, core::vector2d<s32>{64, 128});


		video->draw2DImage(itt5, core::vector2d<s32>{-10 + 192 + 64*3, 0*3});
		video->draw2DImage(itt3, core::vector2d<s32>{-10 + 192 + 0*3, 64*3});
		video->draw2DImage(itt1, core::vector2d<s32>{-10 + 192 + 64*3, 64*3});
		video->draw2DImage(itt2, core::vector2d<s32>{-10 + 192 + 128*3, 64*3});
		video->draw2DImage(itt4, core::vector2d<s32>{-10 + 192 + 64*3, 128*3});

        smgr->drawAll();
        
		video->endScene();
	}
	
	device->drop();
	
	return 0;
}

