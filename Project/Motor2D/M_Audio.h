#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "j1Module.h"

#define DEFAULT_MUSIC_FADE_TIME 0.0f

struct _Mix_Music;
struct Mix_Chunk;

class M_Audio : public j1Module
{
public:

	M_Audio(bool);

	// Destructor
	virtual ~M_Audio();

	// Called before render is available
	bool Start();

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	void StopMusic();

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

	void SetVolume(uint volume);
	int GetVolume();

private:

	_Mix_Music*				music = NULL;
	std::vector<Mix_Chunk*>	fx;
	uint musicChannel = 0;
};

#endif // __j1AUDIO_H__