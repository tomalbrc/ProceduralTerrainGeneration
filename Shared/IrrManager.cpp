//
//  IrrManager.cpp
//  ProcMapGeneration
//
//  Created by Tom Albrecht on 30.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#include "IrrManager.h"
#include "WorldInfo.h"

WorldInfo *IrrManager::worldInfo() {
    return m_worldInfo;
}

void IrrManager::worldInfo(WorldInfo *wi) {
    m_worldInfo = wi;
}
