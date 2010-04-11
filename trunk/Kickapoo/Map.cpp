#include "Common.h"

Map::Map()
{
	wall = new Texture;
	tower = new Texture;
	tower_death = new Texture;
	playerTexture = new Texture;
	playerSelected = new Texture;
	wall->load("gfx/wall.png");
	tower->load("gfx/tower.png");
	tower_death->load("gfx/tower_death.png");
	playerTexture->load("gfx/player.png");
	playerSelected->load("gfx/player_selected.png");
}

Map::~Map()
{
	delete wall;
	delete tower;
	delete tower_death;
	delete playerTexture;
	delete playerSelected;
}

static void trimr(char * buffer) {
	unsigned length = strlen(buffer);
	while(buffer[0] && (buffer[length-1] == '\n' || buffer[length-1] == '\r'))
		buffer[length-1] = 0;
}

const char* basename(const char* p) {
	if(strlen(p) >= 3)
		return &p[strlen(p)-3];
	return "";
}

Map* Map::load( const std::string& name )
{
	FILE* file = fopen(name.c_str(), "rb");
	if(!file)
		return NULL;

	Map* map = new Map;

	if(!strcmpi(basename(name.c_str()), "bmp")) {
		BITMAPFILEHEADER bmfh;
		fread(&bmfh,sizeof(BITMAPFILEHEADER),1,file);
		BITMAPINFOHEADER bmih;
		fread(&bmih,sizeof(BITMAPINFOHEADER),1,file);

		fseek(file, bmfh.bfOffBits, SEEK_SET);

		map->width = bmih.biWidth;
		map->height = bmih.biHeight;
		map->map.resize(map->width * map->height);

		assert(bmih.biBitCount == 24);
		assert(bmih.biCompression == 0);

		unsigned pad = (LONG)((float)map->width*(float)bmih.biBitCount/8.0);
		unsigned byteWidth = pad;

		//add any extra space to bring each line to a DWORD boundary
		while(pad%4!=0) {
			 pad++;
		}

		for(unsigned i = map->height; i-- > 0; ) {
			byte bytes[3];
			for(unsigned j = 0; j < map->width; ++j) {
				int index = (map->height - i - 1) * map->width + j;
				fread(bytes, 3, 1, file);
				std::swap(bytes[0], bytes[2]);
				if(bytes[0]>80 && bytes[1]>80 && bytes[2]>80)
					continue;

				if(bytes[0]>80) {
					if(bytes[2] > 80) {
						map->map[index] = '$';					
					} else {
						map->map[index] = '@';
					}
				}
				else if(bytes[2]>80) {
					map->map[index] = '^';
				}
				else if(bytes[1]>80) {
					map->map[index] = '&';
				}
				else {
					map->map[index] = '#';
				}
			}
			fread(bytes, pad-byteWidth, 1, file);
		}
	}
	else {
		fscanf(file, "%i %i", &map->width, &map->height);

		while(!feof(file)) {
			static char buffer[10000];
			fgets(buffer, sizeof(buffer), file);
			trimr(buffer);
			map->map += buffer;
		}
	}

	assert(map->map.size() == map->width*map->height);
	
	map->blockRandom.resize(map->map.size());
	map->towerListMapped.resize(map->map.size());

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
			if(block == '@' || block == '$')
			{
				Player player;
				player.Position.x = j;
				player.Position.y = i;
				player.hasMissiles = block == '$';
				player.playerTexture = playerTexture;
				player.playerSelectedTexture = playerSelected;
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

	for(unsigned i = 0; i < towerList.size(); ++i) {
		Tower& tower = towerList[i];
		towerListMapped[index(tower.Position.x, tower.Position.y)] = &tower;
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
				float size = 2+cosf(blockRandom[i * width + j]*6) * 1.0f;

				wall->set();
				g_Renderer()->drawRect(j*BLOCK_SIZE+size, i*BLOCK_SIZE+size, BLOCK_SIZE-2*size, BLOCK_SIZE-2*size);
			}
			else {
				wall->set();
				g_Renderer()->drawRect(j*BLOCK_SIZE+1, i*BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2, D3DCOLOR_XRGB(16, 16, 16));

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

			/*	float sx = (j+1)*BLOCK_SIZE-1;
				float sy = i*BLOCK_SIZE;
				float ex = (j+1)*BLOCK_SIZE-1;
				float ey = (i+1)*BLOCK_SIZE;

				float centerx = (sx + ex) * 0.5f - (ex-sx)*0.5f;
				float centery = (sy + ey) * 0.5f - (ey-sy)*0.5f;
				wall->set();
				g_Renderer()->drawRect( centerx, centery, 4, ey - sy, D3DCOLOR_XRGB(64, 64, 255));
			*/}
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
/*				float sx =j*BLOCK_SIZE;
				float sy = (i+1)*BLOCK_SIZE-1;
				float ex =(j+1)*BLOCK_SIZE;
				float ey = (i+1)*BLOCK_SIZE-1;

				float centerx = (sx + ex) * 0.5f - (ex-sx)*0.5f;
				float centery = (sy + ey) * 0.5f - (ey-sy)*0.5f;
				wall->set();
				g_Renderer()->drawRect( centerx, centery, ex-sx , 4, D3DCOLOR_XRGB(64, 64, 255));
*/
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

bool Map::blocked(int x, int y, bool withTower) const {
	if(map[index(x, y)] == '#')
		return true;

	Tower* tower = towerListMapped[index(x, y)];
	if(tower && tower->state == ETS_ALIVE)
		return withTower;

	return false;
}