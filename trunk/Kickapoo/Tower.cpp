#include "Common.h"
#include "Tower.h"

void Tower::ai(std::vector<Player> * players)
{
	D3DXVECTOR2 myPos(getX(), getY());
	D3DXVECTOR2 dvec;

	float curTime = g_Timer()->getEngineTime();

	if((curTime - lastShootAt) <  shootTimeDelta)
		return;

	lastShootAt = curTime;

	for(int i = 0 ; i < players->size() ; ++i)
	{
		D3DXVECTOR2 player(players->at(i).getX(), players->at(i).getY());

		D3DXVec2Subtract(&dvec, &player, &myPos);
		float distanceFromPlayer = D3DXVec2Length(&dvec);

		if(distanceFromPlayer < minPlayerDistance)
		{
			float willShoot = RandomFloat(0.0f, 1.2f);
			if(willShoot > retarded)
			{
				D3DXVECTOR2 shootDir;
				D3DXVec2Normalize(&shootDir, &dvec);
				
				g_ParticleSystem()->spawnParticle(myPos, shootDir, false, 5.0f, 30.0f * (1.0f - retarded),
					D3DCOLOR_ARGB(255, 0x80, 0, 0x20), 4.0f, ParticleHarmful);
			}
		}
	}
}
