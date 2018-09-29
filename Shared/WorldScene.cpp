//
//  WorldScene.cpp
//  ProcMapGeneration-macOS
//
//  Created by Tom Albrecht on 09.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#include <unistd.h>

#include "WorldScene.h"
#include "MapControlEventReceiver.h"
#include "TerrainShaderCallback.h"
#include "TerrainGenerator.h"
#include "Utils.h"
#include "IMetaTriangleSelector.h"

#include "XEffects.h"

#define PLAYER_PROPS entities[player]

using namespace irr;
using namespace irr::scene;
using namespace irr::video;
using namespace irr::core;

// Paths for irrlicht fonts (generated using FontTool)
static auto kFontPath1 = getResourcePath("fonts/betafont.xml");
static auto kFontPath = getResourcePath("fonts/betafont.xml");

// Paths for models
static auto kModelBigBushPath = getResourcePath("models/BigBush.obj");
static auto kModelSmallTreeWithLeavePath = getResourcePath("models/SmallTreeWithLeave.obj");
static auto kModelBigTreeWithLeavesPath = getResourcePath("models/BigTreeWithLeaves.obj");
static auto kModelRock1Path = getResourcePath("models/Rock1.obj");
static auto kModelCloud1Path = getResourcePath("models/Cloud1.obj");
static auto kModelCloud2Path = getResourcePath("models/Cloud2.obj");
static auto kModelCloud3Path = getResourcePath("models/Cloud3.obj");

// Paths for textures
static auto kTreeTexturePath = getResourcePath("models/TreeTexture.png");
static auto kBushTexturePath = getResourcePath("models/BushTexture.png");
static auto kCloudTexturePath = getResourcePath("models/white.png");
static auto kRockTexturePath = getResourcePath("models/rock.png");

// Ids for scenes
static const irr::s32 kEnemyID = -1344;
static const irr::s32 kItemID = -1345;

static const irr::core::vector3df kPlayerSpawnPosition{5000.f, 400.f, 5000.f};

static const int kPlayerMaxAmmo = 1000.f;

void setupPhysics() {
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    
    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
    
    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
    
    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
    
    btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    
    dynamicsWorld->setGravity(btVector3(0, -10, 0));
}
void addBody(btAlignedObjectArray<btCollisionShape*> & collisionShapes) {
    //keep track of the shapes, we release memory at exit.
    //make sure to re-use collision shapes among rigid bodies whenever possible!
    
    ///create a few basic rigid bodies
    
    //the ground is a cube of side 100 at position y = -56.
    //the sphere will hit it at y = -6, with center at -5
    {
        btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
        
        collisionShapes.push_back(groundShape);
        
        btTransform groundTransform;
        groundTransform.setIdentity();
        groundTransform.setOrigin(btVector3(0, -56, 0));
        
        btScalar mass(0.);
        
        //rigidbody is dynamic if and only if mass is non zero, otherwise static
        bool isDynamic = (mass != 0.f);
        
        btVector3 localInertia(0, 0, 0);
        if (isDynamic)
            groundShape->calculateLocalInertia(mass, localInertia);
        
        //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
        btRigidBody* body = new btRigidBody(rbInfo);
        
        //add the body to the dynamics world
        dynamicsWorld->addRigidBody(body);
    }
    
    {
        //create a dynamic rigidbody
        
        //btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
        btCollisionShape* colShape = new btSphereShape(btScalar(1.));
        collisionShapes.push_back(colShape);
        
        /// Create Dynamic Objects
        btTransform startTransform;
        startTransform.setIdentity();
        
        btScalar mass(1.f);
        
        //rigidbody is dynamic if and only if mass is non zero, otherwise static
        bool isDynamic = (mass != 0.f);
        
        btVector3 localInertia(0, 0, 0);
        if (isDynamic)
            colShape->calculateLocalInertia(mass, localInertia);
        
        startTransform.setOrigin(btVector3(2, 10, 0));
        
        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
        btRigidBody* body = new btRigidBody(rbInfo);
        
        dynamicsWorld->addRigidBody(body);
    }
}

