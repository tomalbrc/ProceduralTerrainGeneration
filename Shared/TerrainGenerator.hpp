//
//  TerrainGenerator.hpp
//  ProcMapGeneration-macOS
//
//  Created by Mac ERPA on 27.08.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#ifndef TerrainGenerator_hpp
#define TerrainGenerator_hpp

#include <stdio.h>
#include <irrlicht.h>
#include "NoiseMapGenerator.h"

class TerrainGenerator {
public:
    /**
     * Terrain generator.
     * Outputs IMesh-es for offsets
     */
    TerrainGenerator(irr::core::dimension2du chunkSize, float terrainHeight, irr::IrrlichtDevice *device);
    
    /**
     * Generates an irrlicht IMesh
     *
     * @param chunkLocation The offset of the chunk
     */
    irr::scene::IMeshSceneNode *getMeshAt(irr::core::vector2di chunkLocation);
    
	irr::video::IImage *_image;
	irr::video::IImage *_heightmap;

private:
    unsigned m_seed;
    irr::IrrlichtDevice* m_device;
    irr::core::dimension2du m_chunkSize;
    float m_terrainHeight;
};


#endif /* TerrainGenerator_hpp */
