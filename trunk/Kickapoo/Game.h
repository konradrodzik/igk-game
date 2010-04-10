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

	//! kills tower
	void killTower(Tower* tower);

	//! spawn tower explosion
	void explodeTower(void* tower);

	//! left mouse button clicked
	void onLeftClick();

	void changeState(EGameState::TYPE state);
	void create();
	void update();
	void drawDynamicObjects();
	void draw();

	
protected:
	

	EGameState::TYPE state_;
	Texture kryzys_;
	Texture crysis_;
	Texture gameScreen_;
	Font introFont_;
	Map* map;
	std::vector<Tower> towers;
	std::vector<Player> playerList;
	Player* activePlayer;
	float relativeTime;
};