WorldScene::WorldScene(irr::IrrlichtDevice *device) : IrrScene(device) {
    eventReceiver = new MapControlEventReceiver();
    device->setEventReceiver(eventReceiver);
    
    quadScale = 1.f;
    chunkSizeAB = 128.f;
    viewDistance = 3.f;
    
    IVideoDriver* video = device->getVideoDriver();
    ISceneManager* smgr = device->getSceneManager();
    
    terrainGen = new TerrainGenerator(irr::core::dimension2du{(unsigned)chunkSizeAB, (unsigned)chunkSizeAB}, quadScale, 30, device);
    
    worldTriangleSelector = this->device()->getSceneManager()->createMetaTriangleSelector();
    mainScene = smgr->addEmptySceneNode();
    
    player = smgr->addCubeSceneNode(1.f);
    player->setScale(core::vector3df{1.5f,3.0f,1.5f}*3.f);
    player->setPosition(kPlayerSpawnPosition);
    LivingMetadata md;
    md.health = 100;
    md.ammo = kPlayerMaxAmmo;
    PLAYER_PROPS = md;
    
    cam2 = smgr->addCameraSceneNode();
    cam2->setPosition(kPlayerSpawnPosition);
    cam2->setFOV(20.f);
    mainScene->addChild(cam2);
    
    auto light = smgr->addLightSceneNode();
    light->setRadius(10000.f);
    light->setLightType(video::ELT_POINT);
    light->setPosition(vector3df{1.f,7.f,1.5f});
    light->setName("light");
    player->addChild(light);
    
    shaderMaterialIDS = tom::setupShader(device, vector2df(chunkSizeAB), quadScale, light);
    
    // give player a collision animator with worldTriangleSelector as tri selector
    setupCollisionAnimator();
    
    eventReceiver->setPressedKeyHandler([this, device](irr::EKEY_CODE kc){
        if (kc == irr::KEY_KEY_N) {
            viewDistance++;
        } else if (kc == irr::KEY_KEY_M) {
            viewDistance--;
        } else if (kc == irr::KEY_F1) {
            player->setPosition(kPlayerSpawnPosition);
			cam2->setPosition(kPlayerSpawnPosition);
        }  else if (kc == irr::KEY_SPACE) {
            if (player->getAnimators().size()) {
                auto c = dynamic_cast<ISceneNodeAnimatorCollisionResponse*>((*player->getAnimators().begin()));
                if (c && !c->isFalling()) {
                    c->jump(3);
                }
            }
        } else if (kc == irr::KEY_F2) {
            if (player->getAnimators().size() == 0) {
                this->setupCollisionAnimator();
            } else {
                player->removeAnimators();
            }
        } else if (kc == irr::KEY_F4) {
            
            for (auto & node : chunks) {
                auto chldr = node.second->getChildren();
                for (auto i = chldr.begin(); i != chldr.end(); i++) {
                    effect->removeShadowFromNode(*i);
                    (*i)->remove();
                }
                
                effect->removeShadowFromNode(node.second);
                node.second->remove();
            }
            chunks.clear();
            
        } else if (kc == irr::KEY_KEY_P) {
			spawnEnemies();
		} else if (kc == irr::KEY_KEY_R) {
            entities[player].ammo = 1000;
        } else if (kc == irr::KEY_ESCAPE) {
            device->closeDevice();
        }
    });
    
    fpsTextElement = device->getGUIEnvironment()->addStaticText(L"", irr::core::rect<irr::s32>(5, 5, 140, 30), false, false, device->getGUIEnvironment()->getRootGUIElement(), 1001, true);
    viewDistanceElement = device->getGUIEnvironment()->addStaticText(L"", irr::core::rect<irr::s32>(5, 5+25, 140, 45), false, false, device->getGUIEnvironment()->getRootGUIElement(), 1002, true);
    coordsElement = device->getGUIEnvironment()->addStaticText(L"", irr::core::rect<irr::s32>(5, 5+25+25, 140+115, 70), false, false, device->getGUIEnvironment()->getRootGUIElement(), 1003, true);
    
    font = device->getGUIEnvironment()->getFont(kFontPath);
    
    // Setup the material to color a triangle in WorldScene::raycast()
    triangleMaterial.Lighting = false;
	triangleMaterial.Wireframe = false;
    
    effect = new EffectHandler(device, device->getVideoDriver()->getScreenSize(), false, false, true);
    effect->setAmbientColor(SColor(255, 200,200,200));
    effect->setClearColour(video::SColor(255,135,206,235));
    effect->addShadowLight(SShadowLight(1024*4, kPlayerSpawnPosition, vector3df{player->getPosition().X,100,player->getPosition().Z},
                                        SColor(255,255,240,241), 1.f, 1000.0f, 1200, true));
    effect->addShadowToNode(player, EFT_16PCF, ESM_BOTH);

    printf("GLSL #VERSION: %d\n", video->getDriverAttributes().getAttributeAsInt("ShaderLanguageVersion"));
    
    btAlignedObjectArray<btCollisionShape*> collisionShapes;
    addBody();
}

