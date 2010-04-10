#include "Common.h"

class Map
{
public:
	Map();

	~Map();

	static Map* load(const std::string& name);

	void update();

	void draw();

public:
	char **map;
};