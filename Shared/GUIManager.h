//
//  GUIManager.h
//  ProcMapGeneration
//
//  Created by Tom Albrecht on 30.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#ifndef GUIManager_h
#define GUIManager_h

#include <stdio.h>
#include <irrlicht.h>
#include "IrrManager.h"

class GUIManager : public IrrManager {
public:
    GUIManager() = default;
    GUIManager(irr::IrrlichtDevice *device);
    
    virtual void update(double dt) override;
    void render();
    
private:
    irr::IrrlichtDevice *m_device;
    
    // GUI elements
    irr::gui::IGUIStaticText* fpsTextElement;
    irr::gui::IGUIStaticText* viewDistanceElement;
    irr::gui::IGUIStaticText* coordsElement;
    
    // font for ammo
    irr::gui::IGUIFont *font;
    
    int lastFPS = 0;
};

#endif /* GUIManager_h */
