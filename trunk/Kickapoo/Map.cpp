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

	int width;
	int height;
	fscanf(file, "%i %i", &width, &height);
	
	map->map = new char *[height] ;
	for( int i = 0 ; i < height ; i++ )
		map->map[i] = new char[width];


	for(int i = 0; i < height; ++i)
	{
		for(int j = 0; j < width; ++j)
		{
			char block;
			fscanf(file, "%c", &block);
			map->map[j][i] = block;
		}
	}

	fclose(file);
	return map;
}

void Map::update()
{

}

void Map::draw()
{

}