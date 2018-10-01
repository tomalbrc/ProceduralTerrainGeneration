//
//  PhysicsManager.cpp
//  ProcMapGeneration
//
//  Created by Tom Albrecht on 29.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#include "PhysicsManager.h"

using namespace irr;
using namespace irr::core;

// func prototype
btTriangleMesh *createTriangleMesh(irr::scene::IMesh* const mesh, irr::scene::ISceneNode *node);

PhysicsManager::PhysicsManager(irr::IrrlichtDevice *device, irr::core::vector3df gravity) {
    m_world = new irrBulletWorld(device, true, true);
    m_world->setGravity(gravity);
    m_gravity = gravity;
    m_playerNode = nullptr;
}

void PhysicsManager::setPlayer(irr::scene::IMesh *const mesh, irr::scene::ISceneNode *const node) {
    m_charController = new IKinematicCharacterController(m_world);
    m_charController->setGravity(m_gravity.Y);
    m_charController->setJumpForce(40.f);
    m_playerNode = node;
}

void PhysicsManager::jump(irr::f32 power) {
    m_charController->jump();
}

IRigidBody* PhysicsManager::addGroundShape(irr::scene::IMesh * const mesh, irr::scene::ISceneNode * const node) {
    auto shape = new IBvhTriangleMeshShape(node, mesh, 0.f);
    
    IRigidBody *body;
    body = m_world->addRigidBody(shape);
    body->setMassProps(0, vector3df{});
    
    return body;
}
void PhysicsManager::warp(vector3df origin) {
    m_charController->warp(origin);
}


IRigidBody* PhysicsManager::addEntity(irr::scene::IMesh * const mesh, irr::scene::ISceneNode * const node) {
    auto shape = new IBoxShape(node, 1.f, false);
    
    IRigidBody *body;
    body = m_world->addRigidBody(shape);
    
    entities[node] = body;
    
    return body;
}

void PhysicsManager::removeEntity(irr::scene::ISceneNode *node) {
    auto rb = entities[node];
    m_world->removeCollisionObject(rb);
}


void PhysicsManager::update(double dt) {
    m_world->stepSimulation(dt);
    if (m_playerNode) {
        m_charController->playerStep(m_world, dt);
        m_playerNode->setPosition(m_charController->getWorldTransform().getTranslation());
    }
}

void PhysicsManager::move(irr::core::vector3df mov) {
    if (m_playerNode) m_charController->setWalkDirection(mov);
}

bool PhysicsManager::grounded() {
    return m_charController->isOnGround();
}

irrBulletWorld *PhysicsManager::world() const {
    return m_world;
}

