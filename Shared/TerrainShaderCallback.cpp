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

void TerrainShaderCallback::OnSetMaterial(const irr::video::SMaterial &material) {
    m_material = material;
}

void TerrainShaderCallback::OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData) {
    video::IVideoDriver* video = services->getVideoDriver();
    
    // set inverted world matrix
    // if we are using highlevel shaders (the user can select this when
    // starting the program), we must set the constants by name.
    
    core::matrix4 invView = video->getTransform(video::ETS_VIEW);
    invView.makeInverse();
    services->setVertexShaderConstant("ViewMatrix", video->getTransform(video::ETS_WORLD).pointer(), 16);
    
    // set clip matrix
    
    core::matrix4 worldViewProj;
    worldViewProj = video->getTransform(video::ETS_PROJECTION);
    worldViewProj *= video->getTransform(video::ETS_VIEW);
    worldViewProj *= video->getTransform(video::ETS_WORLD);
    
    services->setVertexShaderConstant("mWorldViewProj", worldViewProj.pointer(), 16);
    
    // set camera position
    
    core::vector3df pos = m_device->getSceneManager()->
    getActiveCamera()->getAbsolutePosition();
    
    services->setVertexShaderConstant("mLightPos", reinterpret_cast<f32*>(&pos), 3);
    
    // set light color
    
    video::SColorf col(0.0f,1.0f,1.0f,0.0f);
    services->setVertexShaderConstant("mLightColor",
                                          reinterpret_cast<f32*>(&col), 4);
    
    // set transposed world matrix
    
    core::matrix4 world = video->getTransform(video::ETS_WORLD);
    world = world.getTransposed();
    

    services->setVertexShaderConstant("mTransWorld", world.pointer(), 16);
        
    // set texture, for textures you can use both an int and a float setPixelShaderConstant interfaces (You need it only for an OpenGL driver).
    
    s32 TextureLayerID = 0;
    services->setPixelShaderConstant("myTexture", &TextureLayerID, 1);
}
