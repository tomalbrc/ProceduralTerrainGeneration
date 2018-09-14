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
#include "IMetaTriangleSelector.h"

using namespace irr;
using namespace irr::scene;
using namespace irr::video;
using namespace irr::core;

static const char *kTreeTexturePath = "models/TreeTexture.png";
static const char *kBushTexturePath = "models/BushTexture.png";
static const char *kCloudTexturePath = "models/white.png";
static const char *kRockTexturePath = "models/rock.png";

static const irr::s32 kEnemyID = -1344;

static const irr::core::vector3df kPlayerSpawnPosition{5000.f, 400.f, 5000.f};

static const int kPlayerMaxAmmo = 1000.f;

WorldScene::WorldScene(irr::IrrlichtDevice *device) : IrrScene(device) {
    eventReceiver = new MapControlEventReceiver();
    device->setEventReceiver(eventReceiver);
    
    quadScale = 3.0f;
    chunkSizeAB = 32.f;
    
    IVideoDriver* video = device->getVideoDriver();
    ISceneManager* smgr = device->getSceneManager();
    
    terrainGen = new TerrainGenerator(irr::core::dimension2du{(unsigned)chunkSizeAB, (unsigned)chunkSizeAB}, quadScale, 100.0, device);
    
    worldTriangleSelector = this->device()->getSceneManager()->createMetaTriangleSelector();
    mainScene = smgr->addEmptySceneNode();
    
    player = smgr->addCubeSceneNode(1.f);
    player->setScale(core::vector3df{1.5f,3.0f,1.5f}*2.f);
    player->setPosition(kPlayerSpawnPosition);
    LivingMetadata md;
    md.health = 100;
    md.ammo = kPlayerMaxAmmo;
    entities[player] = md;
    
    cam2 = smgr->addCameraSceneNode();
    cam2->setPosition(kPlayerSpawnPosition);
    mainScene->addChild(cam2);
    
    auto light = smgr->addLightSceneNode();
    light->setRadius(100.f);
    light->setLightType(video::ELT_DIRECTIONAL);
    light->setRotation(irr::core::vector3df(90, 0, 0));
    player->addChild(light);
    
    shaderMaterialIDS = tom::setupShader(device, vector2df(chunkSizeAB), quadScale);
    
    // give player a collision animator with worldTriangleSelector as tri selector
    setupCollisionAnimator();
    
    eventReceiver->setPressedKeyHandler([this](irr::EKEY_CODE kc){
        if (kc == irr::KEY_KEY_N) {
            viewDistance++;
        } else if (kc == irr::KEY_KEY_M) {
            viewDistance--;
        } else if (kc == irr::KEY_KEY_R) {
            player->setPosition(kPlayerSpawnPosition);
			cam2->setPosition(kPlayerSpawnPosition);
        }  else if (kc == irr::KEY_SPACE) {
            if (player->getAnimators().size()) {
                auto c = dynamic_cast<ISceneNodeAnimatorCollisionResponse*>((*player->getAnimators().begin()));
                if (c && !c->isFalling()) {
                    c->jump(5*50);
                }
            }
        } else if (kc == irr::KEY_KEY_F) {
            player->removeAnimators();
        } else if (kc == irr::KEY_KEY_P) {
			spawnEnemies();
		}
    });
    
    fpsTextElement = device->getGUIEnvironment()->addStaticText(L"", irr::core::rect<irr::s32>(5, 5, 140, 30), false, false, device->getGUIEnvironment()->getRootGUIElement(), 1001, true);
    viewDistanceElement = device->getGUIEnvironment()->addStaticText(L"", irr::core::rect<irr::s32>(5, 5+25, 140, 45), false, false, device->getGUIEnvironment()->getRootGUIElement(), 1002, true);
    coordsElement = device->getGUIEnvironment()->addStaticText(L"", irr::core::rect<irr::s32>(5, 5+25+25, 140+115, 70), false, false, device->getGUIEnvironment()->getRootGUIElement(), 1003, true);
    
    font = device->getGUIEnvironment()->getFont("fonts/betafont.xml");
    
    triangleMaterial.Lighting = false;
	triangleMaterial.Wireframe = false;
}

