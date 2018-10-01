//
//  PhysicsManager.h
//  ProcMapGeneration
//
//  Created by Tom Albrecht on 29.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#ifndef PhysicsManager_h
#define PhysicsManager_h

#include <stdio.h>
#include <map>
#include "IrrManager.h"
#include <irrBullet.h>

/**
 * Easy way to add scene nodes, meshes to a physics simulation
 * using the irrBullet wrapper for bullet3
 */
class PhysicsManager : public IrrManager {
public:
    PhysicsManager() = default;
    PhysicsManager(irr::IrrlichtDevice *device, irr::core::vector3df gravity);
    
    /**
     * Add rigid body
     */
    void setPlayer(irr::scene::IMesh *const mesh, irr::scene::ISceneNode *const node);
    
    /**
     * Adds a static node
     */
    IRigidBody* addGroundShape(irr::scene::IMesh * const mesh, irr::scene::ISceneNode * const node);
    
    /**
     * Add an entity to the physics sim
     */
    IRigidBody* addEntity(irr::scene::IMesh * const mesh, irr::scene::ISceneNode * const node);
    
    /**
     * Removes a node from the phys sim
     */
    void removeEntity(irr::scene::ISceneNode *node);
    
    /**
     * update/step the physics sim
     */
    virtual void update(double dt) override;
    
    /**
     * Makes the player('s character controller) jump
     */
    void jump(irr::f32 power);
    
    /**
     * "Teleports" the player in the physics sim
     */
    void warp(irr::core::vector3df origin);
    
    /**
     * Moves the player in the given direction vector
     */
    void move(irr::core::vector3df mov);
    
    /**
     * @return true if player is grounded
     */
    bool grounded();
    
    /**
     * The irrBullet world
     */
    irrBulletWorld * world() const;
    
private:
    irrBulletWorld *m_world;
    irr::core::vector3df m_gravity;
    
    irr::scene::ISceneNode *m_playerNode;
    IKinematicCharacterController *m_charController;
    std::map<irr::scene::ISceneNode*, IRigidBody*> entities;
};

#endif /* PhysicsManager_h */
