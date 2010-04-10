#pragma once
#include "Common.h"

extern int playerLifes;

extern Sound* playerHitGround;
extern void LoadCollisions(const std::string& fileName);

class Level
{
public:
	std::vector<Body*> bodies;	
	Texture background;

public:
	Sphere* player;
	float m_TimeCoeff;
	float m_TimeCoeffTimeout;
	std::list<ParcitleEmitter *> m_Particles;
	std::vector<Joint*> m_joints;

public:
	Level(void);
	~Level(void);

	float GetLevelTime() const;
	float GetLevelFrameTime() const;

	virtual void create() {}
	void release();

	void loadCollision(const char* filePath);
	void draw();
	void SpawnPowerup(float friction, float jump, float x, float y, float r = 2.0f, const char * tex = "ball3.png");

	void SpawnParticles(int count, float x, float r, float xCoeff = 1.0f, float yCoeff = 1.0f);
};

class Level1 : public Level
{
	void create();
};

class Level2 : public Level
{
	void create();
};

class Level3 : public Level
{
	void create();
};
