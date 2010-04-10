#include "Common.h"
#include "Level.h"

int playerLifes = 3;

extern float playerForceCoeff;
extern float playerJumpCoeff;

void onPowerUpCollision(Body* target, Body* otherBody, EContactType type, float friction, float relVelx, float relVely, void* deleteMe)
{
	if(!target || !otherBody)
		return;

	PowerUp * p1 = PowerUp::Cast(target);
	PowerUp * p2 = PowerUp::Cast(otherBody);

	if(type == CT_ADD)
	{
		if(otherBody->label == "kolce")
		{
			--playerLifes;
			//player.respawnPlayer();
		}
	}
}

void onPlayerCollision(Body* target, Body* otherBody, EContactType type, float friction, float relVelx, float relVely,void*) {
	//	if(friction > 0.9f)
	if(type == CT_RESULTS) {
		float normalImp = friction;

		//! collision with ground!
		if(normalImp > 100.0f)
			if(!otherBody) g_Audio()->play(playerHitGround, 0.7f);

	}
}

bool once = true;
void kolce(Body* target, Body* otherBody, EContactType type, float friction, float relVelx, float relVely, void*) {
	if(!target || !otherBody)
		return;


	if(type == CT_ADD)
	{
		if(otherBody->label == "kulka")
		{
			g_Game->levelState = EGameover;
			pause = true;

		}
		else if(otherBody->label == "kulka")
		{
			g_Game->levelState = EGameover;
			pause = true;
		}
	}
}

void wyjscie(Body* target, Body* otherBody, EContactType type, float friction, float relVelx, float relVely, void*) {
	if(!target || !otherBody)
		return;


	if(type == CT_ADD)
	{
		if(otherBody->label == "kulka")
		{
			g_Game->levelState = EWin;
			pause = true;
		}
		else if(otherBody->label == "kulka")
		{
			g_Game->levelState = EWin;
			pause = true;
		}
	}
}

Level::Level(void)
{
	m_TimeCoeff = 1.0f;
	m_TimeCoeffTimeout = 0;
	playerJumpCoeff = 1.0f;
}

Level::~Level(void)
{
	for(std::list<ParcitleEmitter *>::iterator i = m_Particles.begin() ; i != m_Particles.end() ; ++i)
	{
		delete *i;
	}
}

void Level::loadCollision(const char* filePath)
{
	//g_World()->refresh();
	lines.clear();
	points.clear();
	g_World()->refresh();

	FILE* f = fopen(filePath, "r");
	if(f == NULL)
		return;

	char buff[2048];
	ZeroMemory(buff, 2048);
	fseek(f, 0, SEEK_END);
	int length = ftell(f);
	fseek(f, 0, SEEK_SET);
	fread(buff, 1, length, f);
	rewind(f);

	D3DXVECTOR2 point;

	char* pointer = buff;

	while(!feof(f))
	{
		if(*f->_ptr == '\n')
		{
			f->_ptr++;
			if(*f->_ptr == '\n')
			{
				lines.push_back(points);
				points.clear();
				f->_ptr++;
				continue;
				//f->_ptr++;
			}
			continue;
		}

		float x, y;
		if(fscanf(f, "%f %f", &x, &y) != 2)
			break;

		point.x = x;
		point.y = y;
		points.push_back(point);
	}

	fclose(f);	

	for(int i= 0; i < lines.size(); ++i)
	{
		std::vector<D3DXVECTOR2> poin;
		poin = lines[i];
		bool success = g_World()->addShape(poin);
		if(!success) {
			points.clear();
			return;
		}
	}
}

void Level::release()
{
	for(int i = 0; i < bodies.size(); i++) {
		//g_World()->getWorld()->DestroyBody(bodies[i]->getBody());
		delete bodies[i];
	}
	bodies.clear();

	contacts->clear();
}

