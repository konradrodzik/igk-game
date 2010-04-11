#pragma once
#include "Common.h"
#include "Map.h"

#define DEG2RAD 0.0174532925

extern Audio g_AudioSystem;

namespace EGameState
{
	enum TYPE
	{
		Intro,
		Tutorial,
		Selection,
		Running,
		LevelFinished,
		GameFinished
	};
};

struct SLine
{
	float x1,y1,x2,y2;
};


extern Sound* g_fireSound;
extern Sound* g_wallSound;


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

	void onLeftDown() { leftMouseDown = true; }
	void onLeftUp() { leftMouseDown = false; }

	void changeState(EGameState::TYPE state);
	void create();
	void update();
	void drawDynamicObjects();
	void draw();

	void updateClock();

	//! loads level
	void loadLevel();
	void onTowerKilled();	
	void onPlayerKilled(Player* player);
protected:
	static const int maxLevels_ = 6;

	EGameState::TYPE state_;
	Texture kryzys_;
	Texture crysis_;
	Texture gameScreen_;
	Texture zero_;
	Texture one_;
	Texture selection_;
	Font* introFont_;
	Map* map;
	std::vector<Tower> towers;
	std::vector<Player> playerList;
	Player* activePlayer;
	float relativeTime;
	bool leftMouseClick, leftMouseDown;
	Texture clockTexture;

	int towersAlive_;
	int replayCount_;
	int level_;

	Sound* clockSound;
	Font* clockFont;
	Texture scoreLay;
	Font* scoreFont;

	Sound* explosionSound;
	Sound* pickSound;
	Sound* background;
	bool backgroundMusicStarted;

	Sound* typingSound;
};
