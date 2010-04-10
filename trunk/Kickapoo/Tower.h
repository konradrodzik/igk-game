#pragma once
#include "Common.h"

class Tower
{
public:
	void draw()
	{
		texture->set();
		//g_Renderer()->drawRect()
	}

private:
	Texture* texture;
	
};