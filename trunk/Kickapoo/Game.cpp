#include "Common.h"
#include "Game.h"

static float _introTime = 0.0f;
string _introText = "The time has changed and the world is FUCKED. BULLSHIT!";


Game::Game(void)
: state_(EGameState::Intro)
{
}

Game::~Game(void)
{
}

void Game::changeState(EGameState::TYPE state_)
{
	
}

void Game::startGame()
{
	changeState(EGameState::Tutorial);
}

void Game::create()
{
	//! Create intro
	//! Fade In [0.0f - 1.0f]
	//! Wave it and FadeOut [1.0f - 2.0f]
	//! Fade In Game Screen [2.0f - 3.0f]
	//! Type text [3.0f - 3.0f + textLength * 1.0f]
	AnimationSequenceScalar* introTimeLine = new AnimationSequenceScalar(_introTime, 0.0f, 3 + _introText.size(), 3 + _introText.size());
	AnimationSequenceActivator* startGame = new AnimationSequenceActivator( MakeDelegate(this, &Game::startGame) );
	introTimeLine->setNext(startGame);
	AnimationSequence::add(introTimeLine);


	map = Map::load("mapa.txt");
}

void Game::update()
{
	float dt = g_Timer()->getFrameTime();

	if(state_ == EGameState::Intro)
	{
		

	} else
	{

	}
}

void Game::draw()
{
	map->draw();
}
