//
//  Utils.h
//  ProcMapGeneration-macOS
//
//  Created by Tom Albrecht on 05.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//
#pragma once

#ifndef Utils_h
#define Utils_h
#include "TerrainShaderCallback.h"
#include <functional>
#include <mutex>
#include <deque>
#include <string>
#include <vector>
#include <unistd.h>

static const irr::core::stringc ResourcePath(const char* resource) {
    auto cwd = getcwd(NULL, 0);
#ifdef __APPLE__
    strcat(cwd, "/ProcMapGeneration-macOS.app/Contents/Resources/");
#else
    strcat(cwd, "/");
#endif
    strcat(cwd, resource);
    return cwd;
}

namespace tom {
    using namespace irr;
    using namespace video;
    
    using MainCallback = std::packaged_task<void()>;
    using MainCallbackQueue = std::deque<MainCallback>;
    
    class threading {
    public:
        /**
         * Manages the queue of std::function.
         * all this on the main thread in order for threading::addMainCallback to work properly
         */
        static void manageMainthreadCallbacks();
        
        /**
         * Executes a lambda on the main thread. (preferrably a mutable c++14 one)
         * Remember to call threading::manageMainthreadCallbacks() in your main function!
         */
        static void addMainCallback(const std::function<void(void)> &f);
        
        /**
         * Executes a lambda on a separate thread. (preferrably a mutable c++14 one)
         */
        static void onSeparateThread(const std::function<void(void)> &f);
        
        /**
         * Sleeps for ms milliseconds
         */
        static void sleep(int ms);
    private:
        static MainCallbackQueue mainCallbackQueue;
        static std::mutex mainCallbackQueueMutex;
    };
    
    ///
    /// game utils
    ///
    class shader {
    public:
        static std::vector<irr::s32> setupShader(irr::IrrlichtDevice *device, const irr::core::vector2df& chunkSize, const float& quadScale, irr::scene::ILightSceneNode *light);
    };
}


#endif /* Utils_h */
