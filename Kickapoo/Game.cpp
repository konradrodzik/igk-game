#include "Common.h"
#include "Game.h"

static float _introTime = 0.0f;
static float _fake;
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

	//! TODO: implement tutorial
	if(state_ == EGameState::Tutorial)
	{
		state_ = EGameState::Running;
	}
}

void Game::startGame()
{
	changeState(EGameState::Tutorial);
}

void Game::killTower(Tower* tower)
{
	//! kill tower
	AnimationSequenceActivator* kill = new AnimationSequenceActivator(MakeDelegate(tower, &Tower::kill));
	
	//! add particle wait 1 sec and hide tower
	AnimationSequenceActivator1Param* spawnParticle = new AnimationSequenceActivator1Param(MakeDelegate(this, &Game::explodeTower), tower);
	AnimationSequenceScalar* wait1Sec = new AnimationSequenceScalar(_fake, 0, 1, 1.0f);
	AnimationSequenceActivator* hide = new AnimationSequenceActivator(MakeDelegate(tower, &Tower::hide));
	
	//! set seq
	kill->setNext(spawnParticle);
	spawnParticle->setNext(wait1Sec);
	wait1Sec->setNext(hide);

	//! add to system
	AnimationSequence::add(kill);
}

void Game:: explodeTower(void* t)
{
	Tower* tower = (Tower*)t;
	ParticleSystem * ps = ParticleSystem::getSingletonPtr();
	ps->spawnExplosion(D3DXVECTOR2(tower->getX(), tower->getY()));
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
		ParticleSystem * ps = ParticleSystem::getSingletonPtr();
#if 0
		ps->spawnParticle(D3DXVECTOR2(g_Mouse()->getX(), g_Mouse()->getY()),
			D3DXVECTOR2(0, 1), false, 1.0f, 50.0f, D3DCOLOR_ARGB(0x80, 0x80, 0x80, 0), 4.0f);
#endif

		
		g_ParticleSystem()->updateParticles();
		map->update();
	}
		

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
