//
//  Utils.cpp
//  ProcMapGeneration-macOS
//
//  Created by Tom Albrecht on 05.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#include <stdio.h>
#include <mutex>
#include "Utils.h"

tom::MainCallbackQueue tom::threading::mainCallbackQueue;
std::mutex tom::threading::mainCallbackQueueMutex;

