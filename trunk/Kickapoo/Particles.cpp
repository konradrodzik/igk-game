#include "Common.h"
#include "Particles.h"
#include <math.h>

Particle::Particle(IParticleSystem * _pSystem, const D3DXVECTOR2& pos, const D3DXVECTOR2& dir,
	 bool loop, float lifeTime, float velocity, D3DCOLOR pcolor, float size, int type,
	 Texture * tex, bool needsRot)
	: start(pos),
	  dirVec(dir),
	  looping(loop),
	  lifeTime(lifeTime),
	  velocity(velocity),
	  color(pcolor),
	  size(size),
	  type(type),
	  particleSystem(_pSystem),
	  texture(tex)
{
	respawnTime = particleSystem->currentTime();
	position = start;
	needRotation = needsRot;
}

Particle::~Particle()
{
}

bool Particle::updateState(Map * map)
{
	float tt = particleSystem->currentTime();

	if((type == ParticleShot || type == ParticleHarmful) && map)
	{
		float px = (position.x) / BLOCK_SIZE;
		float py = (position.y) / BLOCK_SIZE;
		if(map->blocked(px, py))
		{
		return false;
		}
	}

	if(tt > (respawnTime + lifeTime))
	{
		if(!looping)
			return false;
		respawnTime = tt;
		position = start;
		return true;
	}

	position += applyVelocity(this, g_Timer()->getFrameTime());
	return true;
}

Particle2::Particle2(IParticleSystem * _pSystem, const D3DXVECTOR2& pos, const D3DXVECTOR2& dir,
		  bool loop, float lifeTime, float velocity, D3DCOLOR color, float size, int type, Texture * tex, bool needsRot,
		  const FastDelegate2<Particle *, float, D3DXVECTOR2>& _func)
		  : Particle(_pSystem, pos, dir, loop, lifeTime, velocity, color, size, type, tex, needsRot),
		  _velocityTransform(_func)
{
}

Particle2::~Particle2()
{
}

ParticleSystem::ParticleSystem()
{
	instances.reserve(10000);
}

ParticleSystem::~ParticleSystem()
{
	for(std::vector<Particle *>::iterator it = instances.begin() ; it != instances.end() ; ++it)
		delete *it;
}

void ParticleSystem::updateParticles(Map * map)
{
	curTime = g_Timer()->getEngineTime();
	for(std::vector<Particle *>::iterator it = instances.begin() ; it != instances.end() ; )
	{
		if((*it)->updateState(map))
		{
			++it;
		} else {
			delete *it;
			it = instances.erase(it);
		}
	}
}

void ParticleSystem::spawnParticle(const D3DXVECTOR2& pos, const D3DXVECTOR2& direction,
	bool looping, float lifeTime, float velocity, D3DCOLOR color, float size, int type,
	Texture * tex, bool needsRot)
{
	instances.push_back(new Particle(this, pos, direction, looping, lifeTime, velocity, color, size, type,
		tex, needsRot));
}

void ParticleSystem::spawnParticle(const FastDelegate2<Particle *, float, D3DXVECTOR2>& _func, 
				   const D3DXVECTOR2& pos, const D3DXVECTOR2& direction,
				   bool looping, float lifeTime, float velocity, D3DCOLOR color, float size, int type,
				   Texture * tex, bool needsRot)
{
	instances.push_back(new Particle2(this, pos, direction, looping, lifeTime, velocity, color, size, type,
		tex, needsRot, _func));
}

/*void ParticleSystem::spawnParticle(const D3DXVECTOR2& pos, const D3DXVECTOR2& direction,
				   bool looping, float lifeTime, float velocity, D3DCOLOR color, float size, int type,
				   const FastDelegate2<float, float, float>& _func)
{
	instances.push_back(new Particle2(this, pos, direction, looping, lifeTime, velocity, color, size, type, _func));
}*/

float ParticleSystem::currentTime() const
{
	return curTime;
}

class TextureSorter
{
public:
	bool operator()(Particle *& p1, Particle *& p2) const
	{
		return (unsigned)p1->texture < (unsigned)p2->texture;
	}
};

