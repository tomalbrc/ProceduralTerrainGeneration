//
//  TerrainShaderCallback.hpp
//  ProcMapGeneration-macOS
//
//  Created by Tom Albrecht on 03.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#ifndef TerrainShaderCallback_hpp
#define TerrainShaderCallback_hpp

#include <stdio.h>
#include <vector>
#include "irrlicht.h"

class TerrainShaderCallback : public irr::video::IShaderConstantSetCallBack {
public:
    TerrainShaderCallback(irr::IrrlichtDevice *device);
    
	/**
	 * Chunk size for water effect
	 */
	const irr::core::vector2df & chunkSize() const;
	void chunkSize(const irr::core::vector2df & chunkSize);

	/**
	 * Scale of the quads /(a chunk)
	 */
	const float& quadScale() const;
	void quadScale(const float & quadScale);
    
    /**
     * Light source for the terrain
     */
    irr::scene::ILightSceneNode *lightSource() const;
    void lightSource(irr::scene::ILightSceneNode *lightSource);
    
    void OnSetMaterial(const irr::video::SMaterial &material) override;
    void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData) override;
    
private:
    irr::video::SMaterial m_material;
    irr::IrrlichtDevice *m_device;

	irr::core::vector2df m_chunkSize;
	float m_quadScale;
    irr::scene::ILightSceneNode *m_lightSource;
    
};

#endif /* TerrainShaderCallback_hpp */
