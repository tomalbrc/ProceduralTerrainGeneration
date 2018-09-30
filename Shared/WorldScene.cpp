//
//  WorldScene.cpp
//  ProcMapGeneration
//
//  Created by Tom Albrecht on 09.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#include <unistd.h>
#include <chrono>

#include "WorldScene.h"
#include "TerrainShaderCallback.h"
#include "TerrainGenerator.h"
#include "Utils.h"
#include "IMetaTriangleSelector.h"

#include "XEffects.h"

#define PLAYER_PROPS worldInfo->entity->entities()[worldInfo->player]

using namespace irr;
using namespace irr::scene;
using namespace irr::video;
using namespace irr::core;
using namespace tom;

// Paths for irrlicht fonts (generated using FontTool)
static auto kFontPath1 = ResourcePath("fonts/betafont.xml");

// Paths for models
static auto kModelBigBushPath = ResourcePath("models/BigBush.obj");
static auto kModelSmallTreeWithLeavePath = ResourcePath("models/SmallTreeWithLeave.obj");
static auto kModelBigTreeWithLeavesPath = ResourcePath("models/BigTreeWithLeaves.obj");
static auto kModelRock1Path = ResourcePath("models/Rock1.obj");
static auto kModelCloud1Path = ResourcePath("models/Cloud1.obj");
static auto kModelCloud2Path = ResourcePath("models/Cloud2.obj");
static auto kModelCloud3Path = ResourcePath("models/Cloud3.obj");

// Paths for textures
static auto kTreeTexturePath = ResourcePath("models/TreeTexture.png");
static auto kBushTexturePath = ResourcePath("models/BushTexture.png");
static auto kCloudTexturePath = ResourcePath("models/white.png");
static auto kRockTexturePath = ResourcePath("models/rock.png");

static const irr::core::vector3df kPlayerSpawnPosition{5000.f, 400.f, 5000.f};

static const int kPlayerMaxAmmo = 1000.f;

WorldScene::WorldScene(irr::IrrlichtDevice *device) : IrrScene(device) {
    IVideoDriver* video = device->getVideoDriver();
    ISceneManager* smgr = device->getSceneManager();
    
    worldInfo = std::make_shared<WorldInfo>();
    worldInfo->setup(device, vector3df{0,-50,0});
    
    worldInfo->player = smgr->addCubeSceneNode(1.f);
    worldInfo->player->setScale(core::vector3df{1.5f,3.0f,1.5f}*3.f);
    LivingMetadata md;
    md.health = 100;
    md.ammo = kPlayerMaxAmmo;
    PLAYER_PROPS = md;
    
    worldInfo->camera = smgr->addCameraSceneNode();
    worldInfo->camera->setPosition(kPlayerSpawnPosition);
    worldInfo->camera->setFOV(20.f);
    worldInfo->mainScene->addChild(worldInfo->camera);
    
    auto light = smgr->addLightSceneNode();
    light->setRadius(10000.f);
    light->setLightType(video::ELT_POINT);
    light->setPosition(vector3df{1.f,7.f,1.5f});
    light->setName("light");
    worldInfo->player->addChild(light);
    
    shaderMaterialIDS = tom::shader::setupShader(device, vector2df(worldInfo->chunkSizeAB), worldInfo->quadScale, light);
    
    threading::onSeparateThread([dis = this]() mutable {
        threading::sleep(3000);
        threading::addMainCallback([dis = dis]() mutable {
            //dis->setupCollisionAnimator();
        });
    });
    
    auto effect = new EffectHandler(device, device->getVideoDriver()->getScreenSize(), false, false, true);
    effect->setAmbientColor(SColor(255, 200,200,200));
    effect->setClearColour(video::SColor(255,135,206,235));
    effect->addShadowLight(SShadowLight(1024*4, kPlayerSpawnPosition, vector3df{worldInfo->player->getPosition().X,100,worldInfo->player->getPosition().Z},
                                        SColor(255,255,240,241), 0.f, 1000.0f, 5000, true));
    effect->addShadowToNode(worldInfo->player, EFT_16PCF, ESM_BOTH);
    worldInfo->effectHandler = effect;
    
    worldInfo->physics->setPlayer(worldInfo->player->getMesh(), worldInfo->player);
    worldInfo->physics->warp(kPlayerSpawnPosition);
    
    worldInfo->sound->playBackgroundSound(ResourcePath("sounds/bg.ogg").c_str());
    worldInfo->sound->pauseBackgroundSound();
    
    worldInfo->input->setupKeyHandler();
    
    printf("GLSL #VERSION: %d\n", video->getDriverAttributes().getAttributeAsInt("ShaderLanguageVersion"));
}

