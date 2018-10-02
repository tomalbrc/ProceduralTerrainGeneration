//
//  Utils.cpp
//  ProcMapGeneration
//
//  Created by Tom Albrecht on 05.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#include <stdio.h>
#include <mutex>
#include <chrono>
#include <thread>
#include <future>
#include "Utils.h"

using namespace tom;

tom::MainCallbackQueue tom::threading::mainCallbackQueue;
std::mutex tom::threading::mainCallbackQueueMutex;


/**
 * Manages the queue of std::function.
 * Call this from the main thread in order for threading::addMainCallback(...) to work properly
 */
void threading::manageMainthreadCallbacks() {
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

/**
 * Executes a lambda on the main thread. (preferrably a mutable c++14 one)
 * Remember to call threading::manageMainthreadCallbacks() in your main function!
 */
void threading::addMainCallback(const std::function<void(void)> &f) {
    std::packaged_task<void()> task(std::move(f)); //!!
    std::lock_guard<std::mutex> lock{mainCallbackQueueMutex};
    mainCallbackQueue.push_back(std::move(task));
}

/**
 * Executes a lambda on a separate thread. (preferrably a mutable c++14 one)
 */
void threading::onSeparateThread(const std::function<void(void)> &f) {
    std::thread thread{f};
    thread.detach();
}

/**
 * Sleeps for ms milliseconds
 */
void threading::sleep(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}


/// MARK: Game Utils
std::vector<irr::s32> shader::setupShader(irr::IrrlichtDevice *device, const irr::core::vector2df& chunkSize, const float& quadScale, irr::scene::ILightSceneNode *light) {
    std::vector<irr::s32> res;
    
    irr::video::IGPUProgrammingServices *gps = device->getVideoDriver()->getGPUProgrammingServices();
    if (gps) {
        const irr::video::E_GPU_SHADING_LANGUAGE shadingLanguage = EGSL_DEFAULT;
        
        auto mc = new TerrainShaderCallback{ device };
        mc->chunkSize(chunkSize);
        mc->quadScale(quadScale);
        mc->lightSource(light);
        
        auto glslVer = device->getVideoDriver()->getDriverAttributes().getAttributeAsInt("ShaderLanguageVersion");
        
        irr::s32 matType1, matType2, matType3;
        if (glslVer == 102 || glslVer == 120) {
            matType1 = gps->addHighLevelShaderMaterialFromFiles(
                                                                ResourcePath("shader/terrain120.vert"), "main", video::EVST_VS_1_1,
                                                                ResourcePath("shader/terrain120.frag"), "main", video::EPST_PS_1_1,
                                                                mc, video::EMT_SOLID, 0, shadingLanguage);
            
            matType2 = gps->addHighLevelShaderMaterialFromFiles(
                                                                ResourcePath("shader/tree120.vert"), "main", video::EVST_VS_1_1,
                                                                ResourcePath("shader/tree120.frag"), "main", video::EPST_PS_1_1,
                                                                mc, video::EMT_SOLID, 0, shadingLanguage);
            
            matType3 = gps->addHighLevelShaderMaterialFromFiles(
                                                                ResourcePath("shader/cloud120.vert"), "main", video::EVST_VS_1_1,
                                                                ResourcePath("shader/cloud120.frag"), "main", video::EPST_PS_1_1,
                                                                mc, video::EMT_SOLID, 0, shadingLanguage);
        } else if (glslVer == 103 || glslVer == 130) {
            matType1 = gps->addHighLevelShaderMaterialFromFiles(
                                                                ResourcePath("shader/terrain130.vert"), "main", video::EVST_VS_1_1,
                                                                ResourcePath("shader/terrain130.frag"), "main", video::EPST_PS_1_1,
                                                                mc, video::EMT_SOLID, 0, shadingLanguage);
            
            matType2 = gps->addHighLevelShaderMaterialFromFiles(
                                                                ResourcePath("shader/tree130.vert"), "main", video::EVST_VS_1_1,
                                                                ResourcePath("shader/tree130.frag"), "main", video::EPST_PS_1_1,
                                                                mc, video::EMT_SOLID, 0, shadingLanguage);
            
            matType3 = gps->addHighLevelShaderMaterialFromFiles(
                                                                ResourcePath("shader/cloud130.vert"), "main", video::EVST_VS_1_1,
                                                                ResourcePath("shader/cloud130.frag"), "main", video::EPST_PS_1_1,
                                                                mc, video::EMT_SOLID, 0, shadingLanguage);
        }
        
        //res.emplace_back(matType1, matType2);
        res.push_back(matType1);
        res.push_back(matType2);
        res.push_back(matType3);
    }
    return res;
}