void WorldScene::update(double dt) {
    manageInput(eventReceiver, player, cam2, dt);
    updateFPSCounter();
    
    auto &lll2 = effect->getShadowLight(0);
    auto pos = player->getPosition();
    pos.Y += 90.f;
    pos.X -= 30.f;
    pos.Z -= 30.f;
    lll2.setPosition(pos);
    lll2.setTarget(player->getPosition());
    
    auto playerChunkLoc = irr::core::vector2di(player->getPosition().X / ((chunkSizeAB-1)*quadScale), player->getPosition().Z / ((chunkSizeAB - 1)*quadScale));
    
    for (auto ting : chunks) {
        if (ting.second) ting.second->setVisible(false);
    }
    
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
            } // if (hasKey) ... ELSE
        } // viewDistance X
    } // viewDistance Y
    
//
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
    //m_device->getSceneManager()->drawAll(); // disabled in favor of effect->update()
    effect->update();

    if (eventReceiver->mouseInformation().clickedLeft) raycast();

    device()->getGUIEnvironment()->drawAll();
    
    auto str = L"AMMO: " + std::to_wstring(PLAYER_PROPS.ammo) + L" / " + std::to_wstring(kPlayerMaxAmmo);
    font->draw(str.c_str(), core::rect<s32>{static_cast<int>(device()->getVideoDriver()->getScreenSize().Width-500),0,0,0}, video::SColor{255,0,0,0});
    //if (currentChunkNoiseTex != nullptr) m_device->getVideoDriver()->draw2DImage(currentChunkNoiseTex, vector2d<s32>{5,5});
    
    m_device->getVideoDriver()->endScene();
}



///
/// PRIVATE
///

void WorldScene::manageInput(MapControlEventReceiver *eventReceiver, irr::scene::ISceneNode *player, irr::scene::ICameraSceneNode *cam2, f32 deltaTime) {
    auto x = eventReceiver->mouseInformation().x;
    auto screenW = (1920 * 0.75f);
    auto perc = x / screenW;
    auto angle = 360.f*2 * -perc;
    float yVal = sin(irr::core::degToRad(angle));
    float xVal = cos(irr::core::degToRad(angle));
    
    float value = (eventReceiver->keyPressed(irr::KEY_SHIFT) ? 10.f : 0.5f)*100.f * deltaTime;
    if (eventReceiver->keyPressed(irr::KEY_RIGHT) || eventReceiver->keyPressed(irr::KEY_KEY_D)) player->setPosition(player->getPosition() + irr::core::vector3df(value*cos(irr::core::degToRad(angle - 90.f)), 0.f, value*sin(irr::core::degToRad(angle - 90.f))));
    if (eventReceiver->keyPressed(irr::KEY_LEFT) || eventReceiver->keyPressed(irr::KEY_KEY_A)) player->setPosition(player->getPosition() + irr::core::vector3df(value*cos(irr::core::degToRad(angle + 90.f)), 0.f, value*sin(irr::core::degToRad(angle  +90.f))));
    if (eventReceiver->keyPressed(irr::KEY_UP) || eventReceiver->keyPressed(irr::KEY_KEY_W)) player->setPosition(player->getPosition() + irr::core::vector3df(value*xVal, 0.f, value*yVal));
    if (eventReceiver->keyPressed(irr::KEY_DOWN) || eventReceiver->keyPressed(irr::KEY_KEY_S)) player->setPosition(player->getPosition() + irr::core::vector3df(value*cos(irr::core::degToRad(angle + 180.f)), 0.f, value*sin(irr::core::degToRad(angle + 180.f))));
    
    cam2->setTarget(player->getPosition());
    
    player->setRotation(irr::core::vector3df(0,-angle,0));
    
    auto newPosition = player->getPosition() + irr::core::vector3df(-50 * xVal, 20.f, -50 * yVal);
    float lowpassfilterFactor = (.1f);
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
    
    core::stringw str2 = L"Coords: ";
    str2 += player->getPosition().X;
    str2 += L", ";
    str2 += player->getPosition().Y;
    str2 += L", ";
    str2 += player->getPosition().Z;
    coordsElement->setText(str2.c_str());
    
    // TODO: Move to update()
    // Apply enemy/player xz velocity (gravity & collision is taken care of by ISceneCollisionAnimator)
    float friction = 0.1f;
    for (auto & entity : entities) {
        auto vel = entity.second.xzVelocity;
        vel *= friction;
        entity.second.xzVelocity -= vel;
        
        auto pos = entity.first->getPosition();
        pos.X += vel.X;
        pos.Z += vel.Y;
        
        // follow player
        if (entity.first->getID() == kEnemyID) {
            auto diff = player->getPosition()-entity.first->getPosition();
            auto angle = atan2(diff.X, diff.Z);
            pos += vector3df{sin(angle), 0, cos(angle)}*0.1f;
        } else if (entity.first->getID() == kItemID && entity.first->isVisible()) {
            if (player->getPosition().getDistanceFrom(entity.first->getPosition()) < 20.0f /*point is inside players radius*/) {
                entity.first->removeAnimators();
                
                pos = entity.first->getPosition().getInterpolated(player->getPosition(), 0.5f);
                
                // move item towards player
                
                if (player->getPosition().getDistanceFrom(entity.first->getPosition()) < 1.f) {
                    entity.first->setVisible(false);
                    PLAYER_PROPS.ammo += 20;
                }
            }
        }
        entity.first->setPosition(pos);
    }
}

