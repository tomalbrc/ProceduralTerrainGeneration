//
//  TerrainGenerator.hpp
//  ProcMapGeneration-macOS
//
//  Created by Tom Albrecht on 27.08.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#ifndef TerrainGenerator_hpp
#define TerrainGenerator_hpp

#include <stdio.h>
#include <irrlicht.h>
#include <functional>
#include <vector>
#include "NoiseMapGenerator.h"

class TerrainGenerator {
public:
    /**
     * Terrain generator.
     * Outputs IMesh-es for offsets
     */
    TerrainGenerator(irr::core::dimension2du chunkSize, float quadScale, float terrainHeight, irr::IrrlichtDevice *device);
    
    /**
     * Generates an irrlicht IMesh
     *
     * @param chunkLocation The offset of the chunk
     */
    irr::scene::IMeshSceneNode *getMeshAt(irr::core::vector2di chunkLocation);
    irr::scene::IMeshSceneNode* getMeshAt(irr::core::vector2di chunkLocation, const std::function<void(irr::scene::IMeshSceneNode*,irr::core::vector2di)> & completion);

	irr::video::IImage *_image;
	irr::video::IImage *_heightmap;

private:
    unsigned m_seed = 1;
    irr::IrrlichtDevice* m_device;
    irr::core::dimension2du m_chunkSize;
	float m_quadScale = 1.f;
    float m_terrainHeight = 100.f;
};


#endif /* TerrainGenerator_hpp */
