#include "Common.h"
#include "Map.h"

Map::Map()
{
	wall = new Texture;
	wall->load("gfx/wall.jpg");
}

Map::~Map()
{
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

	fclose(file);
	return map;
}

void Map::update()
{

}

void Map::draw()
{
	for(int i = 0; i < height; ++i)
	{
		for(int j = 0; j < width; ++j)
		{
			char block = at(j, i);
			if(block == '#')
			{
				wall->set();
				g_Renderer()->drawRect(width*BLOCK_SIZE, height*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
			}
		}
	}
}