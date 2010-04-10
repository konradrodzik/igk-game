#include "Common.h"
#include "Mouse.h"

Mouse::Mouse(void)
{
	size = 1.5f;
}

Mouse::~Mouse(void)
{
}

void Mouse::create()
{
	cursor = "cursor.png";
}

void Mouse::update()
{
	float sensitivity = 20.0f;
	int dx, dy;
	g_Input()->getMovement(dx, dy);

	x += dx * sensitivity * g_Timer()->getFrameTime(); 
	y -= dy * sensitivity * g_Timer()->getFrameTime();

	if(x < -40) x = -40; if(x > 40) x = 40;
	if(y < -30) y = -30; if(y > 30) y = 30;
}

void Mouse::drawCursor()
{
	cursor.set();
	g_Renderer()->setIdentity();
	g_Renderer()->drawRect(x - size/2, y - size/2, size, size);
}