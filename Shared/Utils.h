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


namespace tom {
    using namespace irr;
    using namespace video;

    using MainCallback = std::packaged_task<void()>;
    using MainCallbackQueue = std::deque<MainCallback>;
    
    static MainCallbackQueue mainCallbackQueue;
    static std::mutex mainCallbackQueueMutex;
    
    inline void manageMainthreadCallbacks() {
        std::unique_lock<std::mutex> lock(tom::mainCallbackQueueMutex);
        if (!tom::mainCallbackQueue.empty()) {
            // executeon main
            auto fun = std::move(tom::mainCallbackQueue.front());
            tom::mainCallbackQueue.pop_front(); // throw away
            lock.unlock();
            fun();
            lock.lock();
            printf("executed task\n");
        }
    }
    
    inline void addMainCallback(const std::function<void(void)> &f) {
        std::packaged_task<void()> task(std::move(f)); //!!
        std::lock_guard<std::mutex> lock{tom::mainCallbackQueueMutex};
        tom::mainCallbackQueue.push_back(std::move(task));
    }
    
    inline void onSeparateThread(const std::function<void(void)> &f) {
        std::thread thread{f};
        thread.detach();
    }
    
    
    
    
    ///
    /// game utils
    ///
    
    inline std::vector<irr::s32> setupShader(irr::IrrlichtDevice *device) {
        std::vector<irr::s32> res;
        
        irr::video::IGPUProgrammingServices *gps = device->getVideoDriver()->getGPUProgrammingServices();
        if (gps) {
            const irr::video::E_GPU_SHADING_LANGUAGE shadingLanguage = EGSL_DEFAULT;
            
            auto mc = new TerrainShaderCallback{ device };
            
            auto matType1 = gps->addHighLevelShaderMaterialFromFiles(
                                                                     "ProcMapGeneration-macOS.app/Contents/Resources/file.vert", "vertexMain", video::EVST_VS_1_1,
                                                                     "ProcMapGeneration-macOS.app/Contents/Resources/file.frag", "pixelMain", video::EPST_PS_1_1,
                                                                     mc, video::EMT_SOLID, 0, shadingLanguage);
            
            auto matType2 = gps->addHighLevelShaderMaterialFromFiles(
                                                                     "file.vert", "vertexMain", video::EVST_VS_1_1,
                                                                     "file.frag", "pixelMain", video::EPST_PS_1_1,
                                                                     mc, video::EMT_TRANSPARENT_ADD_COLOR, 0, shadingLanguage);
            //res.emplace_back(matType1, matType2);
        }
        return res;
    }
}

#endif /* Utils_h */
