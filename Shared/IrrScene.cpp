//
//  IrrScene.cpp
//  ProcMapGeneration-macOS
//
//  Created by Tom Albrecht on 09.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#include "IrrScene.h"
#include <assert.h>

IrrScene::IrrScene(irr::IrrlichtDevice *device) : m_device{device} {
    assert(m_device);
}

irr::IrrlichtDevice *IrrScene::device() const {
    return m_device;
}

void IrrScene::device(irr::IrrlichtDevice *device) {
    m_device = device;
}
