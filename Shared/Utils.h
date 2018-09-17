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
#include <future>
#include <functional>
#include <mutex>
#include <thread>
#include <deque>
#include <vector>

namespace tom {
    using namespace irr;
    using namespace video;
    
    using MainCallback = std::packaged_task<void()>;
    using MainCallbackQueue = std::deque<MainCallback>;
    
    class threading {
    public:
        static MainCallbackQueue mainCallbackQueue;
        static std::mutex mainCallbackQueueMutex;
        
        static void manageMainthreadCallbacks() {
            std::unique_lock<std::mutex> lock(mainCallbackQueueMutex);
            if (!mainCallbackQueue.empty()) {
                // executeon main
                auto fun = std::move(mainCallbackQueue.front());
                mainCallbackQueue.pop_front(); // throw away
                lock.unlock();
                fun();
                lock.lock();
                printf("executed task\n");
            }
        }
        
        static void addMainCallback(const std::function<void(void)> &f) {
            std::packaged_task<void()> task(std::move(f)); //!!
            std::lock_guard<std::mutex> lock{mainCallbackQueueMutex};
            mainCallbackQueue.push_back(std::move(task));
        }
        
        static void onSeparateThread(const std::function<void(void)> &f) {
            std::thread thread{f};
            thread.detach();
        }
    };
    
    ///
    /// game utils
    ///
    
    static std::vector<irr::s32> setupShader(irr::IrrlichtDevice *device, const irr::core::vector2df& chunkSize, const float& quadScale) {
        std::vector<irr::s32> res;
        
        irr::video::IGPUProgrammingServices *gps = device->getVideoDriver()->getGPUProgrammingServices();
        
        if (gps) {
            const irr::video::E_GPU_SHADING_LANGUAGE shadingLanguage = EGSL_DEFAULT;
            
            auto mc = new TerrainShaderCallback{ device };
			mc->chunkSize(chunkSize);
			mc->quadScale(quadScale);
#ifdef _WIN32
			irr::s32 matType1 = gps->addHighLevelShaderMaterialFromFiles(
				"shader/terrain130.vert", "main", video::EVST_VS_1_1,
				"shader/terrain130.frag", "main", video::EPST_PS_1_1,
				mc, video::EMT_SOLID, 0, shadingLanguage);

			irr::s32 matType2 = gps->addHighLevelShaderMaterialFromFiles(
				"shader/tree130.vert", "main", video::EVST_VS_1_1,
				"shader/tree130.frag", "main", video::EPST_PS_1_1,
				mc, video::EMT_SOLID, 0, shadingLanguage);

			irr::s32 matType3 = gps->addHighLevelShaderMaterialFromFiles(
				"shader/cloud130.vert", "main", video::EVST_VS_1_1,
				"shader/cloud130.frag", "main", video::EPST_PS_1_1,
				mc, video::EMT_SOLID, 0, shadingLanguage);

#else
			irr::s32 matType1 = gps->addHighLevelShaderMaterialFromFiles(
				"shader/terrain120.vert", "main", video::EVST_VS_1_1,
				"shader/terrain120.frag", "main", video::EPST_PS_1_1,
				mc, video::EMT_SOLID, 0, shadingLanguage);

			irr::s32 matType2 = gps->addHighLevelShaderMaterialFromFiles(
				"shader/tree120.vert", "main", video::EVST_VS_1_1,
				"shader/tree120.frag", "main", video::EPST_PS_1_1,
				mc, video::EMT_SOLID, 0, shadingLanguage);

			irr::s32 matType3 = gps->addHighLevelShaderMaterialFromFiles(
				"shader/cloud120.vert", "main", video::EVST_VS_1_1,
				"shader/cloud120.frag", "main", video::EPST_PS_1_1,
				mc, video::EMT_SOLID, 0, shadingLanguage);

#endif

            //res.emplace_back(matType1, matType2);
            res.push_back(matType1);
            res.push_back(matType2);
            res.push_back(matType3);
        }
        return res;
    }
}


#endif /* Utils_h */
