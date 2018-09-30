//
//  WorldScene.h
//  ProcMapGeneration
//
//  Created by Tom Albrecht on 09.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#ifndef WorldScene_h
#define WorldScene_h

#include <stdio.h>
#include <map>
#include "IrrScene.h"

#include "TerrainGenerator.h"
#include "MapControlEventReceiver.h"

#include "PhysicsManager.h"
#include "SoundManager.h"
#include "InputManager.h"

#include "WorldInfo.h"

class EffectHandler;

class WorldScene : public IrrScene {
public:
    WorldScene(irr::IrrlichtDevice *device);
	~WorldScene() = default;
    
    void update(double dt) final;
    
    void render() final;
    
private:
    std::shared_ptr<WorldInfo> worldInfo;
    

	// shader cache material id
    std::vector<irr::s32> shaderMaterialIDS; 

    // RC
    void raycast();
    // !RC
    
    // callback for multi-threaded terrain generation
    void terrainGenerationFinished(irr::scene::IMeshSceneNode* m, irr::core::vector2di key);
    
    // adds a cloud with random rotation on position with given parent
    void addPlant(irr::core::vector3df vertexPos, irr::scene::ISceneNode *parent);
    void addCloud(irr::core::vector3df vertexPos, irr::scene::ISceneNode *parent);
    void addRock(irr::core::vector3df vertexPos, irr::scene::ISceneNode *parent);
    
};

#endif /* WorldScene_h */
