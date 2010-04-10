#include "Common.h"
#include "Game.h"

Game::Game(void)
{
}

Game::~Game(void)
{
}

void Game::create()
{
	sprite = CSprite::load("sprite/2010.txt");
	sprite->setPosition(109, 0);
	crysisLogo = new Texture;
	crysisLogo->load("kryzys_logo.jpg");

	map = Map::load("mapa.txt");
}

void Game::update()
{
	sprite->update();
}

void Game::draw()
{
	crysisLogo->set();
	g_Renderer()->drawRect(21, 240, 758, 119);

	static float time = 0.0f;
	if(time < 100)
	{
		time += 0.5f;
		sprite->draw();
	}
}