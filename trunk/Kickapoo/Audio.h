#pragma once
#include "Common.h"

typedef FMOD::Sound Sound;

class Audio : public Singleton<Audio>
{
public:
	Audio() : channel(NULL) {}
	~Audio() {}

	void create();
	void release();
	void update();
	void play(FMOD::Sound* sound, float volume = 1.0f);

	FMOD::Sound* loadSound(const char* filePath);
	FMOD::Sound* loadStream(const char* filePath);

private:
	FMOD::System *system;
	FMOD::Channel *channel;
	FMOD_RESULT   result;
};
DefineAccessToSingleton(Audio)