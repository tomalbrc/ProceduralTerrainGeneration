//
//  PhysicsManager.hpp
//  ProcMapGeneration-macOS
//
//  Created by Tom Albrecht on 29.09.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#ifndef PhysicsManager_hpp
#define PhysicsManager_hpp

#include <stdio.h>
#include <map>

#include <irrBullet.h>

/**
 * Easy way to add scene nodes, meshes to a physics simulation
 */
class PhysicsManager {
public:
    PhysicsManager() = default;
    PhysicsManager(irr::IrrlichtDevice *device, irr::core::vector3df gravity);
    
    /**
     * Add rigid body
     */
    void setPlayer(irr::scene::IMesh *const mesh, irr::scene::ISceneNode *const node);
    
    IRigidBody* addGroundShape(irr::scene::IMesh * const mesh, irr::scene::ISceneNode * const node);
    
    /**
     * Add an entity to the physics sim
     */
    IRigidBody* addEntity(irr::scene::IMesh * const mesh, irr::scene::ISceneNode * const node);
    
    /**
     * update/step the physics sim
     */
    void update(double dt);
    
    void jump(irr::f32 power);
    
    void warp(irr::core::vector3df origin);
    
    void move(irr::core::vector3df mov);
    
    bool grounded();
    
private:
    irrBulletWorld *m_world;
    irr::core::vector3df m_gravity;
    
    irr::scene::ISceneNode *m_playerNode;
    IKinematicCharacterController *m_charController;
    std::map<irr::scene::ISceneNode*, IRigidBody*> entities;
};

#endif /* PhysicsManager_hpp */