void ParticleSystem::renderParticles()
{
	if(instances.empty())
		return;

	std::vector<Particle *> texturedParticles;
	texturedParticles.reserve(instances.size());

	g_Renderer()->setIdentity();
	getDevice()->SetTexture(0, NULL);

	std::vector<D3DXVECTOR2> positions; positions.reserve(instances.size());
	std::vector<D3DXVECTOR2> sizes; sizes.reserve(instances.size());
	std::vector<D3DCOLOR> colors; colors.reserve(instances.size());

	for(std::vector<Particle *>::const_iterator it = instances.begin() ; it != instances.end() ; ++it)
	{
		if((*it)->texture)
		{
			texturedParticles.push_back(*it);
			continue;
		}
		positions.push_back((*it)->pos());
		sizes.push_back(D3DXVECTOR2((*it)->size, (*it)->size));
		colors.push_back((*it)->color);
	}

	//! not textured
	if(!positions.empty())
		g_Renderer()->drawRects(&positions, &sizes, &colors, positions.size());

	if(!texturedParticles.empty())
	{
		std::sort(texturedParticles.begin(), texturedParticles.end(), TextureSorter());
		Texture * currenTex = NULL;
		for(int i = 0 ; i < texturedParticles.size() ; ++i)
		{
			if(currenTex != texturedParticles[i]->texture)
			{
				currenTex = texturedParticles[i]->texture;
				currenTex->set();
			}

			if(texturedParticles[i]->needRotation)
			{		
				float size = texturedParticles[i]->size;

				g_Renderer()->drawRotatedRect(
					texturedParticles[i]->pos().x,
					texturedParticles[i]->pos().y,
					size, size,
					texturedParticles[i]->dirVec, texturedParticles[i]->color);
			}
			else
			{
				g_Renderer()->drawRect(texturedParticles[i]->pos().x, 
					texturedParticles[i]->pos().y,
					texturedParticles[i]->size,
					texturedParticles[i]->size, texturedParticles[i]->color);
			}
		}	

		getDevice()->SetTexture(0, NULL);
	}
}

float ParticleSystem::_explosionTransform(float v, float dt)
{
	if(dt == 0.0f)
		return 0.0f;
	float tt = dt / (dt + 1.0f);

	return v * tt * cos(tt);
}

void ParticleSystem::spawnExplosion(const D3DXVECTOR2& pos, float lifeTime,
					float distance, D3DXCOLOR color, float size,
					int nParticles, int type)
{
	for (int i = 0 ; i < nParticles ; ++i)
	{
		D3DXVECTOR2 dir;
		float angle = RandomFloat(0.0f, 3.14f * 2.0f);
		dir.x = cos(angle);
		dir.y = sin(angle);
		float _life = RandomFloat(lifeTime / 3.0f, lifeTime);
		float _velocity = RandomFloat(10.0f, 30.0f);

		spawnParticle(pos, dir, false, _life,
			_velocity, color, size, type);
	}
}

bool ParticleSystem::particlesFoundByRect(float x, float y, float w, float h, int byType) const
{
	for(std::vector<Particle *>::const_iterator it = instances.begin() ; it != instances.end() ; ++it)
	{
		Particle * p = *it;
		if(p->type != byType)
			continue;
		if(p->pos().x < x)
			continue;
		if(p->pos().y < y)
			continue;
		if(p->pos().x > (x + w))
			continue;
		if(p->pos().y > (y + h))
			continue;
		return true;
	}	

	return false;
}

void ParticleSystem::clipParticles(float x, float y, float w, float h, int byType)
{
	for(std::vector<Particle *>::iterator it = instances.begin() ; it != instances.end() ; )
	{
		Particle * p = *it;
		if((p->type != byType) ||
		   (p->pos().x < x) ||
		   (p->pos().y < y) ||
		   (p->pos().x > (x + w)) ||
		   (p->pos().y > (y + h)))
		{
			++it;
		} else
		{
			delete *it;
			it = instances.erase(it);
		}
	}
}

void ParticleSystem::clear()
{
	for(std::vector<Particle *>::iterator it = instances.begin() ; it != instances.end() ; ++it)
		delete *it;
	instances.clear();
}
