//
//  WorldScene.cpp
//  ProcMapGeneration-macOS
//
//  Created by Tom Albrecht on 09.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#include "WorldScene.h"
#include "MapControlEventReceiver.h"
#include "TerrainShaderCallback.h"
#include "TerrainGenerator.h"
#include "Utils.h"

using namespace irr;
using namespace irr::scene;
using namespace irr::video;
using namespace irr::core;

static const char *kTreeTexturePath = "models/TreeTexture.png";
static const char *kBushTexturePath = "models/BushTexture.png";
static const char *kCloudTexturePath = "models/white.png";
static const char *kRockTexturePath = "models/rock.png";

WorldScene::WorldScene(irr::IrrlichtDevice *device) : IrrScene(device) {
    eventReceiver = new MapControlEventReceiver();
    device->setEventReceiver(eventReceiver);
    
    IVideoDriver* video = device->getVideoDriver();
    ISceneManager* smgr = device->getSceneManager();
    
    chunkSizeAB = 32.f;
    terrainGen = new TerrainGenerator(irr::core::dimension2du{chunkSizeAB,chunkSizeAB}, 175.0, device);
    chunkSizeAB *= 6.f;
    
    mainScene = smgr->addEmptySceneNode();
    
    player = smgr->addCubeSceneNode(2.5f);
    player->setScale(irr::core::vector3df{1, 1, 1});
    player->setPosition(irr::core::vector3df{400,400,400});
    
    cam2 = smgr->addCameraSceneNode();
    cam2->setPosition(irr::core::vector3df(0, 0, -10));
    mainScene->addChild(cam2);
    
    auto light = smgr->addLightSceneNode();
    light->setRadius(100.f);
    light->enableCastShadow();
    light->setLightType(video::ELT_DIRECTIONAL);
    light->setRotation(irr::core::vector3df(90, 0, 0));
    player->addChild(light);
    
    shaderMaterialIDS = tom::setupShader(device);
    
    eventReceiver->setPressedKeyHandler([this](irr::EKEY_CODE kc){
        if (kc == irr::KEY_KEY_N) {
            viewDistance++;
        } else if (kc == irr::KEY_KEY_M) {
            viewDistance--;
        } else if (kc == irr::KEY_KEY_R) {
            player->setPosition(core::vector3df{0,400,0});
        }  else if (kc == irr::KEY_SPACE) {
            if (player->getAnimators().size()) {
                auto c = dynamic_cast<ISceneNodeAnimatorCollisionResponse*>((*player->getAnimators().begin()));
                if (c && !c->isFalling()) {
                    c->jump(25);
                }
            }
        } else if (kc == irr::KEY_KEY_F) {
            if (player->getAnimators().size() == 0) {
                player->removeAnimators();
                ///----------
                auto playerChunkLoc = irr::core::vector2di(player->getPosition().X / (float)((int)chunkSizeAB-1), player->getPosition().Z / (float)((int)chunkSizeAB-1));
                printf("Chunks location: %d %d", playerChunkLoc.X, playerChunkLoc.Y);
                scene::ISceneNodeAnimator* anim = m_device->getSceneManager()->createCollisionResponseAnimator(chunks[playerChunkLoc]->getTriangleSelector(), player, core::vector3df(1.f,1.f,1.f),
                                                                                        core::vector3df(0,-10.f,0));
                player->addAnimator(anim);
                anim->drop();
                ///----------
            } else {
                player->removeAnimators();
            }
        }
    });
    
    lastCollisionLoc = vector2di{-101,-101};
    
    fpsTextElement = device->getGUIEnvironment()->addStaticText(L"", irr::core::rect<irr::s32>(25, 25, 140, 50), true, false, device->getGUIEnvironment()->getRootGUIElement(), 1001, true);
    viewDistanceElement = device->getGUIEnvironment()->addStaticText(L"", irr::core::rect<irr::s32>(25, 25+25, 140, 75), true, false, device->getGUIEnvironment()->getRootGUIElement(), 1002, true);
    lastFPS = 0;
}



