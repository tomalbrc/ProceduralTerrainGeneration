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

class WorldScene : public IrrScene {
public:
    WorldScene(irr::IrrlichtDevice *device);
    
    void update(double dt) final;
    
    void render() final;
    
private:
    int viewDistance = 8; // view distance as radius in chunks
    
    irr::scene::ISceneNode *mainScene;
    irr::scene::ICameraSceneNode *cam2;
    irr::scene::IMeshSceneNode *player;
    
    irr::gui::IGUIStaticText* fpsTextElement;
    irr::gui::IGUIStaticText* viewDistanceElement;
    irr::gui::IGUIStaticText* coordsElement;
    
    MapControlEventReceiver *eventReceiver;
    
    int lastFPS;
    
    float chunkSizeAB = 32.f;
	float quadScale = 6.f;
    
    irr::core::vector2di lastCollisionLoc;
    
    TerrainGenerator *terrainGen;
    
    // chunk cache
    std::map<irr::core::vector2di, irr::scene::IMeshSceneNode*> chunks;
	// shader cache material id
    std::vector<irr::s32> shaderMaterialIDS; 
    
    
    void manageInput(MapControlEventReceiver *eventReceiver, irr::scene::ISceneNode *player, irr::scene::ICameraSceneNode *cam2);
    void updateFPSCounter();
	void updateCollisionAnimator(const irr::core::vector2di & playerChunkLoc);

    // callback for multi-threaded terrain generation
    void terrainGenerationFinished(irr::scene::IMeshSceneNode* m, irr::core::vector2di key);
    
    // adds a cloud with random rotation on position with given parent
    void addCloud(irr::core::vector3df vertexPos, irr::scene::IMeshSceneNode *parent);
    void addRock(irr::core::vector3df vertexPos, irr::scene::IMeshSceneNode *parent);
    
};

#endif /* WorldScene_h */