void Level::draw()
{
	background.set();
	g_Renderer()->drawRect(-40, -30, 80, 60, 0, 0, 1, 768.0/1024);

	player->draw();

	for(int i=0; i < m_joints.size();++i)
		m_joints[i]->draw();

	for(int i=0; i < bodies.size(); ++i) 
		bodies[i]->draw();

	bool safety=false;
	for(int i=0; i < bodies.size(); ) 
	{
		if(bodies[i]->shouldDie) {
			safety = true;
			bodies.erase(bodies.begin() + i);
		}
		else
			++i;
	}

	if(safety)
		contacts->clear();

	if(g_Timer()->getEngineTime() > m_TimeCoeffTimeout)
	{
		m_TimeCoeff = 1.0f;
	}

	std::list<ParcitleEmitter *>::iterator next_it;
	for(std::list<ParcitleEmitter *>::iterator it = m_Particles.begin() ; it != m_Particles.end() ; it = next_it)
	{
		next_it = it;
		++next_it;
		(*it)->Update(GetLevelTime());
		(*it)->Render(b2Vec2(0,0), GetLevelTime());
		if((*it)->Kill())
		{
			delete (*it);
			m_Particles.erase(it);
		}
	}
}

void Level::SpawnPowerup(float friction, float jump, float x, float y, float r, const char * tex)
{
	PowerUp * pup = new PowerUp(x,y,r,1,0.5f,0.5f,tex);
	pup->frictionCoeff = friction;
	pup->jumpCoeff = jump;
	pup->setOnContact(CollisionDelegate(onPowerUpCollision));
	bodies.push_back(pup);
}

float Level::GetLevelTime() const
{
	if(g_Timer()->getEngineTime() > m_TimeCoeffTimeout)
		return g_Timer()->getEngineTime();
	return m_TimeCoeff * g_Timer()->getEngineTime();
}

float Level::GetLevelFrameTime() const
{
	if(g_Timer()->getEngineTime() > m_TimeCoeffTimeout)
		return g_Timer()->getFrameTime();
	return m_TimeCoeff * g_Timer()->getFrameTime();
}

void Level::SpawnParticles(int count, float x, float y, float xCoeff, float yCoeff )
{
	ParcitleEmitter * emiter = new ParcitleEmitter();
	emiter->Emit(b2Vec2(x,y), GetLevelTime(), count, xCoeff, yCoeff);
	m_Particles.push_back(emiter);
}


void Level1::create()
{
	loadCollision("lvl1_col.txt");

	background = "lvl1.png";
	player = new Sphere(0, 5, 1.5f, 6, 0.9f);
	player->setOnContact(CollisionDelegate(onPlayerCollision));
	player->label = "kulka";

	for(int i=0; i < 4; ++i) {
		bodies.push_back(new Sphere(RandomFloat(0, 30), RandomFloat(0, 10), 2.4, 1.0f, 0.5f, 0.5f, "ball3.png"));
	}
 	g_Game->level->bodies.push_back(new Box(-17.5, -22, 13, 4, -5, 0.5, "kolce.png"));
 	g_Game->level->bodies[g_Game->level->bodies.size()-1]->setOnContact(kolce);
  	g_Game->level->bodies.push_back(new Box(6, -25, 14, 4, -5, 0.5, "kolce.png"));
  	g_Game->level->bodies[g_Game->level->bodies.size()-1]->setOnContact(kolce);
 	g_Game->level->bodies.push_back(new Box(12.5, -8, 14, 1, -5, 0.5, "kolce.png"));
 	g_Game->level->bodies[g_Game->level->bodies.size()-1]->setOnContact(kolce);

	g_Game->level->bodies.push_back(new Box(-38.5, -12, 3, 8, -5, 0.5, "kolce.png"));
	g_Game->level->bodies[g_Game->level->bodies.size()-1]->setOnContact(wyjscie);

// 	SpawnPowerup(2.0f,1.0f,30,10,2.0f,"ball3.png");
// 	SpawnParticles(1000, 15, 15);
}