void WorldScene::setupCollisionAnimator(irr::scene::ISceneNode *target) {
	if (target == nullptr) {
		target = player;
	}
	target->removeAnimators();
	///----------
    const core::aabbox3d<f32>& box = target->getBoundingBox();
    core::vector3df radius = box.MaxEdge - box.getCenter();
    radius *= target->getScale();
    
	scene::ISceneNodeAnimator* anim = device()->getSceneManager()->createCollisionResponseAnimator(worldTriangleSelector, target, radius,
		core::vector3df(0, -10.f, 0), core::vector3df(0), .01f);
	target->addAnimator(anim);
	anim->drop();
	///----------
}

void WorldScene::raycast() {
    if (PLAYER_PROPS.ammo <= 0) return;
    PLAYER_PROPS.ammo--;
    
    
	auto angle = player->getRotation().Y;
	auto loc = vector3df(cos(degToRad(-angle)), 0, sin(degToRad(-angle)));
    
    line3df ray;
    ray.start = player->getPosition();
    ray.end = player->getPosition() + loc*1000.f;
    
    vector3df collisionPoint;
    triangle3df selectedTriangle;
    
    auto collMan = device()->getSceneManager()->getSceneCollisionManager();
    auto selectedScene = collMan->getSceneNodeAndCollisionPointFromRay(ray, collisionPoint, selectedTriangle);
    
    if (selectedScene) {
        // We need to reset the transform before doing our own rendering.
        device()->getVideoDriver()->setTransform(video::ETS_WORLD, core::matrix4());
        device()->getVideoDriver()->setMaterial(triangleMaterial);
        device()->getVideoDriver()->draw3DTriangle(selectedTriangle, video::SColor(0,255,105,180));
        collisionPoint.Y += 0.001f;
        device()->getVideoDriver()->draw3DLine(player->getPosition(), collisionPoint);

		if (selectedScene->getID() == kEnemyID) {
            entities[selectedScene].health--;
            loc *= 30.f;
            entities[selectedScene].xzVelocity = vector2df{loc.X, loc.Z};
            
            if (entities[selectedScene].health <= 0) {
                enemyDied(selectedScene, entities[selectedScene]);
            }
		}
    }
}


