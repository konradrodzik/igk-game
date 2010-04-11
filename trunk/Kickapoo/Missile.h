#pragma once
#include "Common.h"
#include "Particles.h"

class Missile : public Particle
{
public:
	virtual D3DXVECTOR2 applyVelocity(Particle * self, float dt) const;

	Missile(IParticleSystem * _pSystem, const D3DXVECTOR2& pos, const D3DXVECTOR2& dir,
		Texture * tex);
	virtual ~Missile();

public:
	D3DXVECTOR2 mousePoints[3];
};
