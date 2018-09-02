//
//  MapControlEventReceiver.cpp
//  ProcMapGeneration-macOS
//
//  Created by Tom Albrecht on 27.08.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#include "MapControlEventReceiver.hpp"

MapControlEventReceiver::MapControlEventReceiver() {
    for (int x = 0; x < irr::KEY_KEY_CODES_COUNT; x++) m_keys[x] = false;
}

bool MapControlEventReceiver::OnEvent(const irr::SEvent & event) {
    if (event.EventType == irr::EET_KEY_INPUT_EVENT){
        m_keys[event.KeyInput.Key] = event.KeyInput.PressedDown;
        if (event.KeyInput.PressedDown) {
            if (m_pressedKeyHandler) m_pressedKeyHandler(event.KeyInput.Key);
        } else {
            if (m_releasedKeyHandler) m_releasedKeyHandler(event.KeyInput.Key);
        }

    } else if (event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        m_mouseInformation.clickedRight = event.MouseInput.isRightPressed();
        m_mouseInformation.clickedLeft = event.MouseInput.isLeftPressed();
        m_mouseInformation.x = event.MouseInput.X;
        m_mouseInformation.y = event.MouseInput.Y;
        if (m_mouseInfoChangedHandler) m_mouseInfoChangedHandler(m_mouseInformation);
    }
    return false;
}

bool MapControlEventReceiver::keyPressed(irr::EKEY_CODE keyCode) const {
    return m_keys[keyCode];
}

MapControlEventReceiver::MouseInformation MapControlEventReceiver::mouseInformation() const {
    return m_mouseInformation;
}


void MapControlEventReceiver::setPressedKeyHandler(std::function<void(irr::EKEY_CODE)> handler) {
    m_pressedKeyHandler = handler;
}

void MapControlEventReceiver::setReleasedKeyHandler(std::function<void(irr::EKEY_CODE)> handler) {
    m_releasedKeyHandler = handler;
}

void MapControlEventReceiver::setMouseInfoChangedHandler(std::function<void(MouseInformation)> handler) {
    m_mouseInfoChangedHandler = handler;
}
