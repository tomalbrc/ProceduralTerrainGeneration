//
//  SoundManager.cpp
//  ProcMapGeneration
//
//  Created by Tom Albrecht on 30.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#include "SoundManager.h"
#include <cassert>

using namespace cAudio;

SoundManager::SoundManager() {
    m_AudioManager = createAudioManager();
    m_background = nullptr;
    assert(m_AudioManager);
}

SoundManager::~SoundManager() {
    //m_AudioManager->shutDown();
    //destroyAudioManager(m_AudioManager);
}

void SoundManager::playBackgroundSound(const std::string &filepath) {
    if (m_background)
        CAUDIO_DELETE m_background;
    
    m_background = m_AudioManager->create(filepath.c_str(), filepath.c_str());
    if (m_background) {
        m_background->play2d();
    }
}

void SoundManager::resumeBackgroundSound() {
	if (m_background) m_background->play();
}

void SoundManager::pauseBackgroundSound() {
    if (m_background) m_background->pause();
}

bool SoundManager::backgroundSoundPlaying() {
    return m_background->isPlaying();
}

void SoundManager::playSoundAt(const std::string &file, const irr::core::vector3df &origin) {
    auto source = m_AudioManager->create(file.c_str(), file.c_str());
    if (source) {
        source->play3d(cVector3{origin.X, origin.Y, origin.Z});
    }
}
