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

class Tower
{
public:
	Tower()
	{

	}

	void draw()
	{
		//if(type)
		//texture->set();
		//g_Renderer()->drawRect();
	}

	void kill()
	{

	}

	void hide()
	{

	}

private:
	Texture* texture;
	bool isAlive;
	int x, y;
	E_TOWER_TYPE type;
};