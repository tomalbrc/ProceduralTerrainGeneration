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
#include "irrlicht.h"

class TerrainShaderCallback : public irr::video::IShaderConstantSetCallBack {
public:
    TerrainShaderCallback(irr::IrrlichtDevice *device);
    
    void OnSetMaterial(const irr::video::SMaterial &material) override;
    void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData) override;

private:
    irr::video::SMaterial m_material;
    irr::IrrlichtDevice *m_device;
};

#endif /* TerrainShaderCallback_hpp */
