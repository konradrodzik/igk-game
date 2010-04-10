#include "Common.h"
#include "Game.h"

static float _introTime = 0.0f;
string _introText = "The time has changed and the world is FUCKED. BULLSHIT!";


Game::Game(void)
:  state_(EGameState::Running)
,  kryzys_("kryzys_logo.jpg")
,  crysis_("crysis.jpg")
,  gameScreen_("game_screen.jpg")
{
}

Game::~Game(void)
{
}

void Game::changeState(EGameState::TYPE state)
{
	state_ = state;
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
	//! Type text [3.0f - 3.0f + textLength * 0.1f]
	float totalTime = 3 + _introText.size() * 0.1f;
	AnimationSequenceScalar* introTimeLine = new AnimationSequenceScalar(_introTime, 0.0f, totalTime, totalTime);
	AnimationSequenceActivator* startGame = new AnimationSequenceActivator( MakeDelegate(this, &Game::startGame) );
	introTimeLine->setNext(startGame);
	AnimationSequence::add(introTimeLine);

	//! intro font
	RECT rect = {160, 400, g_Window()->getWidth(), g_Window()->getHeight()};
	introFont_.create("Verdana", 20, 0, false, &rect);
	introFont_.setTextColor(D3DCOLOR_RGBA(255, 0, 0, 255));


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
		map->update();

	ParticleSystem * ps = ParticleSystem::getSingletonPtr();
	ps->spawnParticle(D3DXVECTOR2(g_Mouse()->getX(), g_Mouse()->getY()),
		D3DXVECTOR2(0, 1), false, 1.0f, 50.0f, D3DCOLOR_ARGB(0x80, 0x80, 0x80, 0), 4.0f);

	g_ParticleSystem()->updateParticles();
}

void Game::draw()
{
	if(state_ == EGameState::Intro)
	{
		if(_introTime < 1.0f)
		{
			getDevice()->SetTexture(0, crysis_.getTexture());
			g_Renderer()->drawRect(0, 0, g_Window()->getWidth(), g_Window()->getHeight(), D3DCOLOR_ARGB((int)(_introTime * 255.0f),255,255,255));
		} else
			//! Wave it and FadeOut [1.0f - 2.0f]
			if(_introTime < 2.0f)
			{
				getDevice()->SetTexture(0, crysis_.getTexture());
				g_Renderer()->drawRect(0, 0, g_Window()->getWidth(), g_Window()->getHeight(), D3DCOLOR_ARGB((int)((2.0f - _introTime) * 255.0f),255,255,255));
				getDevice()->SetTexture(0, kryzys_.getTexture());
				g_Renderer()->drawRect(0, 0, g_Window()->getWidth(), g_Window()->getHeight(), D3DCOLOR_ARGB((int)((_introTime - 1.0f ) * 255.0f),255,255,255));
			} else
				//! Fade In Game Screen [2.0f - 3.0f]
				if(_introTime < 3.0f)
				{
					getDevice()->SetTexture(0, gameScreen_.getTexture());
					g_Renderer()->drawRect(0, 0, g_Window()->getWidth(), g_Window()->getHeight(), D3DCOLOR_ARGB((int)((_introTime - 2.0f) * 255.0f),255,255,255));
	
				} else
				//! Type text [3.0f - 3.0f + textLength * 0.1f]
				{
					getDevice()->SetTexture(0, gameScreen_.getTexture());
					g_Renderer()->drawRect(0, 0, g_Window()->getWidth(), g_Window()->getHeight(), D3DCOLOR_ARGB(255,255,255,255));
					string typedText;
					typedText.assign(_introText.c_str(), (int)((_introTime - 3.0f) * 10.0f));
					introFont_.write(typedText.c_str());
				}

	} else
	{
		map->draw();
		g_ParticleSystem()->renderParticles();
	}
}
