#include "physics.hpp"

void PhysicsEngine :: applyGravity(Sprite* sprite, double delta)
{
    double speed  = gravity * delta;
    sprite->editToScreen(0, gravity, 0, 0);
}
