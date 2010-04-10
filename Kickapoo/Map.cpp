#include "Common.h"
#include "Map.h"

Map::Map()
{

}

Map::~Map()
{
}

Map* Map::load( const std::string& name )
{
	FILE* file = fopen(name.c_str(), "r");
	if(!file)
		return NULL;

	Map* map = new Map;

	fscanf(file, "%i %i", &map->width, &map->height);
	
	map->map.resize(map->width * map->height);

	for(int i = 0; i < map->height; ++i)
		fgets(&map->map[i * map->width], map->width, file);

	fclose(file);
	return map;
}

void Map::update()
{

}

void Map::draw()
{

}