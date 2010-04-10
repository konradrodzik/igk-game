#pragma once
#include "Common.h"

struct PlayerState
{
	D3DXVECTOR2 Position;
	D3DXVECTOR2 Direction;
	bool Fire;
	float Time;
};

class Player
{
public:
	D3DXVECTOR2 Position;
	D3DXVECTOR2 Velocity;

	vector<PlayerState> StateList;

	PlayerState* findState(float time);

	Player();

private:
};