void WorldScene::update(double dt) {
    manageInput(eventReceiver, player, cam2);

    updateFPSCounter();
    
    auto playerChunkLoc = irr::core::vector2di(player->getPosition().X / ((chunkSizeAB-1)*quadScale), player->getPosition().Z / ((chunkSizeAB - 1)*quadScale));
    bool hasKey = false;
    
    for (auto ting : chunks) {
        if (ting.second) ting.second->setVisible(false);
        if (playerChunkLoc.equals(ting.first)) hasKey = true;
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
}

void WorldScene::render() {
    m_device->getVideoDriver()->beginScene(true, true, video::SColor(255,173,241,255));
    m_device->getSceneManager()->drawAll();
    device()->getGUIEnvironment()->drawAll();
    if (eventReceiver->mouseInformation().clickedLeft) raycast();
    
    auto str = L"AMMO: " + std::to_wstring(entities[player].ammo) + L" / " + std::to_wstring(kPlayerMaxAmmo);
    font->draw(str.c_str(), core::rect<s32>{static_cast<int>(device()->getVideoDriver()->getScreenSize().Width-500),0,0,0}, video::SColor{255,0,0,0});
    m_device->getVideoDriver()->endScene();
    
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
    
    float value = eventReceiver->keyPressed(irr::KEY_KEY_E) ? 10.f : 0.5f;
    if (eventReceiver->keyPressed(irr::KEY_RIGHT) || eventReceiver->keyPressed(irr::KEY_KEY_D)) player->setPosition(player->getPosition() + irr::core::vector3df(value*cos(irr::core::degToRad(angle - 90.f)), 0.f, value*sin(irr::core::degToRad(angle - 90.f))));
    if (eventReceiver->keyPressed(irr::KEY_LEFT) || eventReceiver->keyPressed(irr::KEY_KEY_A)) player->setPosition(player->getPosition() + irr::core::vector3df(value*cos(irr::core::degToRad(angle + 90.f)), 0.f, value*sin(irr::core::degToRad(angle  +90.f))));
    if (eventReceiver->keyPressed(irr::KEY_UP) || eventReceiver->keyPressed(irr::KEY_KEY_W)) player->setPosition(player->getPosition() + irr::core::vector3df(value*xVal, 0.f, value*yVal));
    if (eventReceiver->keyPressed(irr::KEY_DOWN) || eventReceiver->keyPressed(irr::KEY_KEY_S)) player->setPosition(player->getPosition() + irr::core::vector3df(value*cos(irr::core::degToRad(angle + 180.f)), 0.f, value*sin(irr::core::degToRad(angle + 180.f))));
    // gets managed in the eventReceiver::keyPressed lambda with player->getAnimators.first.jump()
    //if (eventReceiver->keyPressed(irr::KEY_SPACE)) player->setPosition(player->getPosition() + irr::core::vector3df(0,value,0));
    if (eventReceiver->keyPressed(irr::KEY_KEY_X)) player->setPosition(player->getPosition() + irr::core::vector3df(0,-value,0));
    
    cam2->setTarget(player->getPosition());
    
    player->setRotation(irr::core::vector3df(0,-angle,0));
    
    auto newPosition = player->getPosition() + irr::core::vector3df(-25 * xVal, 8.f, -25 * yVal);
    float lowpassfilterFactor = .1f;
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
}

void WorldScene::setupCollisionAnimator(irr::scene::ISceneNode *target) {
	if (target == nullptr) {
		target = player;
	}
	target->removeAnimators();
	///----------
	scene::ISceneNodeAnimator* anim = device()->getSceneManager()->createCollisionResponseAnimator(worldTriangleSelector, player, core::vector3df(1.5f,3.0f,1.5f),
		core::vector3df(0, -10.f, 0)*50, core::vector3df(0), .001f);
	target->addAnimator(anim);
	anim->drop();
	///----------
}

void WorldScene::raycast() {
    if (entities[player].ammo <= 0) return;
    entities[player].ammo--;
    
    
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
        device()->getVideoDriver()->draw3DLine(player->getPosition(), collisionPoint);

		if (selectedScene->getID() == kEnemyID) {
			auto oldPos = selectedScene->getPosition();
			oldPos += loc*5.f;
			selectedScene->setPosition(oldPos);
            
            entities[selectedScene].health--;
            if (entities[selectedScene].health <= 0) {
                enemyDied(selectedScene, entities[selectedScene]);
            }
		}
    }
}


