//
//  TerrainGenerator.cpp
//  ProcMapGeneration-macOS
//
//  Created by Tom Albrecht on 27.08.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#include "TerrainGenerator.h"
#include "ColorSelector.hpp"
#include <random>
#include <future>
#include <queue>
#include <mutex>
#include <time.h>
#include "Utils.h"


/**
 * C-style functions to generate an image for the heightmap and an image for chunk-texture
 */
irr::video::IImage *IImageColoredFromNoiseMap(const NoiseMap & cm, irr::video::IVideoDriver *video) {
    auto image = video->createImage(irr::video::ECF_A8R8G8B8, irr::core::dimension2d<irr::u32>((irr::u32)cm.size(),(irr::u32)cm.front().size()));
    int x = 0, y = 0;
    for (auto row : cm) {
        for (auto val : row) {
            auto color = ColorSelector::ColorFromValue(val);
            auto scol = irr::video::SColor(255, color.red,color.green,color.blue);
            image->setPixel(x, y, scol);
            x++;
        }
        x = 0;
        y++;
    }
    return image;
}
/**
 * C-style functions to generate an image for the heightmap and an image for chunk-texture
 */
irr::video::IImage *IImageFromNoiseMap(const NoiseMap & nm, irr::video::IVideoDriver *video) {
    auto image = video->createImage(irr::video::ECF_A8R8G8B8, irr::core::dimension2d<irr::u32>((irr::u32)nm.size(),(irr::u32)nm.front().size()));
    int x = 0, y = 0;
    for (auto row : nm) {
        for (auto col : row) {
            auto scol = irr::video::SColor(255, col*255,col*255,col*255);
            image->setPixel(x, y, scol);
            x++;
        }
        x = 0;
        y++;
    }
    return image;
}



///
/// class TerrainGenerator
///
TerrainGenerator::TerrainGenerator(irr::core::dimension2du chunkSize, float quadScale, float terrainHeight, irr::IrrlichtDevice *device) {
    m_chunkSize = chunkSize;
	m_quadScale = quadScale;
	srand((unsigned)time(NULL));
	m_seed = rand();//std::default_random_engine::default_seed;
    m_device = device;
    m_terrainHeight = terrainHeight;
}

irr::scene::IMeshSceneNode* TerrainGenerator::getMeshAt(irr::core::vector2di chunkLocation, const std::function<void(irr::scene::IMeshSceneNode*, irr::core::vector2di)> &completion) {
    
    auto noiseScale = 300.f*1.5f;
    auto offset = irr::core::vector2di{chunkLocation.X*((int)m_chunkSize.Width-1), chunkLocation.Y*((int)m_chunkSize.Height-1)};
    auto nm = NoiseMapGenerator::Generate(m_chunkSize, chunkLocation, m_seed, noiseScale);
	//auto nm2 = NoiseMapGenerator::Generate(m_chunkSize*4.f, chunkLocation, m_seed, noiseScale*4.f);

    //irr::video::IImage *image = IImageColoredFromNoiseMap(nm2, m_device->getVideoDriver());
    irr::video::IImage *imageHeightmap = IImageFromNoiseMap(nm, m_device->getVideoDriver());
    
//	_image = image;
	_heightmap = imageHeightmap;
    
    tom::threading::addMainCallback([m_quadScale = m_quadScale, key = std::move(chunkLocation), offset = std::move(offset), m_device = m_device, imageHeightmap = imageHeightmap, m_terrainHeight = m_terrainHeight, m_chunkSize = m_chunkSize, completion = std::move(completion)]() mutable {
        auto quadScale = irr::core::dimension2df{1.f,1.f}*m_quadScale;
        auto geomentryCreator = m_device->getSceneManager()->getGeometryCreator();
        auto terrain = geomentryCreator->createTerrainMesh(imageHeightmap, imageHeightmap, quadScale, m_terrainHeight*4.f, m_device->getVideoDriver(), m_chunkSize*2.0 , false);
        terrain->setMaterialFlag(irr::video::EMF_LIGHTING, true); // global lightning for heightmap display
        terrain->setMaterialFlag(irr::video::EMF_BILINEAR_FILTER, false);
        terrain->getMeshBuffer(0)->getMaterial().GouraudShading = false;
        
        
        
        auto msn = m_device->getSceneManager()->addMeshSceneNode(terrain);
        msn->setPosition(irr::core::vector3df{(float)offset.X*quadScale.Width, 0,(float)offset.Y*quadScale.Height});
        
        completion(msn, key);
    });
    
    return nullptr;
}
