#ifndef RELWARB_PARTICLES_H
#define RELWARB_PARTICLES_H

#include "relwarb_defines.h"
#include "relwarb_math.h"

#include <vector>

struct GameState;
struct Bitmap;

struct Particle
{
	z::vec2 p;
	z::vec2 dp;

	z::vec4 color;

	real32 totalLife;
	real32 life;
};

struct ParticleSystem
{
	real32 systemLife;
	bool32 alive = false;

	z::vec2 pos;
	int     particlesPerSecond;
	real32  particleLife;
	real32  particleLifeDelta;

	z::vec4 startColor;
	z::vec4 endColor;
	Bitmap* particleBitmap;

	real32 minAngle;
	real32 maxAngle;

	real32 minVelocity;
	real32 maxVelocity;

	z::vec2 gravity;

	// TODO(Charly): Collision related stuff

	std::vector<Particle> particles;
};

ParticleSystem* SpawnParticleSystem(z::vec2 pos);
void            UpdateParticleSystems(real32 dt);

#endif // RELWARB_PARTICLES_H