//
//  SoundManager.h
//  ProcMapGeneration
//
//  Created by Tom Albrecht on 30.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#ifndef SoundManager_h
#define SoundManager_h

#include <stdio.h>
#include <string>
#include "irrlicht.h"

class SoundManager {
public:
    SoundManager() = default;
    
    void playBackgroundSound(const std::string &filepath);
    
    void playSoundAt(const irr::core::vector3df &origin);
};

#endif /* SoundManager_h */
