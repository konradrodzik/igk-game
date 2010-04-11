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

	if(type == ParticleHarmful)
	{

	}
}

Particle::~Particle()
{
}

bool Particle::updateState(Map * map, float rt)
{
	float tt = particleSystem->currentTime();

	if((type == ParticleShot || type == ParticleHarmful) && map)
	{
		float px = (position.x) / BLOCK_SIZE;
		float py = (position.y) / BLOCK_SIZE;
		if(map->blocked(px, py, false))
		{
			if(type == ParticleShot)
			{
			g_ParticleSystem()->spawnExplosion(position, 2.0f, 30.0f,
				D3DCOLOR_ARGB(200,0,200,0), 3.0f, 20, 0, true);
			}

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

	position += applyVelocity(this, g_Timer()->getFrameTime(), rt);
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
	{
		Particle * p = *it;
		delete p;
	}
	for(std::vector<Particle *>::iterator it = queue.begin() ; it != queue.end() ; ++it)
	{
		Particle * p = *it;
		delete p;
	}
}

template<typename Cont, typename Itor>
Itor fast_erase(Cont& cont, Itor itor) {
	if(cont.empty())
		return cont.end();

	if(cont.end()-1 == itor) {
		cont.resize(cont.size()-1);
		return cont.end();
	}

	std::swap(*itor, cont.back());
	cont.resize(cont.size()-1);
	return itor;
}

void ParticleSystem::updateParticles(Map * map, float rt)
{
	LARGE_INTEGER timer = g_Timer()->getCurrentTime();
	
	curTime = g_Timer()->getEngineTime();
	for(std::vector<Particle *>::iterator it = instances.begin() ; it != instances.end() ; )
	{
		if((*it)->updateState(map, rt))
		{
			++it;
		} else {
			Particle * p = *it;
			delete p;

			it = fast_erase(instances, it);
			//it = instances.erase(it);
		}
	}

	double updateTImer = g_Timer()->calculateTime(&timer);
	updateTImer;

	// Copy items from queue to this item
	for(std::vector<Particle *>::iterator it = queue.begin() ; it != queue.end() ; ++it)
	{
	instances.push_back(*it);
	}
	queue.clear();
}

void ParticleSystem::spawnParticle(const D3DXVECTOR2& pos, const D3DXVECTOR2& direction,
	bool looping, float lifeTime, float velocity, D3DCOLOR color, float size, int type,
	Texture * tex, bool needsRot, bool queue)
{
	Particle * pp = new Particle(this, pos, direction, looping, lifeTime, velocity, color, size, type,
		tex, needsRot);

	if(queue)
		this->queue.push_back(pp);
	else
		instances.push_back(pp);
}

void ParticleSystem::spawnParticle(const FastDelegate2<Particle *, float, D3DXVECTOR2>& _func, 
				   const D3DXVECTOR2& pos, const D3DXVECTOR2& direction,
				   bool looping, float lifeTime, float velocity, D3DCOLOR color, float size, int type,
				   Texture * tex, bool needsRot, bool queue)
{
	Particle * pp = new Particle2(this, pos, direction, looping, lifeTime, velocity, color, size, type,
		tex, needsRot, _func);
	if(queue)
		this->queue.push_back(pp);
	else
		instances.push_back(pp);
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

	static std::vector<Particle *> texturedParticles;
	texturedParticles.resize(0);
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
					int nParticles, int type, bool queue)
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
			_velocity, color, size, type, NULL, false, queue);
	}
}

void ParticleSystem::checkParticlesAgainstMap(Map& map, int byType, Game& game) {
	for(std::vector<Particle *>::iterator it = instances.begin() ; it != instances.end() ; )
	{
		Particle * p = *it;
		if(p->type != byType) {
			++it;
			continue;
		}

		int index = map.index(p->pos().x / BLOCK_SIZE, p->pos().y / BLOCK_SIZE);
		Tower* tower = map.towerListMapped[index];

		if(tower) {
			if(tower->state != ETS_ALIVE) {
				++it;
				continue;
			}

			game.killTower(tower);

			it = fast_erase(instances, it);
		}
		else {
			++it;
		}
	}
}

void ParticleSystem::checkParticlesAgainstPlayer(const std::vector<Player> * players, int byType, FastDelegate0<> killPlayer)
{
	for(std::vector<Particle *>::iterator it = instances.begin() ; it != instances.end() ; ++it)
	{
		Particle * p = *it;
		if(p->type != byType) {
			continue;
		}

		for(int i = 0 ; i < players->size() ; ++i)
		{
			const Player * q = &(*players)[i];
			float x1 = q->getX() - q->playerSize / 2;
			float y1 = q->getY() - q->playerSize / 2;
			float x2 = q->getX() + q->playerSize / 2;
			float y2 = q->getY() + q->playerSize / 2;
			if(p->pos().x < x1 || p->pos().x > x2 ||
				p->pos().y < y1 || p->pos().y > y2)
			{
				killPlayer();
				return;
			}
		}
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
			Particle * p = *it;
			delete p;

			it = instances.erase(it);
		}
	}
}

void ParticleSystem::clear()
{
	for(std::vector<Particle *>::iterator it = instances.begin() ; it != instances.end() ; ++it)
	{
		Particle * p = *it;
		delete p;
	}

	for(std::vector<Particle *>::iterator it = queue.begin() ; it != queue.end() ; ++it)
	{
		Particle * p = *it;
		delete p;
	}

		
	instances.clear();
	queue.clear();
}

void ParticleSystem::addParticle(Particle * particle, bool queue)
{
	if(queue)
		this->queue.push_back(particle);
	else
		instances.push_back(particle);
}
