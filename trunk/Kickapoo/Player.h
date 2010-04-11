#pragma once
#include "Common.h"

struct PlayerState
{
	D3DXVECTOR2 Position;
	D3DXVECTOR2 Aim;
	bool Fire;
	float Time;
	bool Handled;

	PlayerState() {
		memset(this, 0, sizeof(*this));
	}

	D3DXVECTOR2 center() const {
		return Position + D3DXVECTOR2(0.5f, 0.5f);
	}
};

typedef vector<PlayerState> PlayerStateList;

class Player
{
public:
	static const int playerSize = BLOCK_SIZE;
	D3DXVECTOR2 Position;
	D3DXVECTOR2 Velocity;
	float LastShootTime;
	Texture* playerTexture;
	Texture* playerSelectedTexture;
	bool hasMissiles;
	bool dead;

	bool contains(int x, int y)
	{

		if(Position.x*playerSize > x || Position.x*playerSize + playerSize < x ) return false;
		if(Position.y*playerSize > y || Position.y*playerSize + playerSize < y ) return false;
		return true;		
	}

	PlayerStateList StateList;

	PlayerState* findState(float time);

	D3DXVECTOR2 relativePosition(float rt);

	Player();


	int getX() const
	{
		return Position.x*playerSize+playerSize/2;
	}

	int getY() const
	{
		return Position.y*playerSize+playerSize/2;
	}

	void record(float dt, float rt, Map* map, bool fire);

	void update(float rt);

	void draw(bool drawStateList, bool drawFromState, float relativeTime);

	void reset();

private:
	Texture _bullet;
	Texture _missile;
};