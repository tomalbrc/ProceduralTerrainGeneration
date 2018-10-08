//
//  GUIManager.cpp
//  ProcMapGeneration
//
//  Created by Tom Albrecht on 30.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#include "GUIManager.h"
#include "Utils.h"
#include "WorldInfo.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace tom;


#define PLAYER_PROPS worldInfo()->entity->entities()[worldInfo()->player]

static auto kFontPath = ResourcePath("fonts/haettenschweiler.xml");
static auto kFontStoneSans = ResourcePath("fonts/stone-sans.xml");
static int kPlayerMaxAmmo = 1000.f;

GUIManager::GUIManager(irr::IrrlichtDevice *device) {
    m_device = device;
    m_paused = false;
    coordsElement = device->getGUIEnvironment()->addStaticText(L"", irr::core::rect<irr::s32>(5, 150, 140+115, 170), false, false, device->getGUIEnvironment()->getRootGUIElement(), 1003, true);
    
    font = device->getGUIEnvironment()->getFont(kFontPath);
    fontStoneSans = device->getGUIEnvironment()->getFont(kFontStoneSans);

    startTime = device->getTimer()->getTime();
    
    auto img = device->getVideoDriver()->createImageFromFile(ResourcePath("models/pauseBackground.png"));
    pauseBackTex = device->getVideoDriver()->addTexture("pauseBackground", img);
}

void GUIManager::update(double dt) {
    core::stringw str2 = L"Coords: ";
    str2 += worldInfo()->player->getPosition().X;
    str2 += L", ";
    str2 += worldInfo()->player->getPosition().Y;
    str2 += L", ";
    str2 += worldInfo()->player->getPosition().Z;
    coordsElement->setText(str2.c_str());
}

void GUIManager::render() {
    auto str = L"AMMO: " + std::to_wstring(PLAYER_PROPS.ammo) + L"/" + std::to_wstring(kPlayerMaxAmmo);
    font->draw(str.c_str(), core::rect<s32>{static_cast<int>(m_device->getVideoDriver()->getScreenSize().Width-205),5,0,0}, video::SColor{255,0,0,0});
    
    auto dt = worldInfo()->device->getTimer()->getTime() -startTime;
    if (dt < 4000.f) {
        str = L"PREPARE";
        font->draw(str.c_str(), core::rect<s32>{static_cast<int>(m_device->getVideoDriver()->getScreenSize().Width/2.f)-250, static_cast<int>(m_device->getVideoDriver()->getScreenSize().Height/2.f)-50,0,0}, video::SColor{255,0,0,0});
    }
    
    str = L"ENTITIES: " + std::to_wstring(worldInfo()->entity->entities().size());
    font->draw(str.c_str(), core::rect<s32>{10, 5 + 45,0,0}, video::SColor{180,0,0,0});

    str = L"FPS: ";
    str += std::to_wstring(m_device->getVideoDriver()->getFPS());
    font->draw(str.c_str(), core::rect<s32>{10, 5,0,0}, video::SColor{180,170,170,0});
    
    str = L"View distance: ";
    str += std::to_wstring(worldInfo()->viewDistance);
    font->draw(str.c_str(), core::rect<s32>{10, 5+45+45,0,0}, video::SColor{180,20,40,20});
    
    
    // Query entities and render health bar texture after transform position to screen coords
    for (auto e : worldInfo()->entity->entities()) {
        if (e.second.health <= 0) {
            continue;
        }
        
        auto dist = worldInfo()->player->getPosition().getDistanceFrom(e.first->getPosition());
        if (dist > 150.f || e.first->getID() != kEnemyID) {
            continue;
        }
        
        auto screenPos = m_device->getSceneManager()->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(e.first->getPosition());
        
        str = L"hp: ";
        str += std::to_wstring(e.second.health);
        fontStoneSans->draw(str.c_str(), core::rect<s32>{screenPos.X-30, screenPos.Y-90,0,0}, video::SColor{255,200,20,20});
    }
    
    if (m_paused) {
        renderPauseMenu();
    }
}

void GUIManager::pause(bool isPaused) {
    m_paused = isPaused;
}

