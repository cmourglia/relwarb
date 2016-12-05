
#include "relwarb_defines.h"

void UpdateWorld(real32 dt)
{
    // Here is where the magic happens
    // Multiple stuff has to be done here, for each dynamic entities
    // (objects, actors or however you wanna call them).

    // 1. Integration step (Symplectic-Euler):
    // for each entity
    //      - Add gravity if needed :
    //          This needs more thinking / testing : how do we want to handle
    //          dashes, spells that may require a big air-time, etc ?
    //      - Compute acceleration (Newton FTW, a(t) = F(t) * 1 / m)
    //      - Compute new velocity : v(t) = v(t-1) + a(t) * dt
    //      - Compute new position : p(t) = p(t-1) + v(t) * dt
    //
    //      - NOTE(Charly): Forward Euler would have been
    //          p(t) = p(t-1) + v(t-1) * dt
    //          v(t) = v(t-1) + a(t) * dt
    //
    // 2. Collision detection
    // Depending on the types of shapes we want collision for (I think I won't
    // be wrong if I say that we want other stuff than AABBs), we might need
    // a pruning phase (only test collisions for stuff that can collide).
    // Then, for each potentially colliding pair of entities, perform the test
    // (Depending on the shapes, GJK might be the best tool)
    //
    // 3. Collision solving
    // for each collision
    //      - Call a callback function which goal is to enable gameplay programming.
    //        This could look something like
    //        bool32 CollisionCallback(Entity* e1, Entity* e2, void* userParam)
    //        The idea is that, depending on the objects colliding, you might want to
    //        be able to do some particular work. For example, if a character is
    //        colliding against a wall where there are many spikes, you want him to
    //        loose HP. You might want some destruction, or particles to spawn
    //        (you hit a fire hydrant), or you might want to trigger some event on
    //        some not visible colliding objects (you ran through a checkpoint,
    //        or on an invisible trap).
    //        Depending on the case, you might not want to enable the collision solving
    //        (an AABB used for a checkpoint), this is the wholo point of returning
    //        a boolean as a result of that function. You want, or not, the collision
    //        to be solved.
    //
    //      - if (CollisionCallback(e1, e2)) {
    //          Now you want to solve the collision.
    //          The easiest way to do it, is by directly modifying the position of
    //          the objects overlapping.
    //          Objects move along the normal of the contact, and their position is
    //          modified so that they just do not overlap anymore.
    //          The mass of each object can be used to weight the amount of correction
    //          for each object (The heavier, the harder to move. Immovable objects
    //          have an infinite mass / null inverse mass)
    //      }
}
