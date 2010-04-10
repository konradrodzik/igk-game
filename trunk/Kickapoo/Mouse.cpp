#include "Common.h"
#include "Mouse.h"

Mouse::Mouse(void)
{
	size = 16.0f;
}

Mouse::~Mouse(void)
{
}

void Mouse::create()
{
	cursor = "cursor.png";
	x = g_Window()->getWidth() / 2;
	y = g_Window()->getHeight() / 2;
}

void Mouse::update()
{
	float sensitivity = 300.0f;
	int dx, dy;
	g_Input()->getMovement(dx, dy);

	x += dx * sensitivity * g_Timer()->getFrameTime(); 
	y -= dy * sensitivity * g_Timer()->getFrameTime();

	if(x < 0) x = 0; if(x > g_Window()->getWidth()) x = g_Window()->getWidth();
	if(y < 0) y = 0; if(y > g_Window()->getHeight()) y = g_Window()->getHeight();
}

void Mouse::drawCursor()
{
	cursor.set();
	g_Renderer()->setIdentity();
	g_Renderer()->drawRect(x - size/2, y - size/2, size, size);
}