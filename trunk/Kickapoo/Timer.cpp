#include "Common.h"
#include "Timer.h"

void Timer::init()
{
	if (!QueryPerformanceFrequency(&ticksPerSecond))
		return;

	QueryPerformanceCounter(&startTime);
}

float Timer::update()
{
	static LARGE_INTEGER lastTime = startTime;
	LARGE_INTEGER currentTime;

	QueryPerformanceCounter(&currentTime);
	
	float seconds =  (static_cast<float>(currentTime.QuadPart) - static_cast<float>(lastTime.QuadPart)) 
				    / static_cast<float>(ticksPerSecond.QuadPart);
	
	lastTime = currentTime;	

	static float frameTimes[MAX_PROBES_AVG] = {0, 0, 0, 0, 0, 0};
	static int currentProbes = 0;

	frameTimes[currentProbes++] = seconds;

	if (currentProbes > MAX_PROBES_AVG - 1) {
		currentProbes = 0;
	}

	frameTime = 0;

	for (int i = 0; i < MAX_PROBES_AVG; i++)
		frameTime += frameTimes[i];

	frameTime = frameTime / (float)MAX_PROBES_AVG;

	m_engineTime += frameTime;
	return frameTime;
}

float Timer::getFPS()
{
	static LARGE_INTEGER lastTime = startTime;

	LARGE_INTEGER currentTime;

	QueryPerformanceCounter(&currentTime);

	float fps = static_cast<float>(ticksPerSecond.QuadPart) / (static_cast<float>(currentTime.QuadPart)
			  - static_cast<float>(lastTime.QuadPart));
	
	lastTime = currentTime;
	
	avgFps = (avgFps * 100 + fps) / 101.0f;

	return avgFps;
}

LARGE_INTEGER Timer::getCurrentTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return currentTime;
}

float Timer::calculateTime(LARGE_INTEGER *time)
{
	LARGE_INTEGER currentTime;

	QueryPerformanceCounter(&currentTime);

	return (static_cast<float>(currentTime.QuadPart) - static_cast<float>(time->QuadPart)) 
		  / static_cast<float>(ticksPerSecond.QuadPart);
}