bool GUIManager::paused() const {
    return m_paused;
}






/// PRIVATE IMPL.

void GUIManager::renderPauseMenu() {
    // draw pause menu background
    worldInfo()->device->getVideoDriver()->draw2DImage(pauseBackTex, vector2di{10,10});
    
    switch (m_menuDepth) {
        case 0: {
            auto selectedColor = video::SColor{255,200,250,200};
            auto normalColor = video::SColor{255,255,255,255};
            
            auto contCol = m_selectedPauseMenuEntry == 1 ? selectedColor : normalColor;
            auto optCol = m_selectedPauseMenuEntry == 2 ? selectedColor : normalColor;
            auto exCol = m_selectedPauseMenuEntry == 3 ? selectedColor : normalColor;
            
            std::wstring str = L"Continue";
            font->draw(str.c_str(), core::rect<s32>{40, 100,0,0}, contCol);
            
            str = L"Options";
            font->draw(str.c_str(), core::rect<s32>{40, 200,0,0}, optCol);
            
            str = L"Exit";
            font->draw(str.c_str(), core::rect<s32>{40, 300,0,0}, exCol);
            break;
        }
        case 1: {
            auto selectedColor = video::SColor{255,250,250,250};
            auto normalColor = video::SColor{255,190,255,190};
            
            auto contCol = m_selectedPauseMenuEntry == 1 ? selectedColor : normalColor;
            auto optCol = m_selectedPauseMenuEntry == 2 ? selectedColor : normalColor;
            auto exCol = m_selectedPauseMenuEntry == 3 ? selectedColor : normalColor;
            
            std::wstring str = L"Continue2";
            font->draw(str.c_str(), core::rect<s32>{40, 100,0,0}, contCol);
            
            str = L"Options";
            font->draw(str.c_str(), core::rect<s32>{40, 200,0,0}, optCol);
            
            str = L"Exit";
            font->draw(str.c_str(), core::rect<s32>{40, 300,0,0}, exCol);
            break;
        }
        case 2: {
            auto selectedColor = video::SColor{255,250,250,250};
            auto normalColor = video::SColor{255,190,255,190};
            
            auto contCol = m_selectedPauseMenuEntry == 1 ? selectedColor : normalColor;
            auto optCol = m_selectedPauseMenuEntry == 2 ? selectedColor : normalColor;
            auto exCol = m_selectedPauseMenuEntry == 3 ? selectedColor : normalColor;
            
            std::wstring str = L"Continue3";
            font->draw(str.c_str(), core::rect<s32>{40, 100,0,0}, contCol);
            
            str = L"Options";
            font->draw(str.c_str(), core::rect<s32>{40, 200,0,0}, optCol);
            
            str = L"Exit";
            font->draw(str.c_str(), core::rect<s32>{40, 300,0,0}, exCol);
            break;
        }
        default:
            break;
    }
}

/// move up in current menu
void GUIManager::menuUp() {
    m_selectedPauseMenuEntry--;
    if (m_selectedPauseMenuEntry < 0) {
        m_selectedPauseMenuEntry = 0;
    }
}
/// move down in current menu
void GUIManager::menuDown() {
    m_selectedPauseMenuEntry++;
    if (m_selectedPauseMenuEntry > 3) {
        m_selectedPauseMenuEntry = 3;
    }
}
/// select currently active menu entry
void GUIManager::select() {
    if (m_menuDepth == 0) {
        // main pause menu
        if (m_selectedPauseMenuEntry == 3) {
            worldInfo()->device->closeDevice();
        } else if (m_selectedPauseMenuEntry == 1) {
            pause(false);
        } else if (m_selectedPauseMenuEntry == 2) {
            m_selectedPauseMenuEntry = 1;
            m_menuDepth++;
        }
    } else if (m_menuDepth == 1) { // >pause -> options< menu
        
    }
}

/// Goes back in menu (if possible)
/// Does not close pause menu, pause(true) to do that
void GUIManager::back() {
    m_selectedPauseMenuEntry = 1;

    if (m_menuDepth > 0) {
        m_menuDepth--;
    } else {
        pause(false);
    }
}
