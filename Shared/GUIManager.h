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
    
    /// Renders a menu if isPaused is true
    void pause(bool isPaused);
    /// Returns the pause status
    bool paused() const;
    
    /// move up in current menu
    void menuUp();
    /// move down in current menu
    void menuDown();
    /// select currently active menu entry
    void select();
    
    /// Goes back in menu (if possible)
    /// Closes pause menu if menuDepth is below 0
    void back();
    
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
    short m_selectedPauseMenuEntry = 1;
    irr::video::ITexture *pauseBackTex = nullptr;
    void renderPauseMenu();
    int m_menuDepth = 0;
};

#endif /* GUIManager_h */