void WorldScene::update(double dt) {
    tom::threading::manageMainthreadCallbacks();
    manageInput(eventReceiver, player, cam2);

    updateFPSCounter();
    
    auto playerChunkLoc = irr::core::vector2di(player->getPosition().X / ((int)chunkSizeAB-6.f), player->getPosition().Z / ((int)chunkSizeAB-6.f));
    bool hasKey = false;
    for (auto ting : chunks) if (playerChunkLoc.equals(ting.first)) hasKey = true;
    
    if (!playerChunkLoc.equals(lastCollisionLoc) && hasKey && !chunks[playerChunkLoc]->isDebugObject()) {
        lastCollisionLoc = playerChunkLoc;
        player->removeAnimators();
        ///----------
        printf("Chunks location: %d %d", playerChunkLoc.X, playerChunkLoc.Y);
        scene::ISceneNodeAnimator* anim = device()->getSceneManager()->createCollisionResponseAnimator(chunks[playerChunkLoc]->getTriangleSelector(), player, core::vector3df(2.5f),
                                                                                core::vector3df(0,-50.f,0), core::vector3df(0), 0.0005f);
        player->addAnimator(anim);
        anim->drop();
        ///----------
    }
    
    for (auto ting : chunks) if (ting.second) ting.second->setVisible(false);
    for (int y = -viewDistance; y <= viewDistance; y++) {
        for (int x = -viewDistance; x <= viewDistance; x++) {
            if (sqrt(pow(x, 2.f)+pow(y, 2.f)) > (float)viewDistance)  continue;
            
            auto key = irr::core::vector2di(x, y) + playerChunkLoc;
            bool hasKey = false;
            for (auto ting : chunks) if (key.equals(ting.first)) hasKey = true;
            if (hasKey) {
                chunks[key]->setVisible(true);
            } else {
                chunks[key] = device()->getSceneManager()->addCubeSceneNode();
                chunks[key]->setIsDebugObject(true);
                tom::threading::onSeparateThread([this, key = key]() mutable {
                    terrainGen->getMeshAt(key, std::bind(&WorldScene::terrainGenerationFinished, this, std::placeholders::_1, std::placeholders::_2));
                }); // on separate thread
            } // if (hasKey)   ELSE
        } // viewDistance X
    } // viewDistance Y
}

void WorldScene::render() {
    m_device->getVideoDriver()->beginScene(true, true, video::SColor(255,173,241,255));
    m_device->getSceneManager()->drawAll();
    device()->getGUIEnvironment()->drawAll();
    m_device->getVideoDriver()->endScene();
}



///
/// PRIVATE
///

void WorldScene::manageInput(MapControlEventReceiver *eventReceiver, irr::scene::ISceneNode *player, irr::scene::ICameraSceneNode *cam2) {
    auto x = eventReceiver->mouseInformation().x;
    auto screenW = (1920 * 0.75f);
    auto perc = x / screenW;
    auto angle = 360.f*2 * -perc;
    float yVal = sin(irr::core::degToRad(angle));
    float xVal = cos(irr::core::degToRad(angle));
    
    float value = eventReceiver->keyPressed(irr::KEY_KEY_E) ? 20.f : 0.5f;
    if (eventReceiver->keyPressed(irr::KEY_RIGHT) || eventReceiver->keyPressed(irr::KEY_KEY_D)) player->setPosition(player->getPosition() + irr::core::vector3df(value*cos(irr::core::degToRad(angle - 90.f)), 0.f, value*sin(irr::core::degToRad(angle - 90.f))));
    if (eventReceiver->keyPressed(irr::KEY_LEFT) || eventReceiver->keyPressed(irr::KEY_KEY_A)) player->setPosition(player->getPosition() + irr::core::vector3df(value*cos(irr::core::degToRad(angle + 90.f)), 0.f, value*sin(irr::core::degToRad(angle  +90.f))));
    if (eventReceiver->keyPressed(irr::KEY_UP) || eventReceiver->keyPressed(irr::KEY_KEY_W)) player->setPosition(player->getPosition() + irr::core::vector3df(value*xVal, 0.f, value*yVal));
    if (eventReceiver->keyPressed(irr::KEY_DOWN) || eventReceiver->keyPressed(irr::KEY_KEY_S)) player->setPosition(player->getPosition() + irr::core::vector3df(value*cos(irr::core::degToRad(angle + 180.f)), 0.f, value*sin(irr::core::degToRad(angle + 180.f))));
    if (eventReceiver->keyPressed(irr::KEY_SPACE)) player->setPosition(player->getPosition() + irr::core::vector3df(0,value,0));
    if (eventReceiver->keyPressed(irr::KEY_KEY_X)) player->setPosition(player->getPosition() + irr::core::vector3df(0,-value,0));
    
    cam2->setTarget(player->getPosition());
    
    player->setRotation(irr::core::vector3df(0,-angle,0));
    
    auto newPosition = player->getPosition() + irr::core::vector3df(-10 * xVal, 4.f, -10 * yVal);
    float lowpassfilterFactor = .045f;
    newPosition = (newPosition * lowpassfilterFactor) + (cam2->getPosition() * (1.0 - lowpassfilterFactor));
    cam2->setPosition(newPosition);
    
}

void WorldScene::updateFPSCounter() {
    if (lastFPS != device()->getVideoDriver()->getFPS()) {
        core::stringw str = L"FPS: ";
        str += device()->getVideoDriver()->getFPS();
        fpsTextElement->setText(str.c_str());
        lastFPS = device()->getVideoDriver()->getFPS();
    }
    core::stringw str = L"View distance: ";
    str += viewDistance;
    viewDistanceElement->setText(str.c_str());
}


