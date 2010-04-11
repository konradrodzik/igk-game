#include "Common.h"
#include "Tower.h"

void Tower::ai(std::vector<Player> * players, float rt)
{
	if(type != ETT_SHOOTING)
		return;

	if(state != ETS_ALIVE)
		return;

	D3DXVECTOR2 myPos(getX(), getY());
	D3DXVECTOR2 dvec;

	float curTime = g_Timer()->getEngineTime();

	if((curTime - lastShootAt) <  shootTimeDelta)
		return;

	Player* best = NULL;
	float minDist = minPlayerDistance;
	D3DXVECTOR2 dirbest;

	for(int i = 0 ; i < players->size() ; ++i)
	{
		D3DXVECTOR2 player(players->at(i).relativePosition(rt));

		if(players->at(i).dead)
			continue;

		D3DXVec2Subtract(&dvec, &player, &myPos);
		float distanceFromPlayer = D3DXVec2Length(&dvec);

		if(distanceFromPlayer < minDist)
		{
			minDist = distanceFromPlayer;
			best = &players->at(i);
			dirbest = dvec;
		}
	}

	if(best) {
		D3DXVECTOR2 shootDir;
		D3DXVec2Normalize(&shootDir, &dirbest);
		shootTarget = shootDir;
		float willShoot = RandomFloat(0.0f, 1.2f);
		if(willShoot > retarded)
		{				
			g_ParticleSystem()->spawnParticle(myPos, shootDir * 7, false, 5.0f, 6 * BLOCK_SIZE * (1.0f - retarded),
				D3DCOLOR_ARGB(255, 0x0, 0x80, 0xff), 7.0f, ParticleHarmful);
			lastShootAt = curTime;
		}
	}
}
