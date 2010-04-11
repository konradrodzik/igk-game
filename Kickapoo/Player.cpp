#include "Common.h"

Player::Player() : Position(0,0), Velocity(0,0) {

}

PlayerState* Player::findState(float time) {
	for(vector<PlayerState>::reverse_iterator itor = StateList.rbegin(); itor != StateList.rend(); ++itor) {
		if(itor->Time < time)
			return &*itor;
	}
	return NULL;
}

static void resetState(PlayerState& state) {
	state.Handled = false;
}

void Player::reset() {
	std::for_each(StateList.begin(), StateList.end(), resetState);

	LastShootTime = 0;
}

void Player::update(float rt) {
	PlayerState* state = findState(rt);
	if(!state || state->Handled)
		return;

	state->Handled = true;

	if(state->Fire) {
		if(rt - LastShootTime < 1.0f/10.0f)
			return;

		LastShootTime = rt;

		D3DXVECTOR2 direction = state->Aim - state->Position;
		D3DXVec2Normalize(&direction, &direction);

		ParticleSystem::getSingletonPtr()->spawnParticle(
			state->center() * BLOCK_SIZE, direction, false, 10.0f, BLOCK_SIZE * 20, -1, 3, ParticleShot);

	}
}

void Player::record(float dt, float rt, Map* map, bool fire) {
	PlayerState* lastState = findState(rt);
	D3DXVECTOR2 position(0, 0);
	D3DXVECTOR2 force(0, 0);

	PlayerState state;

	if(lastState)
		state.Position = lastState->Position;
	else
		state.Position = Position;

	if(GetKeyState(VK_UP)&0x80 || GetKeyState('W')&0x80)
		force.y += 1;
	if(GetKeyState(VK_DOWN)&0x80 || GetKeyState('S')&0x80)
		force.y -= 1;
	if(GetKeyState(VK_LEFT)&0x80 || GetKeyState('A')&0x80)
		force.x -= 1;
	if(GetKeyState(VK_RIGHT)&0x80 || GetKeyState('D')&0x80)
		force.x += 1;

	Velocity *= exp(-dt) * 0.98f;
	Velocity += 10 * force * dt;

	float length = D3DXVec2Length(&Velocity);
	if(length > 2)
		Velocity /= length / 2;

	D3DXVECTOR2 oldPosition = state.Position;
	state.Position += Velocity * dt * BLOCK_SIZE / 3;

	state.Position = map->slide(oldPosition, state.Position);

	state.Time = rt;
	state.Fire = (GetKeyState(VK_SPACE)&0x80) != 0 || (GetKeyState(VK_SHIFT)&0x80) != 0 ||
		fire;
	state.Aim = D3DXVECTOR2(g_Mouse()->getX() / (float)BLOCK_SIZE, g_Mouse()->getY() / (float)BLOCK_SIZE);

	if(lastState && state.Time - lastState->Time < 1.0f/60.0f) {
		state.Time = lastState->Time;
		*lastState = state;
	}
	else
		StateList.push_back(state);
}

void Player::draw(bool drawStateList, bool drawFromState, float relativeTime) {
	PlayerState* state = NULL;

	if(drawStateList) {
		D3DXVECTOR2 lastPosition = Position;
		for(PlayerStateList::iterator itor = StateList.begin(); itor != StateList.end(); ++itor) {
			g_Renderer()->drawLine(lastPosition.x * BLOCK_SIZE+ BLOCK_SIZE/2, lastPosition.y * BLOCK_SIZE+ BLOCK_SIZE/2, 
				itor->Position.x * BLOCK_SIZE + BLOCK_SIZE/2, itor->Position.y * BLOCK_SIZE+ BLOCK_SIZE/2, 1, D3DCOLOR_XRGB(120, 120, 120));
			lastPosition = itor->Position;
		}

		if(StateList.size()) {
			g_Renderer()->drawRect(lastPosition.x * BLOCK_SIZE+ BLOCK_SIZE/2, lastPosition.y * BLOCK_SIZE+ BLOCK_SIZE/2, 2, 2, D3DCOLOR_XRGB(190, 80, 80));
		}
	}

	if(drawFromState && (state = findState(relativeTime)))	{
		g_Renderer()->drawRect(state->Position.x*BLOCK_SIZE, state->Position.y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
	}
	else {
		g_Renderer()->drawRect(Position.x*BLOCK_SIZE, Position.y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
	}
}
