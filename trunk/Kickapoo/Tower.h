#pragma once
#include "Common.h"

enum E_TOWER_TYPE
{
	ETT_STATIC,
	ETT_SHOOTING,
};

enum E_TOWER_STATE
{
	ETS_ALIVE,
	ETS_DYING,
	ETS_HIDE,
};
//! TODO: add reset state
class Tower
{
public:
	Tower(E_TOWER_TYPE type_)
	{
		type = type_;
		state = ETS_ALIVE;
		minPlayerDistance = 200.0f;
		retarded = 0.6f;
		lastShootAt = 0.0f;
		shootTimeDelta = 0.5f;
		shootTarget = D3DXVECTOR2(1.0f, 0.0f);
	}

	Tower()
	{
		state = ETS_ALIVE;
		minPlayerDistance = 200.0f;
		retarded = 0.6;
		lastShootAt = 0.0f;
		shootTimeDelta = 0.5f;
		shootTarget = D3DXVECTOR2(1.0f, 0.0f);
	}

	~Tower()
	{
		
	}

	void setAliveTexture(Texture* tex)
	{
		aliveTexture = tex;
	}

	void setDeathTexture(Texture* tex)
	{
		deathTexture = tex;
	}

	void draw()
	{
		if(state!=ETS_HIDE)
		{
			if(state == ETS_DYING)
				deathTexture->set();
			else if(state == ETS_ALIVE)
				aliveTexture->set();

			if(state == ETS_ALIVE && type == ETT_SHOOTING)
			{
				g_Renderer()->drawRotatedRect(Position.x*BLOCK_SIZE + BLOCK_SIZE/2, Position.y*BLOCK_SIZE +BLOCK_SIZE/2, BLOCK_SIZE, BLOCK_SIZE,
					shootTarget);
			}
			else
			{
				g_Renderer()->drawRect(Position.x*BLOCK_SIZE, Position.y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
			}
		}
	}

	void kill()
	{
		state = ETS_DYING;
	}

	void hide()
	{
		state = ETS_HIDE;
//		g_Game->onTowerKilled();
	}

	int getX()
	{
		return Position.x*BLOCK_SIZE+BLOCK_SIZE/2;
	}

	int getY()
	{
		return Position.y*BLOCK_SIZE+BLOCK_SIZE/2;
	}

	void ai(std::vector<Player> * players, float rt);

public:
	Texture* aliveTexture;
	Texture* deathTexture;
	bool isAlive;
	D3DXVECTOR2 Position;
	//std::vector<Missle*> missiles;
	E_TOWER_TYPE type;
	E_TOWER_STATE state;
	float minPlayerDistance;
	float retarded;
	float lastShootAt;
	float shootTimeDelta;
	D3DXVECTOR2 shootTarget;
};