void WorldScene::update(double dt) {
    worldInfo->physics->update(dt);
    worldInfo->input->update(dt);
    worldInfo->entity->update(dt);
    worldInfo->gui->update(dt);
    
    auto playerChunkLoc = irr::core::vector2di(worldInfo->player->getPosition().X / ((worldInfo->chunkSizeAB-1)*worldInfo->quadScale), worldInfo->player->getPosition().Z / ((worldInfo->chunkSizeAB - 1)*worldInfo->quadScale));
    
    for (auto ting : worldInfo->chunks) {
        if (ting.second) ting.second->setVisible(false);
    }
    
    for (int y = -worldInfo->viewDistance; y <= worldInfo->viewDistance; y++) {
        for (int x = -worldInfo->viewDistance; x <= worldInfo->viewDistance; x++) {
            if (sqrt(pow(x, 2.f)+pow(y, 2.f)) > (float)worldInfo->viewDistance)  continue;
            
            auto key = irr::core::vector2di(x, y) + playerChunkLoc;
            bool hasKey = false;
            for (auto ting : worldInfo->chunks) if (key.equals(ting.first)) hasKey = true;
            if (hasKey) {
                worldInfo->chunks[key]->setVisible(true);
            } else {
                worldInfo->chunks[key] = device()->getSceneManager()->addCubeSceneNode();
                worldInfo->chunks[key]->setIsDebugObject(true);
                tom::threading::onSeparateThread([this, key = key]() mutable {
                    worldInfo->terrainGen->getMeshAt(key, std::bind(&WorldScene::terrainGenerationFinished, this, std::placeholders::_1, std::placeholders::_2));
                }); // on separate thread
            } // if (hasKey) ... ELSE
        } // viewDistance X
    } // viewDistance Y
    
    // TODO: Move to EnemyManager
    // entity velocity application
    float friction = 0.1f;
    for (auto & entity : worldInfo->entity->entities()) {
        auto vel = entity.second.xzVelocity;
        vel *= friction;
        entity.second.xzVelocity -= vel;
        
        auto pos = entity.first->getPosition();
        pos.X += vel.X;
        pos.Z += vel.Y;
        
        // follow player
        if (entity.first->getID() == kEnemyID) {
            auto diff = worldInfo->player->getPosition()-entity.first->getPosition();
            auto angle = atan2(diff.X, diff.Z);
            pos += vector3df{sin(angle), 0, cos(angle)}*0.1f;
        } else if (entity.first->getID() == kItemID && entity.first->isVisible()) {
            if (worldInfo->player->getPosition().getDistanceFrom(entity.first->getPosition()) < 20.0f /*point is inside players radius*/) {
                entity.first->removeAnimators();
                
                pos = entity.first->getPosition().getInterpolated(worldInfo->player->getPosition(), 0.5f);
                
                // move item towards player
                
                if (worldInfo->player->getPosition().getDistanceFrom(entity.first->getPosition()) < 1.f) {
                    entity.first->setVisible(false);
                    PLAYER_PROPS.ammo += 20;
                }
            }
        }
        entity.first->setPosition(pos);
    }
    
    // TODO: Cleanup shadow of invisible chunks
//    for (auto ting : chunks) {
//            auto chldr = ting.second->getChildren();
//            for (auto i = chldr.begin(); i != chldr.end(); i++) {
//                if (!ting.second->isVisible()) {
//                    effect->removeShadowFromNode(*i);
//                    (*i)->setIsDebugObject(true);
//                }
//                else if ((*i)->isDebugObject()) {
//                    effect->addShadowToNode(*i);
//                    (*i)->setIsDebugObject(false);
//                }
//            }
//        if (!ting.second->isVisible()) {
//            effect->removeShadowFromNode(ting.second);
//            ting.second->setIsDebugObject(true);
//        }
//        else if (ting.second->isDebugObject()) {
//            effect->addShadowToNode(ting.second);
//            ting.second->setIsDebugObject(false);
//        }
//    }
}

void WorldScene::render() {
    m_device->getVideoDriver()->beginScene(false, false, video::SColor(255,80,160,250));
    worldInfo->effectHandler->update();

    if (worldInfo->input->eventReceiver()->mouseInformation().clickedLeft) raycast();

    device()->getGUIEnvironment()->drawAll();
    worldInfo->gui->render();
    
    m_device->getVideoDriver()->endScene();
}



///
/// PRIVATE
///

// Update to use irrBullet
void WorldScene::raycast() {
    if (PLAYER_PROPS.ammo <= 0) return;
    PLAYER_PROPS.ammo--;
    
	auto angle = worldInfo->player->getRotation().Y;
	auto loc = vector3df(cos(degToRad(-angle)), 0, sin(degToRad(-angle)));
    
    line3df ray;
    ray.start = worldInfo->player->getPosition();
    ray.end = worldInfo->player->getPosition() + loc*1000.f;
    
    vector3df collisionPoint;
    triangle3df selectedTriangle;
    
    auto collMan = device()->getSceneManager()->getSceneCollisionManager();
    auto selectedScene = collMan->getSceneNodeAndCollisionPointFromRay(ray, collisionPoint, selectedTriangle);
    
    if (selectedScene) {
        // We need to reset the transform before doing our own rendering.
        device()->getVideoDriver()->setTransform(video::ETS_WORLD, core::matrix4());
        device()->getVideoDriver()->draw3DTriangle(selectedTriangle, video::SColor(0,255,105,180));
        collisionPoint.Y += 0.001f;
        device()->getVideoDriver()->draw3DLine(worldInfo->player->getPosition(), collisionPoint);

		if (selectedScene->getID() == kEnemyID) {
            worldInfo->entity->entities()[selectedScene].health--;
            loc *= 30.f;
            worldInfo->entity->entities()[selectedScene].xzVelocity = vector2df{loc.X, loc.Z};
            
            if (worldInfo->entity->entities()[selectedScene].health <= 0) {
                worldInfo->entity->enemyDied(selectedScene, worldInfo->entity->entities()[selectedScene]);
            }
		}
    }
}


