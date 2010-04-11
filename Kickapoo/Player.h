#pragma once
#include "Common.h"

struct PlayerState
{
	D3DXVECTOR2 Position;
	D3DXVECTOR2 Direction;
	bool Fire;
	float Time;
};

typedef vector<PlayerState> PlayerStateList;

class Player
{
public:
	static const int playerSize = BLOCK_SIZE;
	D3DXVECTOR2 Position;
	D3DXVECTOR2 Velocity;

	bool contains(int x, int y)
	{

		if(Position.x*playerSize > x || Position.x*playerSize + playerSize < x ) return false;
		if(Position.y*playerSize > y || Position.y*playerSize + playerSize < y ) return false;
		return true;		
	}

	PlayerStateList StateList;

	PlayerState* findState(float time);

	Player();


	int getX()
	{
		return Position.x*playerSize+playerSize/2;
	}

	int getY()
	{
		return Position.y*playerSize+playerSize/2;
	}

	void update(float dt);

	void draw(bool drawStateList, bool drawFromState, float relativeTime);

private:
};