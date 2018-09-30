//
//  EnemyManager.h
//  ProcMapGeneration
//
//  Created by Tom Albrecht on 30.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#ifndef EnemyManager_h
#define EnemyManager_h

#include <stdio.h>
#include <map>
#include <irrlicht.h>
#include "IrrManager.h"

// Ids for scenes
static const irr::s32 kItemID = -1345;
static const irr::s32 kEnemyID = -1344;

// Used for player and enemies, basically everything that 'lives'
using LivingMetadata = struct {
    int health = 100;
    int ammo = 100;
    irr::core::vector2df xzVelocity{0,0};
};

// TODO: Rename to EntityManager
class EnemyManager : public IrrManager {
public:
    EnemyManager() = default;
    EnemyManager(irr::IrrlichtDevice *device);
    
    virtual void update(double dt) override;
    
    void spawnEnemy();
    
    /**
     called when an emeny died, spawns loot, gives the player points/xp,
     * shows death aninmation / effects
     */
    void enemyDied(irr::scene::ISceneNode *node, const LivingMetadata & metadata);

    std::map<irr::scene::ISceneNode*, LivingMetadata> &entities();
    
private:
    std::map<irr::scene::ISceneNode*, LivingMetadata> m_entities;

    irr::IrrlichtDevice *m_device;
};

#endif /* EnemyManager_h */
