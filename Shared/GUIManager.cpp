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

static auto kFontPath = ResourcePath("fonts/betafont.xml");
static int kPlayerMaxAmmo = 1000.f;

GUIManager::GUIManager(irr::IrrlichtDevice *device) {
    m_device = device;
    
    fpsTextElement = device->getGUIEnvironment()->addStaticText(L"", irr::core::rect<irr::s32>(5, 5, 140, 30), false, false, device->getGUIEnvironment()->getRootGUIElement(), 1001, true);
    viewDistanceElement = device->getGUIEnvironment()->addStaticText(L"", irr::core::rect<irr::s32>(5, 5+25, 140, 45), false, false, device->getGUIEnvironment()->getRootGUIElement(), 1002, true);
    coordsElement = device->getGUIEnvironment()->addStaticText(L"", irr::core::rect<irr::s32>(5, 5+25+25, 140+115, 70), false, false, device->getGUIEnvironment()->getRootGUIElement(), 1003, true);
    
    font = device->getGUIEnvironment()->getFont(kFontPath);
    
}

void GUIManager::update(double dt) {
    if (lastFPS != m_device->getVideoDriver()->getFPS()) {
        core::stringw str = L"FPS: ";
        str += m_device->getVideoDriver()->getFPS();
        fpsTextElement->setText(str.c_str());
        lastFPS = m_device->getVideoDriver()->getFPS();
    }
    core::stringw str = L"View distance: ";
    str += worldInfo()->viewDistance;
    viewDistanceElement->setText(str.c_str());
    
    core::stringw str2 = L"Coords: ";
    str2 += worldInfo()->player->getPosition().X;
    str2 += L", ";
    str2 += worldInfo()->player->getPosition().Y;
    str2 += L", ";
    str2 += worldInfo()->player->getPosition().Z;
    coordsElement->setText(str2.c_str());
}

void GUIManager::render() {
    auto str = L"AMMO: " + std::to_wstring(PLAYER_PROPS.ammo) + L" / " + std::to_wstring(kPlayerMaxAmmo);
    font->draw(str.c_str(), core::rect<s32>{static_cast<int>(m_device->getVideoDriver()->getScreenSize().Width-500),0,0,0}, video::SColor{255,0,0,0});
}
