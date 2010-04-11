#include "Common.h"

Missile::Missile(IParticleSystem * _pSystem, const D3DXVECTOR2& pos, const D3DXVECTOR2& dir,
		Texture * tex, Player* _player)
		: Particle(_pSystem, pos, dir, false, 20.0f, 100.0f, ~0, 20.0f, ParticleShot, tex, true), Velocity(dir)
{
	player = _player;

	D3DXVec2Normalize(&Velocity, &Velocity);
}

Missile::~Missile()
{
}

D3DXVECTOR2 Missile::applyVelocity(Particle * _self, float dt, float rt) const
{
	D3DXVECTOR2 curVec(position);
	D3DXVECTOR2 result;

	Missile * self = static_cast<Missile *>(const_cast<Particle *>(_self));

	PlayerState* state = player->findState(rt);
	if(state) {
		D3DXVECTOR2 force = state->Aim - self->position / BLOCK_SIZE;
		//D3DXVec2Normalize(&force, &force);
		self->Velocity += force * dt * 5;
	}

	D3DXVec2Normalize(&self->Velocity, &self->Velocity);

	return self->Velocity * dt * BLOCK_SIZE * 20;
}
