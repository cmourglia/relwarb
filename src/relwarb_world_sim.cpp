#include "relwarb_world_sim.h"

#include <utility>
#include <vector>
#include <algorithm>

#include "relwarb_defines.h"
#include "relwarb_utils.h"
#include "relwarb_entity.h"
#include "relwarb_debug.h"
#include "relwarb.h"

void UpdateWorld(GameState* gameState, real32 dt)
{
    // NOTE(Charly): I have removed generic integration stuff for now.
    //               This function might actually be scripted, or call
    //               scripted entity update functions.
    for (uint32 entityIdx = 0; entityIdx < gameState->nbEntities; ++entityIdx)
    {
        Entity* entity = &gameState->entities[entityIdx];

        switch (entity->entityType)
        {
            case EntityType_Player:
            {
                // NOTE(Charly): We are updating a player, so we need to :
                //  - Change x velocity based on left / right inputs
                //  - If jump is pressed:
                //      - Is it the start of a new jump ?
                //          - Y: start jumping, compute gravity and velocity
                //          based on current state and wished jump height,
                //          keep track of the number of total jumps (gd related)
                //          - N: update jumping elapsed time
                //  - Else:
                //      - Did we begin a jump and stopped early ?
                //          - Change the gravity momentarily and track time

                Controller* controller = entity->controller;

#define MAX_JUMP_TIME   0.25f
#define MAX_STOP_TIME   0.05f
#define MAX_NB_JUMPS    2

                real32 oldX = entity->p.x;

                z::vec2 acc = z::Vec2(0, entity->gravity);
                entity->dp.x = 0.0;

                if (!(entity->status & (EntityStatus_Rooted | EntityStatus_Stunned)))
                {
                    if (controller->moveLeft)
                    {
                        entity->dp.x += -10.0;
                    }

                    if (controller->moveRight)
                    {
                        entity->dp.x += 10.0;
                    }

                    if (controller->jump)
                    {
                        if (controller->newJump && (!entity->alreadyJumping || (entity->newJump && entity->nbJumps < MAX_NB_JUMPS)))
                        {
                            // Start jumping
                            entity->dp.y = entity->initialJumpVelocity;
                            entity->alreadyJumping = true;
                            entity->newJump = false;
                            ++entity->nbJumps;
                            WentAirborne(entity);
                        }
                        else
                        {
                            entity->jumpTime += dt;
                        }
                    }
                    else
                    {
                        entity->newJump = true;

                        if (entity->alreadyJumping)
                        {
                            if (!entity->quickFall && entity->jumpTime < MAX_JUMP_TIME)
                            {
                                entity->quickFall = true;
                                entity->quickFallTime = 0;
                            }

                            if (entity->quickFall && entity->quickFallTime < MAX_STOP_TIME)
                            {
                                entity->quickFallTime += dt;
                                acc.y *= 5;
                            }
                        }
                    }
                }

                entity->p += dt * entity->dp + (0.5 * dt * dt * acc);
                entity->dp += dt * acc;

                // NOTE(Thomas): I don't like that it's handle in a physic resolution function while it's "game logic" related (or graphic related)
                if (z::OppositeSign(entity->p.x - oldX, entity->orientation))
                {
                    entity->orientation *= -1.f;
                }
            } break;

            default:
            {
            }
        }
    }

    // Update particle systems
    for (uint32 systemIdx = 0; systemIdx < MAX_PARTICLE_SYSTEMS; ++systemIdx)
    {
        ParticleSystem* system = gameState->particleSystems + systemIdx;

        if (system->alive)
        {
            // Spawn new particles for the current system
            int newParticlesCount = system->particlesPerSecond * dt;
            //Log(Log_Debug, "system %i should spawn %i particles", systemIdx, newParticlesCount);
            for (int particleIdx = 0; particleIdx < newParticlesCount; ++particleIdx)
            {
                real angle = z::GenerateRandBetween(system->minAngle, system->maxAngle);
                real vel = z::GenerateRandBetween(system->minVelocity, system->maxVelocity);

                Particle particle;
                particle.p = system->pos;
                particle.dp = z::Vec2(vel * z::Cos(angle), vel * z::Sin(angle));
                particle.color = system->startColor;
                particle.life = z::GenerateRandNormal(system->particleLife, system->particleLifeDelta);
                particle.totalLife = particle.life;

                system->particles.push_back(particle);
            }
            // Update system lifetime
            system->systemLife -= dt;
            if (system->systemLife <= 0.f)
            {
                system->alive = false;
            }
        }

        if (!system->particles.empty())
        {
            // Step particles simulation for the current system
            for (auto& particle : system->particles)
            {
                particle.dp += system->gravity * dt;
                particle.p += particle.dp * dt;

                real t = (particle.totalLife - particle.life) / (particle.totalLife);
                particle.color = Lerp(system->startColor, system->endColor, t);

                // Update particle lifetime
                particle.life -= dt;
            }

            size_t before = system->particles.size();

            std::vector<size_t> toBeRemoved;
            size_t particleCount = system->particles.size();
            for (size_t i = 0; i < particleCount; ++i)
            {
                if (system->particles[i].life <= 0.f)
                {
                    toBeRemoved.push_back(i);
                }
            }

            for (size_t i = 0; i < toBeRemoved.size(); ++i)
            {
                std::swap(system->particles[toBeRemoved[i]], system->particles[particleCount - i - 1]);
            }

            for (size_t i = 0; i < toBeRemoved.size(); ++i)
            {
                system->particles.pop_back();
            }

            size_t after = system->particles.size();
            //Log(Log_Debug, "System #%i: %zu %zu", systemIdx, before, after);
        }
    }

    // 2. Collision detection
    // Depending on the types of shapes we want collision for (I think I won't
    // be wrong if I say that we want other stuff than AABBs), we might need
    // a pruning phase (only test collisions for stuff that can collide).
    // Then, for each potentially colliding pair of entities, perform the test
    // (Depending on the shapes, GJK might be the best tool)

    // TODO(Thomas): Do something smart.
    std::vector<std::pair <Entity *, Entity *>> collisions;

    for (uint32 firstIdx = 0; firstIdx < (gameState->nbEntities - 1); ++firstIdx)
    {
        Entity* firstEntity = &gameState->entities[firstIdx];
        if (EntityHasComponent(firstEntity, ComponentFlag_Collidable))
        {
            for (uint32 secondIdx = firstIdx + 1; secondIdx < gameState->nbEntities; ++secondIdx)
            {
                Entity* secondEntity = &gameState->entities[secondIdx];
                if (EntityHasComponent(secondEntity, ComponentFlag_Collidable))
                {
                    if (Intersect(firstEntity, secondEntity))
                    {
                        collisions.push_back(std::pair<Entity *, Entity *>(firstEntity, secondEntity));
                    }
                }
            }
        }
    }

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
    //        (an AABB used for a checkpoint), this is the whole point of returning
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

    for (auto it : collisions)
    {
        z::vec2 overlap = Overlap(it.first, it.second);
        if (CollisionCallback(it.first, it.second, &overlap))
        {
            Assert(EntityHasComponent(it.first, ComponentFlag_Movable) || EntityHasComponent(it.second, ComponentFlag_Movable));

            if (EntityHasComponent(it.first, ComponentFlag_Movable) && EntityHasComponent(it.second, ComponentFlag_Movable))
            {
                // TODO(Thomas): Handle collision w.r.t respective weights
            }
            else
            {
                z::vec2 clampDp;
                if (z::Abs(overlap.x) < z::Abs(overlap.y))
                {
                    overlap = overlap * z::Vec2(1.f, 0.f);
                    clampDp = z::Vec2(0.f, 1.f);
                }
                else
                {
                    overlap = overlap * z::Vec2(0.f, 1.f);
                    clampDp = z::Vec2(1.f, 0.f);
                }
                if (EntityHasComponent(it.first, ComponentFlag_Movable))
                {
                    it.first->p -= overlap;
                    it.first->dp = it.first->dp * clampDp;
                }
                else // (EntityHasFlag(it.second, ComponentFlag_Movable))
                {
                    it.second->p += overlap;
                    it.second->dp = it.second->dp * clampDp;
                }
            }
        }
    }
}

