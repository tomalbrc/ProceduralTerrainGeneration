//
//  WorldScene.h
//  ProcMapGeneration-macOS
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

#include "btBulletDynamicsCommon.h"

class EffectHandler;

class WorldScene : public IrrScene {
public:
    WorldScene(irr::IrrlichtDevice *device);
	~WorldScene() = default;
    
    void update(double dt) final;
    
    void render() final;
    
private:
    // Shadow & other special effects
    EffectHandler* effect = nullptr;

	// Used for player and enemies, basically everything that 'lives'
	using LivingMetadata = struct {
		int health = 100;
        int ammo = 100;
        irr::core::vector2df xzVelocity{0,0};
	};

	// Players view distance
    int viewDistance = 8; // view distance as radius in chunks
    
	// basic nodes definetly needed
    irr::scene::ISceneNode *mainScene;
    irr::scene::ICameraSceneNode *cam2;
    irr::scene::IMeshSceneNode *player;

	std::map<irr::scene::ISceneNode*, LivingMetadata> entities;

	// GUI elements
    irr::gui::IGUIStaticText* fpsTextElement;
    irr::gui::IGUIStaticText* viewDistanceElement;
    irr::gui::IGUIStaticText* coordsElement;
    
    // font for ammo
    irr::gui::IGUIFont *font;
    
    // triangle selector collection for all the models of the terrain/world
    irr::scene::IMetaTriangleSelector *worldTriangleSelector;

    MapControlEventReceiver *eventReceiver;
    
    int lastFPS = 0;
    float chunkSizeAB = 32.f;
	float quadScale = 1.f;
    
    TerrainGenerator *terrainGen;
    
    // chunk cache
    std::map<irr::core::vector2di, irr::scene::IMeshSceneNode*> chunks;
	// shader cache material id
    std::vector<irr::s32> shaderMaterialIDS; 

    // setup collision system for the player with the world
    void setupCollisionAnimator(irr::scene::ISceneNode *target = nullptr);

    // called every frame
    void manageInput(MapControlEventReceiver *eventReceiver, irr::scene::ISceneNode *player, irr::scene::ICameraSceneNode *cam2, irr::f32 deltaTime = 1.f);
    void updateFPSCounter();

    // RC
    irr::video::SMaterial triangleMaterial;
    void raycast();
    // !RC
    
    // callback for multi-threaded terrain generation
    void terrainGenerationFinished(irr::scene::IMeshSceneNode* m, irr::core::vector2di key);
    
    // adds a cloud with random rotation on position with given parent
    void addPlant(irr::core::vector3df vertexPos, irr::scene::ISceneNode *parent);
    void addCloud(irr::core::vector3df vertexPos, irr::scene::ISceneNode *parent);
    void addRock(irr::core::vector3df vertexPos, irr::scene::ISceneNode *parent);
    
	// Spawns an enemy slighty above the player, with gravity and collision enabled
	void spawnEnemies();
    
    /**
     called when an emeny died, spawns loot, gives the player points/xp,
     * shows death aninmation / effects
     */
     void enemyDied(irr::scene::ISceneNode *node, const WorldScene::LivingMetadata & metadata);
    
    /**
     * Setup Physics
     */
    void setupPhysics();
    void addBody(btAlignedObjectArray<btCollisionShape*> &collisionShapes);
};

#endif /* WorldScene_h */
