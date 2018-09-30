//
//  IrrManager.h
//  ProcMapGeneration
//
//  Created by Tom Albrecht on 30.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#ifndef IrrManager_h
#define IrrManager_h

#include <stdio.h>

class WorldInfo;

class IrrManager {
public:
    
    WorldInfo *worldInfo();
    void worldInfo(WorldInfo *wi);
    
    virtual void update(double dt) = 0;
    
protected:
    WorldInfo *m_worldInfo;
};

#endif /* IrrManager_h */
