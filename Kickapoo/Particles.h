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
	int type;

	Particle(IParticleSystem * _pSystem, const D3DXVECTOR2& pos, const D3DXVECTOR2& dir,
		bool loop, float lifeTime, float velocity, D3DCOLOR color, float size, int type);
	virtual ~Particle();
	bool updateState();

	const D3DXVECTOR2& pos() const { return position; }

	virtual float applyVelocity(float v, float dt) const {
		return v * dt;
	}
private:
	IParticleSystem * particleSystem;
	float respawnTime;
	D3DXVECTOR2 position;
};

class Particle2 : public Particle
{
	FastDelegate2<float, float, float> _velocityTransform;
public:
	virtual float applyVelocity(float v, float dt) const
		{
		return _velocityTransform(v, dt);
		}

	Particle2(IParticleSystem * _pSystem, const D3DXVECTOR2& pos, const D3DXVECTOR2& dir,
		bool loop, float lifeTime, float velocity, D3DCOLOR color, float size, int type,
		const FastDelegate2<float, float, float>& _func);
	virtual ~Particle2();
};

class ParticleSystem : public IParticleSystem, public Singleton<ParticleSystem>
{
public:
	ParticleSystem();
	virtual ~ParticleSystem();

	void updateParticles();
	void spawnParticle(const D3DXVECTOR2& pos, const D3DXVECTOR2& direction,
		bool looping, float lifeTime, float velocity, D3DCOLOR color, float size, int type = 0);
	void spawnParticle(const D3DXVECTOR2& pos, const D3DXVECTOR2& direction,
		bool looping, float lifeTime, float velocity, D3DCOLOR color, float size,
		const FastDelegate2<float, float, float>& _func, int type = 0);

	void renderParticles();

	static float _explosionTransform(float v, float dt);

	void spawnExplosion(const D3DXVECTOR2& pos, float lifeTime = 1.5f,
		float distance = 30.0f, D3DXCOLOR color = D3DCOLOR_ARGB(0x80, 0x80, 0, 0), float size = 3.0f,
		int nParticles = 50, int type = 0);

private: // IParticleSystem
	virtual float currentTime() const;

private:
	std::vector<Particle *> instances;
	float curTime;
};

DefineAccessToSingleton(ParticleSystem)
