#include "Common.h"
#include "Tower.h"

void Tower::ai(std::vector<Player> * players, float rt)
{
	if(type != ETT_SHOOTING)
		return;

	D3DXVECTOR2 myPos(getX(), getY());
	D3DXVECTOR2 dvec;

	float curTime = g_Timer()->getEngineTime();

	if((curTime - lastShootAt) <  shootTimeDelta)
		return;


	for(int i = 0 ; i < players->size() ; ++i)
	{
		D3DXVECTOR2 player(players->at(i).relativePosition(rt));

		D3DXVec2Subtract(&dvec, &player, &myPos);
		float distanceFromPlayer = D3DXVec2Length(&dvec);

		if(distanceFromPlayer < minPlayerDistance)
		{
			D3DXVECTOR2 shootDir;
			D3DXVec2Normalize(&shootDir, &dvec);
			shootTarget = shootDir;
			float willShoot = RandomFloat(0.0f, 1.2f);
			if(willShoot > retarded)
			{				
				g_ParticleSystem()->spawnParticle(myPos, shootDir * 4, false, 5.0f, 60.0f * (1.0f - retarded),
					D3DCOLOR_ARGB(255, 0x80, 0, 0x20), 4.0f, ParticleHarmful);
	lastShootAt = curTime;
			}
			return;
		}
	}
}
