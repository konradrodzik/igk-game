#pragma once
#include "Common.h"
#include "Particles.h"

class Missile : public Particle
{
public:
	virtual D3DXVECTOR2 applyVelocity(Particle * self, float dt, float rt) const;

	Missile(IParticleSystem * _pSystem, const D3DXVECTOR2& pos, const D3DXVECTOR2& dir,
		Texture * tex, Player* _player);
	virtual ~Missile();

public:
	D3DXVECTOR2 Velocity;

	Player* player;
};
