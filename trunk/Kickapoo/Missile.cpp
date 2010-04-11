#include "Common.h"

Missile::Missile(IParticleSystem * _pSystem, const D3DXVECTOR2& pos, const D3DXVECTOR2& dir,
		Texture * tex)
		: Particle(_pSystem, pos, dir, false, 20.0f, 100.0f, ~0, 20.0f, ParticleShot, tex, true)
{
	mousePoints[0] = D3DXVECTOR2(g_Mouse()->getX(), g_Mouse()->getY());
	mousePoints[1] = mousePoints[2] = mousePoints[0];
}

Missile::~Missile()
{
}

D3DXVECTOR2 Missile::applyVelocity(Particle * _self, float dt) const
{
	D3DXVECTOR2 curVec(position);
	D3DXVECTOR2 result;

	Missile * self = static_cast<Missile *>(const_cast<Particle *>(_self));

	D3DXVec2CatmullRom(&result, &curVec, &mousePoints[0], &mousePoints[1], &mousePoints[2], dt * self->velocity);

	self->mousePoints[0] = self->mousePoints[1];
	self->mousePoints[1] = self->mousePoints[2];
	self->mousePoints[2] = D3DXVECTOR2(g_Mouse()->getX(), g_Mouse()->getY());

	D3DXVec2Normalize(&self->dirVec, &result);

	return self->dirVec * self->velocity * dt;
}
