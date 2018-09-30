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
#include <irrlicht.h>
#include <cAudio.h>
#include "IrrManager.h"

class SoundManager : public IrrManager  {
public:
    SoundManager();
    ~SoundManager();

    void playBackgroundSound(const std::string &filepath);
    void pauseBackgroundSound();
    void resumeBackgroundSound();
    bool backgroundSoundPlaying();
    
    void playSoundAt(const std::string &file, const irr::core::vector3df &origin);
    
    virtual void update(double dt) override {};
    
private:
    cAudio::IAudioManager *m_AudioManager = nullptr;
    cAudio::IAudioSource *m_background = nullptr;
    bool m_bsPlaying = false;
};

#endif /* SoundManager_h */
