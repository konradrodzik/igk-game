#include "Common.h"
#include "Map.h"

Map::Map()
{
	wall = new Texture;
	tower = new Texture;
	tower_death = new Texture;
	wall->load("gfx/wall.png");
	tower->load("gfx/tower.png");
	tower_death->load("gfx/tower_death.png");
}

Map::~Map()
{
	delete wall;
	delete tower;
}

static void trimr(char * buffer) {
	unsigned length = strlen(buffer);
	while(buffer[0] && (buffer[length-1] == '\n' || buffer[length-1] == '\r'))
		buffer[length-1] = 0;
}

Map* Map::load( const std::string& name )
{
	FILE* file = fopen(name.c_str(), "r");
	if(!file)
		return NULL;

	Map* map = new Map;

	fscanf(file, "%i %i", &map->width, &map->height);

	while(!feof(file)) {
		static char buffer[10000];
		fgets(buffer, sizeof(buffer), file);
		trimr(buffer);
		map->map += buffer;
	}

	assert(map->map.size() == map->width*map->height);
	
	map->blockRandom.resize(map->map.size());

	map->fill();

	fclose(file);
	return map;
}

void Map::fill()
{
	for(int i = 0; i < height; ++i)
	{
		for(int j = 0; j < width; ++j)
		{
			char block = map[index(j, i)];
			if(block == '#') {
				if(blockRandom[index(j, i)] == 0) {
					fill_r(j, i, 0.001f);
				}
			}
		}
	}

}

void Map::fill_r(int x, int y, float dt)
{
	if(map[index(x, y)] != '#')
		return;

	const float scale = 0.1f;

	if(blockRandom[index(x, y)] == 0) {
		blockRandom[index(x, y)] = dt;
	}
	else return;

	if(x > 0)
		fill_r(x-1, y, dt+scale);
	if(x < width-1)
		fill_r(x+1, y, dt+scale);
	if(y > 0)
		fill_r(x, y-1, dt+scale);
	if(y < width-1)
		fill_r(x, y+1, dt+scale);
}
	
void Map::loadContent(vector<Player>& playerList, vector<Tower>& towerList)
{
	playerList.clear();
	towerList.clear();

	for(int i = 0; i < height; ++i)
	{
		for(int j = 0; j < width; ++j)
		{
			char block = map[index(j, i)];
			if(block == '@')
			{
				Player player;
				player.Position.x = j;
				player.Position.y = i;
				playerList.push_back(player);
			}
			else if(block == '^' || block == '&') 
			{
				Tower tower_;
				
				if(block == '^')
					tower_.type = ETT_STATIC;
				else if(block == '&')
					tower_.type = ETT_SHOOTING;

				tower_.Position.x = j;
				tower_.Position.y = i;
				tower_.setAliveTexture(tower);
				tower_.setDeathTexture(tower_death);
				towerList.push_back(tower_);
			}
		}
	}
}

void Map::update()
{
	for(unsigned i = 0; i < blockRandom.size(); ++i)
		blockRandom[i] += g_Timer()->getFrameTime();
}

void Map::draw()
{
	for(int i = 0; i < height; ++i)
	{
		for(int j = 0; j < width; ++j)
		{
			char block = map[index(j, i)];
			if(block == '#')
			{
				float size = 4+cosf(blockRandom[i * width + j]*6) * 2.0f;

				wall->set();
				g_Renderer()->drawRect(j*BLOCK_SIZE+size, i*BLOCK_SIZE+size, BLOCK_SIZE-2*size, BLOCK_SIZE-2*size);
			}
			/*else if(block == '^')
			{
				tower->set();
				g_Renderer()->drawRect(j*BLOCK_SIZE, i*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
			}*/
		}
	}

	for(int i = 0; i < height; ++i)
	{
		for(int j = 0; j < width-1; ++j)
		{
			char a = map[index(j, i)];
			char b = map[index(j+1, i)];

			if(a == '#' && a != b || a != b && b == '#') 
			{
				g_Renderer()->drawLine((j+1)*BLOCK_SIZE-1, i*BLOCK_SIZE,
					(j+1)*BLOCK_SIZE-1, (i+1)*BLOCK_SIZE, 2, D3DCOLOR_XRGB(64, 64, 255));
			}
		}
	}

	for(int i = 0; i < height-1; ++i)
	{
		for(int j = 0; j < width; ++j)
		{
			char a = map[index(j, i)];
			char b = map[index(j, i+1)];

			if(a == '#' && a != b || a != b && b == '#') 
			{
				g_Renderer()->drawLine(j*BLOCK_SIZE, (i+1)*BLOCK_SIZE-1,
					(j+1)*BLOCK_SIZE, (i+1)*BLOCK_SIZE-1, 2, D3DCOLOR_XRGB(64, 64, 255));
			}
		}
	}
}

bool Map::collides(const D3DXVECTOR2& v) const {
	return blocked(v.x + 0.1f, v.y + 0.1f) || blocked(v.x + 0.9f, v.y + 0.1f) ||
		blocked(v.x + 0.1f, v.y + 0.9f) || blocked(v.x + 0.9f, v.y + 0.9f);
}
	
D3DXVECTOR2 Map::slide(const D3DXVECTOR2& a, const D3DXVECTOR2& b) {
	if(!collides(D3DXVECTOR2(b.x, b.y)))
		return b;
	if(!collides(D3DXVECTOR2(b.x, a.y)))
		return D3DXVECTOR2(b.x, a.y);
	if(!collides(D3DXVECTOR2(a.x, b.y)))
		return D3DXVECTOR2(a.x, b.y);
	return a;
}
