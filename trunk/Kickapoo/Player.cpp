#include "Common.h"

Player::Player() : Position(0,0), Velocity(0,0), _bullet("gfx/bullet.png"),
	_missile("gfx/missile.png")
	{
		hasMissiles = false;
	}

static bool operator < (const PlayerState& state, const PlayerState& other) {
	return state.Time < other.Time;
}

int compareMyType (const void * a, const void * b)
{
	PlayerState* aa = (PlayerState*)a;
	PlayerState* bb = (PlayerState*)b;

	if(aa->Time < bb->Time)
		return -1;
	if(aa->Time > bb->Time)
		return 1;
	return 0;
}

PlayerState* Player::findState(float time) {
#if 1
	if(StateList.empty())
		return NULL;

	PlayerState dummy;
	dummy.Time = time;

	vector<PlayerState>::iterator itor = std::lower_bound(StateList.begin(), StateList.end(), dummy);
	
	if(itor != StateList.end())
		return &*itor;
	else
		return &StateList.back();
#else
	for(vector<PlayerState>::reverse_iterator itor = StateList.rbegin(); itor != StateList.rend(); ++itor) {
		if(itor->Time < time)
			return &*itor;
	}
#endif
	return NULL;
}

static void resetState(PlayerState& state) {
	state.Handled = false;
}

void Player::reset() {
	std::for_each(StateList.begin(), StateList.end(), resetState);

	LastShootTime = 0;

	dead = false;
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

		
		g_AudioSystem.play(g_fireSound);
		if(hasMissiles)
		{
			ParticleSystem::getSingletonPtr()->addParticle(
				new Missile(ParticleSystem::getSingletonPtr(), state->center() * BLOCK_SIZE, direction, &_missile, this));
		} else {
			ParticleSystem::getSingletonPtr()->spawnParticle(
				state->center() * BLOCK_SIZE, direction, false, 10.0f, BLOCK_SIZE * 20, -1, 20, ParticleShot, &_bullet);
		}
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

	Velocity *= exp(-dt);
	Velocity += 10 * force * dt;

	float length = D3DXVec2Length(&Velocity);
	if(length > 3)
		Velocity /= length / 3;

	D3DXVECTOR2 oldPosition = state.Position;
	state.Position += Velocity * dt * BLOCK_SIZE / 3;

	state.Position = map->slide(oldPosition, state.Position);

	state.Time = rt;
	state.Fire = (GetKeyState(VK_SPACE)&0x80) != 0 || (GetKeyState(VK_SHIFT)&0x80) != 0 ||
		fire;
	state.Aim = D3DXVECTOR2(g_Mouse()->getX() / (float)BLOCK_SIZE, g_Mouse()->getY() / (float)BLOCK_SIZE);

	if(lastState && state.Time - lastState->Time < 1.0f/60.0f) {
		return;
		state.Time = lastState->Time;
		*lastState = state;
	}
	else
		StateList.push_back(state);
}

void Player::draw(bool drawStateList, bool drawFromState, float relativeTime, bool isActive) {
	PlayerState* state = NULL;

	static std::vector<vertex> lines;
	int i = 0;

	if(drawStateList && StateList.size()) {
		lines.resize(StateList.size() * 2);

		D3DXVECTOR2 lastPosition = Position;

		lines[i].pos.x = lastPosition.x* BLOCK_SIZE+ BLOCK_SIZE/2;
		lines[i].pos.y = lastPosition.y* BLOCK_SIZE+ BLOCK_SIZE/2;
		lines[i].pos.z = 0;
		lines[i].color = D3DCOLOR_ARGB(255,255,255,255);
		i++;

		for(PlayerStateList::iterator itor = StateList.begin(); itor != StateList.end(); ++itor) {
			D3DXVECTOR2 diff = (itor->Position - lastPosition) * BLOCK_SIZE;
			if(D3DXVec2Length(&diff) < 6 && &*itor != &StateList.back()) {
				continue;
			}

			lines[i].pos.x = itor->Position.x* BLOCK_SIZE+ BLOCK_SIZE/2;
			lines[i].pos.y = itor->Position.y* BLOCK_SIZE+ BLOCK_SIZE/2;
			lines[i].pos.z = 0;
			lines[i].color = D3DCOLOR_XRGB(128,128,128);
			i++;
	//		g_Renderer()->drawLine(lastPosition.x * BLOCK_SIZE+ BLOCK_SIZE/2, lastPosition.y * BLOCK_SIZE+ BLOCK_SIZE/2, 
	//			itor->Position.x * BLOCK_SIZE + BLOCK_SIZE/2, itor->Position.y * BLOCK_SIZE+ BLOCK_SIZE/2, 1, D3DCOLOR_XRGB(120, 120, 120));
			lastPosition = itor->Position;
	}

		if(i > 1) {
			getDevice()->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
			getDevice()->SetTexture(0, NULL);
			getDevice()->DrawPrimitiveUP(D3DPT_LINESTRIP, i-1, &lines[0], sizeof(vertex));
		}

		if(StateList.size()) {
			isActive ? playerSelectedTexture->set() : playerTexture->set();
			g_Renderer()->drawRect(lastPosition.x * BLOCK_SIZE+ BLOCK_SIZE/2, lastPosition.y * BLOCK_SIZE+ BLOCK_SIZE/2, 2, 2, D3DCOLOR_XRGB(190, 80, 80));
		}
	}

		

	if(drawFromState && (state = findState(relativeTime)))	{
		D3DXVECTOR2 direction = state->Aim - state->Position;
		isActive ? playerSelectedTexture->set() : playerTexture->set();
		g_Renderer()->drawRotatedRect((state->Position.x+0.5f)*BLOCK_SIZE, (state->Position.y+0.5f)*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, direction, D3DCOLOR_ARGB(dead ? 0x50 : 0xff, 0xff, 0xff, 0xff));
	}
	else {
		playerTexture->set();
		g_Renderer()->drawRect((Position.x)*BLOCK_SIZE, (Position.y)*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, D3DCOLOR_ARGB(dead ? 0x50 : 0xff, 0xff, 0xff, 0xff));
	}
}

D3DXVECTOR2 Player::relativePosition(float rt) {
	PlayerState* state = findState(rt);
	if(state)
		return (state->Position + D3DXVECTOR2(0.5f, 0.5f)) * BLOCK_SIZE;
	return D3DXVECTOR2(getX(), getY());
}

