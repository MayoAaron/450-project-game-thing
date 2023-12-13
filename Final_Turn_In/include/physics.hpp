#ifndef PHYSICS_HPP
#define PHYSICS_HPP
#include "sprite.hpp"

class PhysicsEngine
{
public:
    void applyGravity(Sprite* sprite, double delta);
    bool resolveCollisions(Sprite* sprite);
private:
    double gravity = 9.8;
};

#endif