void WorldScene::terrainGenerationFinished(irr::scene::IMeshSceneNode* m, irr::core::vector2di key) {
    worldInfo->mainScene->addChild(m);

    worldInfo->physics->addGroundShape(m->getMesh(), m);
    
    m->setMaterialFlag(EMF_GOURAUD_SHADING, false);
    m->setMaterialType((video::E_MATERIAL_TYPE)shaderMaterialIDS.front());
    worldInfo->chunks[key]->remove();
    worldInfo->chunks[key] = m;

    worldInfo->effectHandler->addShadowToNode(m, EFT_16PCF, ESM_RECEIVE);

//    auto str = L"waterMesh";
//    auto waterMesh = device()->getSceneManager()->addHillPlaneMesh(str,
//                                                                   dimension2d<f32>(worldInfo->chunkSizeAB-1,worldInfo->chunkSizeAB-1),
//                                                                   dimension2d<u32>(1,1)*8, 0, 0,
//                                                                   dimension2d<f32>(0, 0),
//                                                                   dimension2d<f32>(8, 8)*2);
//    auto node = device()->getSceneManager()->addWaterSurfaceSceneNode(waterMesh->getMesh(0), 0.5f, 258, 1.0f);
//
//    node->setPosition(vector3df{127,8.5f,127});
//    node->setMaterialTexture(0, device()->getVideoDriver()->getTexture(ResourcePath("models/water.png")));
//    node->setMaterialType(EMT_SOLID);
//    m->addChild(node);
    
//    worldInfo->effectHandler->excludeNodeFromLightingCalculations(node);
    
    for (auto i = 0; i < m->getMesh()->getMeshBuffer(0)->getVertexCount(); i++) {
        auto vertexPos = m->getMesh()->getMeshBuffer(0)->getPosition(i);
        
        if (rand()%100 == 1 && vertexPos.Y > 11.f) {
            if (vertexPos.Y < 80.f) {
                addPlant(vertexPos, m);
            } else if (rand()%100 == 5) {
                addRock(vertexPos, m);
            }
        }
        
        if (rand()%1000 == 123) addCloud(vertexPos, m); // Random clouds
    } // Vertex for loop
}


void WorldScene::addPlant(core::vector3df vertexPos, irr::scene::ISceneNode *parent) {
    auto mesh = device()->getSceneManager()->getMesh(kModelBigTreeWithLeavesPath);
    auto mesh2 = device()->getSceneManager()->getMesh(kModelSmallTreeWithLeavePath);
    auto meshBush = device()->getSceneManager()->getMesh(kModelBigBushPath);
    
    auto ran = rand()%3;
    auto meshScene = device()->getSceneManager()->addMeshSceneNode(ran == 0 ? mesh : ran==1 ? mesh2 : meshBush);
    meshScene->setPosition(vertexPos);
    meshScene->setScale(irr::core::vector3df{ran==2?5.f:9.f});
    
    meshScene->setMaterialType((video::E_MATERIAL_TYPE)shaderMaterialIDS.at(1));
    meshScene->setRotation(irr::core::vector3df{0.f,(float)(rand()%360),0.f});
    
    auto img = device()->getVideoDriver()->createImageFromFile(ran > 1 ? kBushTexturePath : kTreeTexturePath);
    meshScene->setMaterialTexture(0, device()->getVideoDriver()->addTexture(ran > 1 ? kBushTexturePath : kTreeTexturePath, img));
    
    parent->addChild(meshScene);
    
    worldInfo->physics->addGroundShape(meshScene->getMesh(), meshScene);
    
    worldInfo->effectHandler->addShadowToNode(meshScene, EFT_16PCF, ESM_BOTH);
}

void WorldScene::addCloud(core::vector3df vertexPos, irr::scene::ISceneNode *parent) {
    auto meshCloud1 = device()->getSceneManager()->getMesh(kModelCloud1Path);
    auto meshCloud2 = device()->getSceneManager()->getMesh(kModelCloud2Path);
    auto meshCloud3 = device()->getSceneManager()->getMesh(kModelCloud3Path);
    
    auto ran = rand()%3;
    auto meshScene = device()->getSceneManager()->addMeshSceneNode(ran == 0 ? meshCloud1 : ran==1 ? meshCloud2 : meshCloud3);
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

void WorldScene::addRock(core::vector3df vertexPos, irr::scene::ISceneNode *parent) {
    auto meshRock1 = m_device->getSceneManager()->getMesh(kModelRock1Path);

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

    worldInfo->physics->addGroundShape(meshScene->getMesh(), meshScene);
}
