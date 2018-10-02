//
//  InputManager.h
//  ProcMapGeneration
//
//  Created by Tom Albrecht on 30.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#ifndef InputManager_h
#define InputManager_h

#include <stdio.h>
#include <map>
#include <memory>
#include "irrlicht.h"
#include "IrrManager.h"
#include "MapControlEventReceiver.h"

class InputManager : public IrrManager {
public:
    InputManager() = default;
    InputManager(irr::IrrlichtDevice *device);
    ~InputManager();
    
    void setupKeyHandler();
    
    virtual void update(double dt) override;
    
    MapControlEventReceiver *eventReceiver();
    
private:
    int viewDistance = 3;
    
    std::shared_ptr<WorldInfo> m_worldInfo;
    
    irr::IrrlichtDevice *m_device;
    MapControlEventReceiver *m_eventReceiver;
    
    void manageInput(MapControlEventReceiver *eventReceiver, irr::scene::ISceneNode *player, irr::scene::ICameraSceneNode *cam2, irr::f32 deltaTime);
};

#endif /* InputManager_h */
