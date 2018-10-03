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
    
    void pause(bool isPaused);
    bool paused() const;
    
private:
    irr::IrrlichtDevice *m_device;
    
    // GUI elements
    irr::gui::IGUIStaticText* coordsElement;
    
    // font for ammo
    irr::gui::IGUIFont *font;
    irr::gui::IGUIFont *fontStoneSans;

    int lastFPS = 0;
    
    irr::u32 startTime;
    
    bool m_paused = false;
    irr::video::ITexture *pauseBackTex = nullptr;
    void renderPauseMenu();
};

#endif /* GUIManager_h */
