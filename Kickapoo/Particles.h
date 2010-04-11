#pragma once
#include "Common.h"

class Particle;

class IParticleSystem
{
public:
	virtual float currentTime() const = 0;
};

enum
{

	ParticleShot = 1,
	ParticleHarmful
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
	Texture * texture;
	bool needRotation;

	Particle(IParticleSystem * _pSystem, const D3DXVECTOR2& pos, const D3DXVECTOR2& dir,
		bool loop, float lifeTime, float velocity, D3DCOLOR color, float size, int type,
		Texture * tex, bool needsRot);
	virtual ~Particle();
	bool updateState(Map *, float rt);

	const D3DXVECTOR2& pos() const { return position; }

	virtual D3DXVECTOR2 applyVelocity(Particle * self, float dt, float rt) const {
		return self->dirVec * self->velocity * dt;
	}
protected:
	IParticleSystem * particleSystem;
	float respawnTime;
	D3DXVECTOR2 position;
};

class Particle2 : public Particle
{
	FastDelegate2<Particle *, float, D3DXVECTOR2> _velocityTransform;
public:
	virtual D3DXVECTOR2 applyVelocity(Particle * self, float dt, float rt) const
		{
		return _velocityTransform(self, dt);
		}

	Particle2(IParticleSystem * _pSystem, const D3DXVECTOR2& pos, const D3DXVECTOR2& dir,
		bool loop, float lifeTime, float velocity, D3DCOLOR color, float size, int type,
		Texture * tex, bool needsRot,
		const FastDelegate2<Particle *, float, D3DXVECTOR2>& _func);
	virtual ~Particle2();
};

class ParticleSystem : public IParticleSystem, public Singleton<ParticleSystem>
{
public:
	ParticleSystem();
	virtual ~ParticleSystem();

	void updateParticles(Map * map, float rt);
	void spawnParticle(const D3DXVECTOR2& pos, const D3DXVECTOR2& direction,
		bool looping, float lifeTime, float velocity, D3DCOLOR color, float size, int type = 0,
		Texture * tex = NULL, bool needsRot = true, bool queue = false);

	void spawnParticle(const FastDelegate2<Particle *, float, D3DXVECTOR2>& _func, 
		const D3DXVECTOR2& pos, const D3DXVECTOR2& direction,
		bool looping, float lifeTime, float velocity, D3DCOLOR color, float size, int type = 0,
		Texture * tex = NULL, bool needsRot = true, bool queue = false);

	void renderParticles();

	static float _explosionTransform(float v, float dt);

	void spawnExplosion(const D3DXVECTOR2& pos, float lifeTime = 1.5f,
		float distance = 30.0f, D3DXCOLOR color = D3DCOLOR_ARGB(0x80, 0x80, 0, 0), float size = 3.0f,
		int nParticles = 10, int type = 0, bool queue = false);

	bool particlesFoundByRect(float x, float y, float w, float h, int byType) const;
	void clipParticles(float x, float y, float w, float h, int byType);
	void clear();

	void addParticle(Particle * particle, bool queue = false);

	void checkParticlesAgainstMap(Map& map, int byType, Game& game);
	void checkParticlesAgainstPlayer(const std::vector<Player> * players, int byType, FastDelegate1<Player*> killPlayer);

private: // IParticleSystem
	virtual float currentTime() const;

private:
	std::vector<Particle *> instances;
	std::vector<Particle *> queue;
	float curTime;
};

DefineAccessToSingleton(ParticleSystem)