void WorldScene::terrainGenerationFinished(irr::scene::IMeshSceneNode* m, irr::core::vector2di key) {
    mainScene->addChild(m);
    if (m->getMaterial(0).getTexture(0) != nullptr) currentChunkNoiseTex = m->getMaterial(0).getTexture(0);

    m->setMaterialType((video::E_MATERIAL_TYPE)shaderMaterialIDS.front());
    chunks[key]->remove();
    chunks[key] = m;
    
    scene::ITriangleSelector* selector = device()->getSceneManager()->createTriangleSelector(m->getMesh(), m);
    m->setTriangleSelector(selector);
    worldTriangleSelector->addTriangleSelector(selector);
    selector->drop();

    effect->addShadowToNode(m, EFT_16PCF, ESM_RECEIVE);

    
    auto str = L"waterMesh";
    auto waterMesh = device()->getSceneManager()->addHillPlaneMesh(str,
                                                                   dimension2d<f32>(chunkSizeAB-1,chunkSizeAB-1),
                                                                   dimension2d<u32>(2,2), 0, 0,
                                                                   dimension2d<f32>(0, 0),
                                                                   dimension2d<f32>(8, 8));
    auto node = device()->getSceneManager()->addWaterSurfaceSceneNode(waterMesh->getMesh(0), 0.1f, 258, 1.0f);

    node->setPosition(vector3df{127,5.5f,127});
    node->setMaterialTexture(0, device()->getVideoDriver()->getTexture(getResourcePath("models/water.png")));
    node->setMaterialType(EMT_SOLID);
    m->addChild(node);
    
    effect->excludeNodeFromLightingCalculations(node);
    
    for (auto i = 0; i < m->getMesh()->getMeshBuffer(0)->getVertexCount(); i++) {
        auto vertexPos = m->getMesh()->getMeshBuffer(0)->getPosition(i);
        
        if (rand()%4000 == 1 && vertexPos.Y > 11.f) {
            if (vertexPos.Y < 80.f) {
                addPlant(vertexPos, m);
            } else if (rand()%1800 == 5) {
                addRock(vertexPos, m);
            }
        }
        
        if (rand()%90000 == 123) addCloud(vertexPos, m); // Random clouds
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
    
    if (ran != 2) {
        scene::ITriangleSelector* selector = device()->getSceneManager()->createTriangleSelectorFromBoundingBox(meshScene);
        meshScene->setTriangleSelector(selector);
        worldTriangleSelector->addTriangleSelector(selector);
    }
    
    effect->addShadowToNode(meshScene, EFT_16PCF, ESM_BOTH);
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
    
    scene::ITriangleSelector* selector = device()->getSceneManager()->createTriangleSelectorFromBoundingBox(meshScene);
    meshScene->setTriangleSelector(selector);
    worldTriangleSelector->addTriangleSelector(selector);
}


void WorldScene::spawnEnemies() {
	auto spawnPos = player->getPosition();
	spawnPos.Y += 10;

	auto enemy = device()->getSceneManager()->addSphereSceneNode(5.f, 8, mainScene, kEnemyID);
	enemy->setPosition(spawnPos);
	mainScene->addChild(enemy);

    enemy->setMaterialFlag(EMF_LIGHTING, false);
    effect->addShadowToNode(enemy, EFT_16PCF, ESM_BOTH);

    scene::ITriangleSelector* selector = device()->getSceneManager()->createOctreeTriangleSelector(enemy->getMesh(), enemy);
    enemy->setTriangleSelector(selector);
    //worldTriangleSelector->addTriangleSelector(selector);

    setupCollisionAnimator(enemy);

    LivingMetadata md;
    md.health = 50;
    
    entities[enemy] = md;
}

void WorldScene::enemyDied(irr::scene::ISceneNode *node, const WorldScene::LivingMetadata & metadata) {
    effect->removeShadowFromNode(node);
    
    auto nodePos = node->getPosition();
    
    auto iter = entities.find(node) ;
    if (iter != entities.end())
        entities.erase(iter);
    
    worldTriangleSelector->removeTriangleSelector(node->getTriangleSelector());
    node->getTriangleSelector()->drop();
    node->setTriangleSelector(nullptr);
    node->remove();
    
    // spawn dropped items
    // (if any)
    
    float partialAngle = degToRad(360.f / 6.f);
    for (int i = 0; i < 5; i++) {
        auto angle = partialAngle * i;
        vector3df velocity{sin(angle), 0.f, cos(angle)};
        velocity *= rand()%10+1;
        
        auto dropModel = device()->getSceneManager()->addSphereSceneNode(1.f, 4, mainScene, kItemID);
        dropModel->setPosition(nodePos);
        dropModel->getMaterial(0).DiffuseColor = SColor(255, 20, 255, 20);
        dropModel->setMaterialType(E_MATERIAL_TYPE::EMT_SOLID);
        mainScene->addChild(dropModel);
        
        setupCollisionAnimator(dropModel);
        
        LivingMetadata md;
        md.xzVelocity = vector2df{velocity.X, velocity.Z};
        entities[dropModel] = md;
    }
}



