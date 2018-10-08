//
//  WorldInfo.h
//  ProcMapGeneration-macOS
//
//  Created by Tom Albrecht on 30.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#ifndef WorldInfo_h
#define WorldInfo_h

#include <cstdio>
#include <irrlicht.h>
#include "PhysicsManager.h"
#include "SoundManager.h"
#include "InputManager.h"
#include "EnemyManager.h"
#include "GUIManager.h"
#include "TerrainGenerator.h"
#include "XEffects.h"

class WorldInfo {
public:
    // gets set via setup()
    std::shared_ptr<EnemyManager> entity;
    std::shared_ptr<PhysicsManager> physics;
    std::shared_ptr<SoundManager> sound;
    std::shared_ptr<InputManager> input;
    std::shared_ptr<GUIManager> gui;
    std::shared_ptr<TerrainGenerator> terrainGen;
    irr::scene::ISceneNode *mainScene;
    // gets set via setup()
    irr::IrrlichtDevice *device;
    // set manually
    EffectHandler* effectHandler;
    irr::scene::IAnimatedMeshSceneNode *player;
    irr::scene::ICameraSceneNode *camera;

    // chunk cache
    std::map<irr::core::vector2di, irr::scene::IMeshSceneNode*> chunks;

    // basic setup
    float chunkSizeAB = 32.f;
    float quadScale = 8.f;
    int viewDistance = 3;
    
    void setup(irr::IrrlichtDevice *_device, const irr::core::vector3df &gravity) {
        chunkSizeAB = 32.f;
        quadScale = 10.f;
        viewDistance = 3;
        
        device = _device;
        mainScene = device->getSceneManager()->addEmptySceneNode();
        terrainGen = std::make_shared<TerrainGenerator>(irr::core::dimension2du{(unsigned)chunkSizeAB, (unsigned)chunkSizeAB}, quadScale, 100, device);
        entity = std::make_shared<EnemyManager>(device);
        entity->worldInfo(this);
        physics = std::make_shared<PhysicsManager>(device, gravity);
        physics->worldInfo(this);
        sound = std::make_shared<SoundManager>();
        sound->worldInfo(this);
        input = std::make_shared<InputManager>(device);
        input->worldInfo(this);
        gui = std::make_shared<GUIManager>(device);
        gui->worldInfo(this);
    }
};


#endif /* WorldInfo_h */
