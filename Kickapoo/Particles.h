#pragma once
#include "Common.h"

class Particle;

class IParticleSystem
{
public:
	virtual float currentTime() const = 0;
};

class Particle
{
public:
	D3DXVECTOR2 start;
	D3DXVECTOR2 dirVec;
	bool looping;
	float lifeTime;
	float velocity;
	D3DCOLOR color;
	float size;

	Particle(IParticleSystem * _pSystem, const D3DXVECTOR2& pos, const D3DXVECTOR2& dir,
		bool loop, float lifeTime, float velocity, D3DCOLOR color, float size);
	~Particle();
	bool updateState();

	const D3DXVECTOR2& pos() const { return position; }
private:
	IParticleSystem * particleSystem;
	float respawnTime;
	D3DXVECTOR2 position;
};

class ParticleSystem : public IParticleSystem, public Singleton<ParticleSystem>
{
public:
	ParticleSystem();
	virtual ~ParticleSystem();

	void updateParticles();
	void spawnParticle(const D3DXVECTOR2& pos, const D3DXVECTOR2& direction,
		bool looping, float lifeTime, float velocity, D3DCOLOR color, float size);

	void renderParticles();

private: // IParticleSystem
	virtual float currentTime() const;

private:
	std::list<Particle *> instances;
	float curTime;
};

DefineAccessToSingleton(ParticleSystem)