void WorldScene::terrainGenerationFinished(irr::scene::IMeshSceneNode* m, irr::core::vector2di key) {
    mainScene->addChild(m);
    m->setMaterialType((video::E_MATERIAL_TYPE)shaderMaterialIDS.front());
    chunks[key]->remove();
    chunks[key] = m;
    
    scene::ITriangleSelector* selector = m_device->getSceneManager()->createOctreeTriangleSelector(m->getMesh(), m);
    m->setTriangleSelector(selector);
    
    auto mesh = m_device->getSceneManager()->getMesh("models/BigTreeWithLeaves.obj");
    auto mesh2 = m_device->getSceneManager()->getMesh("models/SmallTreeWithLeave.obj");
    auto meshBush = m_device->getSceneManager()->getMesh("models/BigBush.obj");
    
    for (auto i = 0; i < m->getMesh()->getMeshBuffer(0)->getVertexCount(); i++) {
        auto vertexPos = m->getMesh()->getMeshBuffer(0)->getPosition(i);
        
        // BigTreeWithLeaves.obj
        if (rand()%100 == 1 && vertexPos.Y > 10.f) {
            if (vertexPos.Y < 80.f) {
                auto ran = rand()%3;
                auto meshScene = m_device->getSceneManager()->addMeshSceneNode(ran == 0 ? mesh : ran==1 ? mesh2 : meshBush);
                meshScene->setPosition(vertexPos);
                meshScene->setScale(irr::core::vector3df{6.f});
                
                meshScene->setMaterialType((video::E_MATERIAL_TYPE)shaderMaterialIDS.at(1));
                meshScene->setRotation(irr::core::vector3df{0.f,(float)(rand()%360),0.f});
                
                auto img = m_device->getVideoDriver()->createImageFromFile(ran > 1 ? kBushTexturePath : kTreeTexturePath);
                meshScene->setMaterialTexture(0, m_device->getVideoDriver()->addTexture(ran > 1 ? kBushTexturePath : kTreeTexturePath, img));
                
                m->addChild(meshScene);
            } else if (rand()%100 == 5) {
                
            }
        }
        
        if (rand()%10000 == 123) {
            addCloud(vertexPos, m);
        } // Randcom clouds
    } // Vertex for loop
}

void WorldScene::addCloud(core::vector3df vertexPos, irr::scene::IMeshSceneNode *parent) {
    auto meshCloud1 = m_device->getSceneManager()->getMesh("models/Cloud1.obj");
    auto meshCloud2 = m_device->getSceneManager()->getMesh("models/Cloud2.obj");
    auto meshCloud3 = m_device->getSceneManager()->getMesh("models/Cloud3.obj");
    
    auto ran = rand()%3;
    auto meshScene = m_device->getSceneManager()->addMeshSceneNode(ran == 0 ? meshCloud1 : ran==1 ? meshCloud2 : meshCloud3);
    vertexPos.Y = 600.f;
    meshScene->setPosition(vertexPos);
    meshScene->setScale(irr::core::vector3df{70.f});
    meshScene->setRotation(irr::core::vector3df{0.f,(float)(rand()%360),0.f});
    meshScene->setMaterialFlag(video::EMF_GOURAUD_SHADING, false);
    meshScene->setMaterialType((video::E_MATERIAL_TYPE)shaderMaterialIDS.back());
    auto img = m_device->getVideoDriver()->createImageFromFile(kCloudTexturePath);
    meshScene->setMaterialTexture(0, m_device->getVideoDriver()->addTexture(kCloudTexturePath, img));
    parent->addChild(meshScene);
}

void WorldScene::addRock(core::vector3df vertexPos, irr::scene::IMeshSceneNode *parent) {
    auto meshRock1 = m_device->getSceneManager()->getMesh("models/Rock1.obj");

    auto ran = rand()%3;
    auto meshScene = m_device->getSceneManager()->addMeshSceneNode(ran == 0 ? meshRock1 : ran==1 ? meshRock1 : meshRock1);
    vertexPos.Y -= 6.f;
    meshScene->setPosition(vertexPos);
    meshScene->setScale(irr::core::vector3df{10.f});
    meshScene->setRotation(irr::core::vector3df{0.f,(float)(rand()%360),0.f});
    meshScene->setMaterialFlag(irr::video::EMF_GOURAUD_SHADING, false);
    meshScene->setMaterialType((video::E_MATERIAL_TYPE)shaderMaterialIDS.back());
    auto img = m_device->getVideoDriver()->createImageFromFile(kRockTexturePath);
    meshScene->setMaterialTexture(0, m_device->getVideoDriver()->addTexture(kRockTexturePath, img));
    parent->addChild(meshScene);
}
