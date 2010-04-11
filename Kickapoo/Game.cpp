#include "Common.h"
#include "Game.h"

static float _introTime = 0.0f;
static float _fake;
static float _selectionAlpha = 0.0f;
static float _splashZeroElementY = 0.0f;
static float _splashOneElementY = 600.0f;
Audio g_AudioSystem;
string _introText = "The time has changed and the world is FUCKED. BULLSHIT!";


Game::Game(void)
:  state_(EGameState::Intro)
,  kryzys_("kryzys_logo.jpg")
,  crysis_("crysis.jpg")
,  selection_("gfx/wave.png")
,  gameScreen_("gfx/splash_main.png")
,  zero_("gfx/splash_0.png")
,  one_("gfx/splash_1.png")
, clockTexture("gfx/circle.png")
{
	relativeTime = 0;
	activePlayer = NULL;
	changeState(EGameState::Tutorial);
	map = NULL;

	g_AudioSystem.create();
	clockSound = g_AudioSystem.loadSound("sfx/clock.mp3");
}

Game::~Game(void)
{
	delete introFont_;

	g_AudioSystem.release();
}

void Game::changeState(EGameState::TYPE state)
{
	state_ = state;

	//! TODO: implement tutorial
	if(state == EGameState::Tutorial)
	{
		changeState(EGameState::Selection);
	} else
	if(state == EGameState::Running) {
		relativeTime = 0;
		
		for(int i = 0; i < playerList.size(); ++i)
			playerList[i].reset();

	} else 
		//! TODO: implement selection
		if(state == EGameState::Selection) {
		relativeTime = 0;
		activePlayer = NULL;

		
		// reset all towers state
		for(int i = 0; i < towers.size(); ++i)
		{
			Tower* tower = &towers[i];
			tower->state = ETS_ALIVE;
		}


		// hack off
		// changeState(EGameState::Running);
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
	//Tower* tower = (Tower*)t;
	//ParticleSystem * ps = ParticleSystem::getSingletonPtr();
	//ps->spawnExplosion(D3DXVECTOR2(tower->getX(), tower->getY()));
	//ps->spawnExplosion(D3DXVECTOR2(tower->getX(), tower->getY()));
}


void Game::create()
{
	//! Create intro
	//! Fade In [0.0f - 1.0f]
	//! Wave it and FadeOut [1.0f - 2.0f]
	//! Fade In Game Screen [2.0f - 3.0f]
	//! Type text [3.0f - 3.0f + textLength * 0.1f]
	if(state_ == EGameState::Intro) {
		float totalTime = 3 + _introText.size() * 0.1f;
		AnimationSequenceScalar* introTimeLine = new AnimationSequenceScalar(_introTime, 0.0f, totalTime, totalTime);
		AnimationSequenceActivator* startGame = new AnimationSequenceActivator( MakeDelegate(this, &Game::startGame) );
		introTimeLine->setNext(startGame);
		AnimationSequence::add(introTimeLine);
	}

	//! intro font
	RECT rect = {160, 400, g_Window()->getWidth(), g_Window()->getHeight()};
	introFont_ = new Font();
	introFont_->create("Verdana", 20, 0, false, &rect);
	introFont_->setTextColor(D3DCOLOR_RGBA(255, 0, 0, 255));


	map = Map::load("mapa.bmp");
	map->loadContent(playerList, towers);

//	if(playerList.size())
//		activePlayer = &playerList[0];
}

void Game::update()
{
	float dt = g_Timer()->getFrameTime();

	if(state_ != EGameState::Intro) {
		map->update();
	}

	if(state_ == EGameState::Intro)
	{
	} 
	else if(state_ == EGameState::Running)
	{
		g_ParticleSystem()->updateParticles(map);

		for(int i = 0; i < towers.size(); ++i)
		{
			Tower* tower = &towers[i];
			if(tower->state != ETS_ALIVE)
				continue;
			if(g_ParticleSystem()->particlesFoundByRect(tower->getX() - BLOCK_SIZE / 2, tower->getY() - BLOCK_SIZE / 2, BLOCK_SIZE, BLOCK_SIZE, ParticleShot))
				killTower(tower);
		}

		ParticleSystem * ps = ParticleSystem::getSingletonPtr();

		relativeTime += dt;

		if(relativeTime > 10.0f) {
			changeState(EGameState::Selection);
		}
		else if(activePlayer) {
			activePlayer->record(dt, relativeTime, map, leftMouseDown);
		}
		


		if(state_ == EGameState::Running) {
			for(int i = 0; i < playerList.size(); ++i)
				playerList[i].update(relativeTime);

			for(int i = 0 ; i < towers.size() ; ++i)
				towers[i].ai(&playerList);

			updateClock();
		}
		else
		{
			g_AudioSystem.stopSoud(clockSound);
		}
	}

	leftMouseClick = false;

	g_AudioSystem.update();
}
	
void Game::drawDynamicObjects()
{
	for(unsigned i = 0; i < playerList.size(); ++i) {
		playerList[i].draw(true, state_ == EGameState::Running, relativeTime);
	}

	for(unsigned i = 0; i < towers.size(); ++i)
	{
		Tower& tower = towers[i];
		tower.draw();
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
				//! and change 2010 into 2110
				{
					getDevice()->SetTexture(0, gameScreen_.getTexture());
					g_Renderer()->drawRect(0, 0, g_Window()->getWidth(), g_Window()->getHeight(), D3DCOLOR_ARGB(255,255,255,255));
					string typedText;
					typedText.assign(_introText.c_str(), (int)((_introTime - 3.0f) * 10.0f));
					introFont_->write(typedText.c_str());

			//		getDevice()->SetTexture(0, zero_.getTexture());
			//		g_Renderer()->drawRect(0, _splashZeroElementY, g_Window()->getWidth(), g_Window()->getHeight(), D3DCOLOR_ARGB(255,255,255,255));
			//		getDevice()->SetTexture(0, one_.getTexture());
			//		g_Renderer()->drawRect(0, _splashOneElementY, g_Window()->getWidth(), g_Window()->getHeight(), D3DCOLOR_ARGB(255,255,255,255));
				}

	} else
	{
		map->draw();
		drawDynamicObjects();
		g_ParticleSystem()->renderParticles();
		drawClock();
		//! draw selection
		if(_selectionAlpha > 0.0f)
		{

			float ssize = 32 * (- _selectionAlpha + 3);

			getDevice()->SetTexture(0, selection_.getTexture());
			g_Renderer()->drawRect( activePlayer->getX()- ssize * 0.5f, activePlayer->getY() - ssize * 0.5f, ssize, ssize, D3DCOLOR_ARGB((int)(_selectionAlpha*255), 255, 0,0));

		}

	}
}


void Game::onLeftClick()
{
	leftMouseClick = true;

	if(state_ ==  EGameState::Selection)
	{

		for(int i=0; i < (int) playerList.size(); ++i)
		{

			if(playerList[i].contains(g_Mouse()->getX(), g_Mouse()->getY()))
			{

				activePlayer = &playerList[i];
				activePlayer->StateList.clear();
				activePlayer->Velocity = D3DXVECTOR2(0, 0);
				
				//! draw selection 
				AnimationSequenceScalar* sel = new AnimationSequenceScalar(_selectionAlpha, 1.0f, 0.0f, 1.0f);
				AnimationSequence::add(sel);

				changeState(EGameState::Running);
				return;
			}
		}
	}
}

void Game::drawClock()
{
	clockTexture.set();
	g_Renderer()->drawRect(620, 420, 128, 128);
	for(int i = 0; i < clockLines.size(); ++i)
	{
		SLine line = clockLines[i];
		g_Renderer()->drawLine(line.x1, line.y1, line.x2, line.y2, 1, D3DCOLOR_RGBA(0, 0, 255, 128));
	}
}

void Game::updateClock()
{
	static float circleAngle = 0;
	static bool isLooping = false;
	float step = 10.0f/360.0f*10; 
	SLine line;
	line.x1 = 684.0f;
	line.y1 = 484.0f;
	line.x2 = line.x1+sin(circleAngle*DEG2RAD)*66;
	line.y2 = line.y1+cos(circleAngle*DEG2RAD)*66;
	circleAngle += step;

	if((int)circleAngle >= clockLines.size())
		clockLines.push_back(line);
	
	if(relativeTime > 10.0f)
	{
		circleAngle = 0;
		clockLines.clear();
	}
	
	if(!isLooping)
	{
		g_AudioSystem.play(clockSound);
		isLooping = true;
	}
	
}
