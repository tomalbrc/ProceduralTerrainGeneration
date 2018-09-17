//
//  TerrainShaderCallback.cpp
//  ProcMapGeneration-macOS
//
//  Created by Tom Albrecht on 03.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#include "TerrainShaderCallback.h"
#include <functional>
#include <vector>
#include <queue>
#include <future>

using namespace irr;

TerrainShaderCallback::TerrainShaderCallback(irr::IrrlichtDevice *device) : irr::video::IShaderConstantSetCallBack{} {
    m_device = device;
}

const irr::core::vector2df& TerrainShaderCallback::chunkSize() const {
	return m_chunkSize;
}

void TerrainShaderCallback::chunkSize(const irr::core::vector2df & chunkSize) {
	m_chunkSize = chunkSize;
}

const float& TerrainShaderCallback::quadScale() const {
	return m_quadScale;
}
void TerrainShaderCallback::quadScale(const float & quadScale) {
	m_quadScale = quadScale;
}

void TerrainShaderCallback::OnSetMaterial(const irr::video::SMaterial &material) {
    m_material = material;
    m_material.GouraudShading = false;
}

void TerrainShaderCallback::OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData) {
    video::IVideoDriver* video = services->getVideoDriver();
    
    float time = (float)m_device->getTimer()->getTime();
    services->setVertexShaderConstant("time", &time, 1);

	services->setVertexShaderConstant("chunkSize", reinterpret_cast<f32*>(&m_chunkSize), 2);
	services->setVertexShaderConstant("quadScale", reinterpret_cast<f32*>(&m_quadScale), 1);
    
    
}