void WorldScene::terrainGenerationFinished(irr::scene::IMeshSceneNode* m, irr::core::vector2di key) {
    mainScene->addChild(m);
    m->setMaterialType((video::E_MATERIAL_TYPE)shaderMaterialIDS.front());
    chunks[key]->remove();
    chunks[key] = m;
    
    scene::ITriangleSelector* selector = device()->getSceneManager()->createOctreeTriangleSelector(m->getMesh(), m);
    m->setTriangleSelector(selector);
    worldTriangleSelector->addTriangleSelector(selector);
    selector->drop();

    for (auto i = 0; i < m->getMesh()->getMeshBuffer(0)->getVertexCount(); i++) {
        auto vertexPos = m->getMesh()->getMeshBuffer(0)->getPosition(i);
        
        // BigTreeWithLeaves.obj
        if (rand()%500 == 1 && vertexPos.Y > 10.f) {
            if (vertexPos.Y < 80.f) {
                addPlant(vertexPos, m);
            } else if (rand()%200 == 5) {
                addRock(vertexPos, m);
            }
        }
        
        if (rand()%10000 == 123) addCloud(vertexPos, m); // Random clouds
    } // Vertex for loop
}


void WorldScene::addPlant(core::vector3df vertexPos, irr::scene::ISceneNode *parent) {
    auto mesh = device()->getSceneManager()->getMesh("models/BigTreeWithLeaves.obj");
    auto mesh2 = device()->getSceneManager()->getMesh("models/SmallTreeWithLeave.obj");
    auto meshBush = device()->getSceneManager()->getMesh("models/BigBush.obj");
    
    auto ran = rand()%3;
    auto meshScene = device()->getSceneManager()->addMeshSceneNode(ran == 0 ? mesh : ran==1 ? mesh2 : meshBush);
    meshScene->setPosition(vertexPos);
    meshScene->setScale(irr::core::vector3df{ran==2?4.f:6.f});
    
    meshScene->setMaterialType((video::E_MATERIAL_TYPE)shaderMaterialIDS.at(1));
    meshScene->setRotation(irr::core::vector3df{0.f,(float)(rand()%360),0.f});
    
    auto img = device()->getVideoDriver()->createImageFromFile(ran > 1 ? kBushTexturePath : kTreeTexturePath);
    meshScene->setMaterialTexture(0, device()->getVideoDriver()->addTexture(ran > 1 ? kBushTexturePath : kTreeTexturePath, img));
    
    parent->addChild(meshScene);
    
    if (ran != 2) {
        scene::ITriangleSelector* selector = device()->getSceneManager()->createOctreeTriangleSelector(meshScene->getMesh(), meshScene);
        meshScene->setTriangleSelector(selector);
        worldTriangleSelector->addTriangleSelector(selector);
    }
}

void WorldScene::addCloud(core::vector3df vertexPos, irr::scene::ISceneNode *parent) {
    auto meshCloud1 = device()->getSceneManager()->getMesh("models/Cloud1.obj");
    auto meshCloud2 = device()->getSceneManager()->getMesh("models/Cloud2.obj");
    auto meshCloud3 = device()->getSceneManager()->getMesh("models/Cloud3.obj");
    
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
    
    scene::ITriangleSelector* selector = device()->getSceneManager()->createOctreeTriangleSelector(meshScene->getMesh(), meshScene);
    meshScene->setTriangleSelector(selector);
    worldTriangleSelector->addTriangleSelector(selector);
}


void WorldScene::spawnEnemies() {
	auto spawnPos = player->getPosition();
	spawnPos.Y += 10;

	auto enemy = device()->getSceneManager()->addSphereSceneNode(5.f, 16, mainScene, kEnemyID);
	enemy->setPosition(spawnPos);
	mainScene->addChild(enemy);

	scene::ITriangleSelector* selector = device()->getSceneManager()->createOctreeTriangleSelector(enemy->getMesh(), enemy);
	enemy->setTriangleSelector(selector);
	worldTriangleSelector->addTriangleSelector(selector);

	setupCollisionAnimator(enemy);

    LivingMetadata md;
    md.health = 50;
    
    entities[enemy] = md;
}

void WorldScene::enemyDied(irr::scene::ISceneNode *node, const WorldScene::LivingMetadata & metadata) {
    
    // apply velocitry manual if enemy was just hit
    
    
    
    worldTriangleSelector->removeTriangleSelector(node->getTriangleSelector());
    node->getTriangleSelector()->drop();
    node->setTriangleSelector(nullptr);
    node->remove();
}