void Level2::create()
{
	loadCollision("lvl2_col.txt");

	background = "lvl2.png";
	player = new Sphere(-30, 23, 1.5f, 6, 0.9f);
	player->setOnContact(CollisionDelegate(onPlayerCollision));
	player->label = "kulka";

	g_Game->level->bodies.push_back(new Box(-25, 17, 30, 2, -5, 0.5, "kolce.png"));
	g_Game->level->bodies[g_Game->level->bodies.size()-1]->setOnContact(kolce);
	g_Game->level->bodies.push_back(new Box(0, -25, 80, 4, -5, 0.5, "kolce.png"));
	g_Game->level->bodies[g_Game->level->bodies.size()-1]->setOnContact(kolce);


	bodies.push_back(new Box(0, -10, 9, 2, 5.0f, 0.8f));
	m_joints.push_back(new Joint);
	m_joints[m_joints.size()-1]->makeDistanceJoint2(bodies[bodies.size()-1]->getBody(), g_World()->m_shapes[0], b2Vec2(-4, -10), b2Vec2(0, -4), true);
	m_joints[m_joints.size()-1]->setBreakPoint(5000.0f);
	m_joints.push_back(new Joint);
	m_joints[m_joints.size()-1]->makeDistanceJoint2(bodies[bodies.size()-1]->getBody(), g_World()->m_shapes[0], b2Vec2(4, -10), b2Vec2(4, -4), true);

	pause = true;
//	for(int i=0; i < 10; ++i) {
//		bodies.push_back(new Sphere(RandomFloat(-30, 30), rand()%20, RandomFloat(1, 5), 1.0f, 0.5f, 0.5f, "ball3.png"));
//	}
}

void Level3::create()
{
	//loadCollision("lvl3_col.txt");

	background = "lvl3.png";
	player = new Sphere(0, 5, 1.5f, 6, 0.9f);
	player->setOnContact(CollisionDelegate(onPlayerCollision));
	player->label = "kulka";

	g_Game->level->bodies.push_back(new Box(-10, 16.5, 75, 1, -5, 0.5, "kolce.png"));
	g_Game->level->bodies[g_Game->level->bodies.size()-1]->setOnContact(kolce);
	g_Game->level->bodies.push_back(new Box(0, 23, 80, 1, -5, 0.5, "kolce.png"));
	g_Game->level->bodies[g_Game->level->bodies.size()-1]->setOnContact(kolce);

	g_Game->level->bodies.push_back(new Box(-10, 7.5, 75, 1, -5, 0.5, "kolce.png"));
	g_Game->level->bodies[g_Game->level->bodies.size()-1]->setOnContact(kolce);

	g_Game->level->bodies.push_back(new Box(11, -0.5, 75, 1, -5, 0.5, "kolce.png"));
	g_Game->level->bodies[g_Game->level->bodies.size()-1]->setOnContact(kolce);

	g_Game->level->bodies.push_back(new Box(0, -22.5, 90, 1, -5, 0.5, "kolce.png"));
	g_Game->level->bodies[g_Game->level->bodies.size()-1]->setOnContact(kolce);

	g_Game->level->bodies.push_back(new Box(28, -14.5, 40, 1, -5, 0.5, "kolce.png"));
	g_Game->level->bodies[g_Game->level->bodies.size()-1]->setOnContact(kolce);

	g_Game->level->bodies.push_back(new Box(-20, -14.5, 15, 1, -5, 0.5, "kolce.png"));
	g_Game->level->bodies[g_Game->level->bodies.size()-1]->setOnContact(kolce);

	g_Game->level->bodies.push_back(new Box(-41, -10.5, 5, 37, -5, 0.5, "kolce.png"));
	g_Game->level->bodies[g_Game->level->bodies.size()-1]->setOnContact(kolce);

	g_Game->level->bodies.push_back(new Box(-25.5, -7.5, 3, 14, -5, 0.5, "kolce.png"));
	g_Game->level->bodies[g_Game->level->bodies.size()-1]->setOnContact(kolce);

	g_Game->level->bodies.push_back(new Box(37, 10.5, 3, 25, -5, 0.5, "kolce.png"));
	g_Game->level->bodies[g_Game->level->bodies.size()-1]->setOnContact(kolce);

	g_Game->level->bodies.push_back(new Box(37, 10.5, 3, 25, -5, 0.5, "kolce.png"));
	g_Game->level->bodies[g_Game->level->bodies.size()-1]->setOnContact(kolce);


	this->m_TimeCoeffTimeout = g_Timer()->getEngineTime() + 100000.0f;
	this->m_TimeCoeff = 0.2f;
	playerJumpCoeff = 0.5f;

	pause = true;
}


