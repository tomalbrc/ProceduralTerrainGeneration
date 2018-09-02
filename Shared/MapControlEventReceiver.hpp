//
//  MapControlEventReceiver.hpp
//  ProcMapGeneration-macOS
//
//  Created by Mac ERPA on 27.08.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#ifndef MapControlEventReceiver_hpp
#define MapControlEventReceiver_hpp

#include <stdio.h>
#include <functional>
#include "irrlicht.h"

class MapControlEventReceiver : public irr::IEventReceiver {
public:
    // Contains information about mouse states
    using MouseInformation = struct {
        float x;
        float y;
        bool clickedRight, clickedLeft;
    };
    
    /**
     * Ctor
     */
    MapControlEventReceiver();
    
    /**
     * Event receiving
     */
    bool OnEvent(const irr::SEvent & event);
    
    /**
     * Returns true if the ke is pressed
     */
    bool keyPressed(irr::EKEY_CODE) const;
    
    /**
     * Information about mouse state
     */
    MouseInformation mouseInformation() const;
    
    /**
     * The handler gets called when a keyboard key has been pressed
     */
    void setPressedKeyHandler(std::function<void(irr::EKEY_CODE)> handler);
    
    /**
     * The handler gets called when a keyboard key has been released
     */
    void setReleasedKeyHandler(std::function<void(irr::EKEY_CODE)> handler);
    
    /**
     * The handler gets called when the mouse position has changed or a mouse-button has been clicked
     */
    void setMouseInfoChangedHandler(std::function<void(MouseInformation)> handler);

private:
    bool m_keys[irr::KEY_KEY_CODES_COUNT];
    MouseInformation m_mouseInformation;
    
    // TODO: Add callbacks when a key has been pressed? (pulling vs event-based)
    std::function<void(irr::EKEY_CODE)> m_pressedKeyHandler;
    std::function<void(irr::EKEY_CODE)> m_releasedKeyHandler;
    std::function<void(MouseInformation)> m_mouseInfoChangedHandler;
};

#endif /* MapControlEventReceiver_hpp */
