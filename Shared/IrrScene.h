//
//  IrrScene.h
//  ProcMapGeneration-macOS
//
//  Created by Tom Albrecht on 09.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#ifndef IrrScene_h
#define IrrScene_h

#include <stdio.h>
#include "irrlicht.h"

class IrrScene {
public:
    /**
     * default ctor
     */
    IrrScene() = default;
    
    /**
     * scene ctor with device
     */
    IrrScene(irr::IrrlichtDevice *device);
    
    /**
     * Returns the current device
     */
    virtual irr::IrrlichtDevice *device() const;
    
    /**
     * Sets a new device
     */
    virtual void device(irr::IrrlichtDevice *device);
    
    /**
     * Called every frame
     */
    virtual void update(double dt) = 0;
    
    /**
     * Called every frame
     */
    virtual void render() = 0;
    
protected:
    irr::IrrlichtDevice *m_device;
};

#endif /* IrrScene_h */
