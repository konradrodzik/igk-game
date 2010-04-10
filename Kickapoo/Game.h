#pragma once
#include "Common.h"
	
class Game
{
public:
	Game(void);
	~Game(void);

	void create();
	void update();
	void draw();

private:
	CSprite* sprite;
	Texture* crysisLogo;
	Map* map;
};