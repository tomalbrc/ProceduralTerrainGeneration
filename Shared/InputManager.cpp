//
//  InputManager.cpp
//  ProcMapGeneration
//
//  Created by Tom Albrecht on 30.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#include "InputManager.h"
#include "WorldInfo.h"

using namespace irr;
using namespace core;

static const irr::core::vector3df kPlayerSpawnPosition;

InputManager::InputManager(IrrlichtDevice *device) {
    m_eventReceiver = new MapControlEventReceiver();
    device->setEventReceiver(m_eventReceiver);
    m_device = device;
}

InputManager::~InputManager() {
    //m_device->setEventReceiver(nullptr);
}

void InputManager::setupKeyHandler() {
    m_eventReceiver->setPressedKeyHandler([this](irr::EKEY_CODE kc){
        if (kc == irr::KEY_KEY_O) {
            if (!worldInfo()->sound->backgroundSoundPlaying()) {
                worldInfo()->sound->resumeBackgroundSound();
            } else worldInfo()->sound->pauseBackgroundSound();
        }
        else if (kc == irr::KEY_KEY_N) {
            viewDistance++;
        } else if (kc == irr::KEY_KEY_M) {
            viewDistance--;
        } else if (kc == irr::KEY_F1) {
            worldInfo()->physics->warp(kPlayerSpawnPosition);
            worldInfo()->camera->setPosition(kPlayerSpawnPosition);
        }  else if (kc == irr::KEY_SPACE) {
            if (worldInfo()->physics->grounded()) worldInfo()->physics->jump(30.f);
        } else if (kc == irr::KEY_F2) {
            // TODO: Update to use irrBullet
            if (worldInfo()->player->getAnimators().size() == 0) {
                //this->setupCollisionAnimator();
            } else {
                //player->removeAnimators();
            }
        } else if (kc == irr::KEY_F4) {
            
            for (auto & node : worldInfo()->chunks) {
                auto chldr = node.second->getChildren();
                for (auto i = chldr.begin(); i != chldr.end(); i++) {
                    worldInfo()->effectHandler->removeShadowFromNode(*i);
                    (*i)->remove();
                }
                
                worldInfo()->effectHandler->removeShadowFromNode(node.second);
                node.second->remove();
            }
            worldInfo()->chunks.clear();
            
        } else if (kc == irr::KEY_KEY_P) {
            worldInfo()->entity->spawnEnemy();
        } else if (kc == irr::KEY_KEY_R) {
            worldInfo()->entity->entities()[worldInfo()->player].ammo = 1000;
        } else if (kc == irr::KEY_ESCAPE) {
            worldInfo()->device->closeDevice();
        }
    });
}

void InputManager::manageInput(MapControlEventReceiver *eventReceiver, irr::scene::ISceneNode *player, irr::scene::ICameraSceneNode *cam2, f32 deltaTime) {
    auto x = eventReceiver->mouseInformation().x;
    auto screenW = (1920 * 0.75f);
    auto perc = x / screenW;
    auto angle = 360.f*2 * -perc;
    float yVal = sin(irr::core::degToRad(angle));
    float xVal = cos(irr::core::degToRad(angle));
    
    vector3df moveVal{0};
    float value = (eventReceiver->keyPressed(irr::KEY_SHIFT) ? 10.f : 0.5f)*100.f * deltaTime;
    if (eventReceiver->keyPressed(irr::KEY_RIGHT) || eventReceiver->keyPressed(irr::KEY_KEY_D)) moveVal += (irr::core::vector3df(value*cos(irr::core::degToRad(angle - 90.f)), 0.f, value*sin(irr::core::degToRad(angle - 90.f))));
    if (eventReceiver->keyPressed(irr::KEY_LEFT) || eventReceiver->keyPressed(irr::KEY_KEY_A)) moveVal += irr::core::vector3df(value*cos(irr::core::degToRad(angle + 90.f)), 0.f, value*sin(irr::core::degToRad(angle  +90.f)));
    if (eventReceiver->keyPressed(irr::KEY_UP) || eventReceiver->keyPressed(irr::KEY_KEY_W)) moveVal += irr::core::vector3df(value*xVal, 0.f, value*yVal);
    if (eventReceiver->keyPressed(irr::KEY_DOWN) || eventReceiver->keyPressed(irr::KEY_KEY_S)) moveVal += irr::core::vector3df(value*cos(irr::core::degToRad(angle + 180.f)), 0.f, value*sin(irr::core::degToRad(angle + 180.f)));
    worldInfo()->physics->move(moveVal/2.f);
    
    worldInfo()->camera->setTarget(worldInfo()->player->getPosition());
    worldInfo()->player->setRotation(irr::core::vector3df(0,-angle,0));
    
    auto newPosition = worldInfo()->player->getPosition() + irr::core::vector3df(-50 * xVal, 10.f, -50 * yVal);
    float lowpassfilterFactor = (.1f);
    newPosition = (newPosition * lowpassfilterFactor) + (cam2->getPosition() * (1.0 - lowpassfilterFactor));
    worldInfo()->camera->setPosition(newPosition);
    
    auto &lll2 = worldInfo()->effectHandler->getShadowLight(0);
    auto pos = worldInfo()->player->getPosition();
    pos.Y += 100.f;
    pos.X -= 30.f;
    pos.Z -= 30.f;
    lll2.setPosition(pos);
    lll2.setTarget(worldInfo()->player->getPosition());
    
}

void InputManager::update(double dt) {
    manageInput(m_eventReceiver, worldInfo()->player, worldInfo()->camera, dt);
}

MapControlEventReceiver *InputManager::eventReceiver() {
    return m_eventReceiver;
}
