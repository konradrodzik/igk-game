#pragma once
#include "Common.h"

static const int BLOCK_SIZE = 20;

class Map
{
public:
	Map();

	~Map();

	static Map* load(const std::string& name);

	void update();

	void draw();

	char at(int x, int y) const {
		return map[y * width + x];
	}

public:
	string map;
	int width;
	int height;
	Texture* wall;
};