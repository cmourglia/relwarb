#include "relwarb_particles.h"

#include "relwarb.h"
#include "relwarb_game.h"

ParticleSystem* SpawnParticleSystem(z::vec2 pos)
{
	uint32 idx;
	for (idx = 0; idx < MAX_PARTICLE_SYSTEMS && state->particleSystems[idx].alive; ++idx)
		;
	Assert(idx < MAX_PARTICLE_SYSTEMS);

	ParticleSystem* result = &state->particleSystems[idx];

	Log(Log_Info, "Hello @ %.3f %.3f", pos.x, pos.y);

	result->pos                = pos;
	result->systemLife         = 2;
	result->alive              = true;
	result->particlesPerSecond = 1000;
	result->particleLife       = 1;
	result->particleLifeDelta  = 0.25;
	result->startColor         = z::Vec4(1, 1, 1, 1);
	result->endColor           = z::Vec4(1, 1, 1, 0);
	result->particleBitmap     = &state->particleBitmap;
	result->minAngle           = (1.0 / 3.0) * z::Pi;
	result->maxAngle           = (2.0 / 3.0) * z::Pi;
	result->minVelocity        = 13;
	result->maxVelocity        = 17;
	result->gravity            = z::Vec2(0, -20);

	return result;
}

void UpdateParticleSystems(real32 dt)
{
	// Update particle systems
	for (uint32 systemIdx = 0; systemIdx < MAX_PARTICLE_SYSTEMS; ++systemIdx)
	{
		ParticleSystem* system = state->particleSystems + systemIdx;

		if (system->alive)
		{
			// Spawn new particles for the current system
			int newParticlesCount = system->particlesPerSecond * dt;
			// Log(Log_Debug, "system %i should spawn %i particles", systemIdx, newParticlesCount);
			for (int particleIdx = 0; particleIdx < newParticlesCount; ++particleIdx)
			{
				real angle = z::GenerateRandBetween(system->minAngle, system->maxAngle);
				real vel   = z::GenerateRandBetween(system->minVelocity, system->maxVelocity);

				Particle particle;
				particle.p         = system->pos;
				particle.dp        = z::Vec2(vel * z::Cos(angle), vel * z::Sin(angle));
				particle.color     = system->startColor;
				particle.life      = z::GenerateRandNormal(system->particleLife,
                                                      system->particleLifeDelta);
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

				real t         = (particle.totalLife - particle.life) / (particle.totalLife);
				particle.color = Lerp(system->startColor, system->endColor, t);

				// Update particle lifetime
				particle.life -= dt;
			}

			size_t before = system->particles.size();

			std::vector<size_t> toBeRemoved;
			size_t              particleCount = system->particles.size();
			for (size_t i = 0; i < particleCount; ++i)
			{
				if (system->particles[i].life <= 0.f)
				{
					toBeRemoved.push_back(i);
				}
			}

			for (size_t i = 0; i < toBeRemoved.size(); ++i)
			{
				std::swap(system->particles[toBeRemoved[i]],
				          system->particles[particleCount - i - 1]);
			}

			for (size_t i = 0; i < toBeRemoved.size(); ++i)
			{
				system->particles.pop_back();
			}

			size_t after = system->particles.size();
			// Log(Log_Debug, "System #%i: %zu %zu", systemIdx, before, after);
		}
	}
}