#pragma once
#include "Common.h"
#include "Map.h"
	

namespace EGameState
{
	enum TYPE
	{
		Intro,
		Tutorial,
		Selection,
		Running,
	};
};


class Game
{
public:
	Game(void);
	~Game(void);

	//! start gameplay after intro
	void startGame();
	void changeState(EGameState::TYPE state);
	void create();
	void update();
	void draw();

protected:
	EGameState::TYPE state_;
	Map* map;
};
