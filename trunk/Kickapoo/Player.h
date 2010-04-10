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
	static const int playerSize = BLOCK_SIZE;
	D3DXVECTOR2 Position;
	D3DXVECTOR2 Velocity;

	bool contains(int x, int y)
	{

		if(Position.x*playerSize - playerSize > x || Position.x*playerSize + playerSize < x ) return false;
		if(Position.y*playerSize - playerSize > y || Position.y*playerSize + playerSize < y ) return false;
		return true;		
	}

	vector<PlayerState> StateList;

	PlayerState* findState(float time);

	Player();

private:
};