bool32 CollisionCallback(Entity* e1, Entity* e2, void* userParam)
{
    // NOTE(Thomas): Only if Player against Wall or something, or always ?
    // NOTE(Thomas): else if or just if ?
    if (e1->entityType == EntityType_Player && e2->entityType == EntityType_Wall)
    {
        z::vec2* overlap = static_cast<z::vec2*>(userParam);
        if (z::Abs(overlap->y) < z::Abs(overlap->x) && overlap->y > 0)
        {
            Landed(e1);
        }
    }
    else if (e2->entityType == EntityType_Player && e1->entityType == EntityType_Wall)
    {
        z::vec2* overlap = static_cast<z::vec2*>(userParam);
        if (z::Abs(overlap->y) < z::Abs(overlap->x) && overlap->y > 0)
        {
            Landed(e2);
        }
    }

    return true;
}

RigidBody* CreateRigidBody(GameState* gameState, real32 mass)
{
    ComponentID id = gameState->nbRigidBodies++;
    Assert(id < WORLD_SIZE);
    RigidBody* result = &gameState->rigidBodies[id];

    result->invMass = (mass == 0.f ? 0.f : 1.f / mass);

    return result;
}

Shape* CreateShape(GameState* gameState, z::vec2 size_, z::vec2 offset_)
{
    ComponentID id = gameState->nbShapes++;
    Assert(id < WORLD_SIZE);
    Shape* result = &gameState->shapes[id];

    result->size = size_;
    result->offset = offset_;

    return result;
}

ParticleSystem* SpawnParticleSystem(GameState* gameState, z::vec2 pos)
{
    uint32 idx;
    for (idx = 0; idx < MAX_PARTICLE_SYSTEMS && gameState->particleSystems[idx].alive; ++idx);
    Assert(idx < MAX_PARTICLE_SYSTEMS);

    ParticleSystem* result = &gameState->particleSystems[idx];

    Log(Log_Info, "Hello @ %.3f %.3f", pos.x, pos.y);

    result->pos = pos;
    result->systemLife = 2;
    result->alive = true;
    result->particlesPerSecond = 1000;
    result->particleLife = 1;
    result->particleLifeDelta = 0.25;
    result->startColor = z::Vec4(1, 1, 1, 1);
    result->endColor = z::Vec4(1, 1, 1, 0);
    result->particleBitmap = &gameState->particleBitmap;
    result->minAngle = (1.0 / 3.0) * z::Pi;
    result->maxAngle = (2.0 / 3.0) * z::Pi;
    result->minVelocity = 13;
    result->maxVelocity = 17;
    result->gravity = z::Vec2(0, -20);

    return result;
}

void AddRigidBodyToEntity(Entity* entity, RigidBody* body)
{
    entity->body = body;
    SetEntityComponent(entity, ComponentFlag_Movable);
}

void AddShapeToEntity(Entity* entity, Shape* shape)
{
    entity->shape = shape;
    SetEntityComponent(entity, ComponentFlag_Collidable);
}
