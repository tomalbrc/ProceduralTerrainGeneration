//
//  EnemyManager.cpp
//  ProcMapGeneration
//
//  Created by Tom Albrecht on 30.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#include "EnemyManager.h"
#include "WorldInfo.h"
#include "Utils.h"

using namespace irr;
using namespace video;
using namespace scene;
using namespace core;

EnemyManager::EnemyManager(irr::IrrlichtDevice *device) {
    m_device = device;
    m_entities.clear();
}

void EnemyManager::update(double dt) {
    
}

void EnemyManager::spawnEnemy() {
    auto spawnPos = worldInfo()->player->getPosition();
    spawnPos.Y += 100;
    
    auto enemy = m_device->getSceneManager()->addSphereSceneNode(5.f, 8, worldInfo()->mainScene, kEnemyID);
    enemy->setPosition(spawnPos);
    worldInfo()->mainScene->addChild(enemy);
    
    enemy->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    worldInfo()->effectHandler->addShadowToNode(enemy, EFT_16PCF, ESM_BOTH);
    
    worldInfo()->physics->addEntity(enemy->getMesh(), enemy);
    
    LivingMetadata md;
    md.health = 50;
    m_entities[enemy] = md;
}


void EnemyManager::enemyDied(irr::scene::ISceneNode *node, const LivingMetadata & metadata) {
    worldInfo()->effectHandler->removeShadowFromNode(node);
    
    worldInfo()->sound->playSoundAt(ResourcePath("sounds/death.ogg").c_str(), node->getPosition() - worldInfo()->camera->getPosition());
    
    auto nodePos = node->getPosition();
    
    auto iter = m_entities.find(node) ;
    if (iter != m_entities.end())
        m_entities.erase(iter);
    
    // TODO: remove from physics sim
    node->remove();
    
    // spawn dropped items
    // (if any)
    
    float partialAngle = degToRad(360.f / 6.f);
    for (int i = 0; i < 5; i++) {
        auto angle = partialAngle * i;
        vector3df velocity{sin(angle), 0.f, cos(angle)};
        velocity *= rand()%10+1;
        
        auto dropModel = m_device->getSceneManager()->addSphereSceneNode(1.f, 4, worldInfo()->mainScene, kItemID);
        dropModel->setPosition(nodePos);
        dropModel->getMaterial(0).DiffuseColor = SColor(255, 20, 255, 20);
        dropModel->setMaterialType(E_MATERIAL_TYPE::EMT_SOLID);
        worldInfo()->mainScene->addChild(dropModel);
        
        worldInfo()->physics->addEntity(dropModel->getMesh(), dropModel);
        
        LivingMetadata md;
        md.xzVelocity = vector2df{velocity.X, velocity.Z};
        m_entities[dropModel] = md;
    }
}

std::map<irr::scene::ISceneNode*, LivingMetadata> & EnemyManager::entities() {
    
    return m_entities;
}
