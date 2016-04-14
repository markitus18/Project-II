#include "j1App.h"
#include "M_FileSystem.h"
#include "M_Audio.h"

#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

M_Audio::M_Audio(bool start_enabled) : j1Module(start_enabled)
{
	music = NULL;
	name.create("audio");
}

// Destructor
M_Audio::~M_Audio()
{}

// Called before render is available
bool M_Audio::Awake(pugi::xml_node& config)
{
	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		enabled = false;
		ret = false;
	}

	// load support for the JPG and PNG image formats
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		enabled = false;
		ret = false;
	}

	//Initialize SDL_mixer
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 4, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		enabled = false;
		ret = false;
	}
	if (ret)
	{
		Mix_Volume(-1, 30);
	}
	else
	{
		Disable();
		ret = true;
	}
	return ret;
}

// Called before quitting
bool M_Audio::CleanUp()
{
	if (!enabled)
		return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if(music != NULL)
	{
		Mix_FreeChunk(music);
		//Mix_FreeMusic(music);
	}

	std::list<Mix_Chunk*>::iterator item;
	for(item = fx.begin(); item != fx.end(); item++)
		Mix_FreeChunk((*item));

	fx.clear();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

// Play a music file
bool M_Audio::PlayMusic(const char* path, float fade_time)
{
	bool ret = true;

	if (!enabled)
		return false;

	if(music != NULL)
	{
		if(fade_time > 0.0f)
		{
		//	Mix_FadeOutMusic(int(fade_time * 1000.0f));
			Mix_FadeOutChannel(CHANNEL_MUSIC, fade_time);
		}
		else
		{
			Mix_HaltChannel(CHANNEL_MUSIC);
		//	Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeChunk(music);
	//	Mix_FreeMusic(music);
	}

	//music = Mix_LoadMUS_RW(App->fs->Load(path), 1);
	music = Mix_LoadWAV_RW(App->fs->Load(path), 1);

	if(music == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if(fade_time > 0.0f)
		{
			if (!Mix_FadeInChannel(CHANNEL_MUSIC, music, true, (int)(fade_time * 1000.0f)) < 0)				//Mix_FadeInMusic(music, -1, (int) (fade_time * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if (!Mix_PlayChannel(CHANNEL_MUSIC, music, 1))													//Mix_PlayMusic(music, -1) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}
	if (ret)
	{
		LOG("Successfully playing %s", path);
	}
	else
	{
		LOG("Error while trying to play %s", path);
	}
	return ret;
}

void M_Audio::StopMusic()
{
	if (!enabled)
		return;

	if (music != NULL)
	{
		Mix_HaltChannel(CHANNEL_MUSIC);

		Mix_FreeChunk(music);

		music = NULL;
	}
}

// Load WAV
unsigned int M_Audio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	if (!enabled)
		return 0;

	Mix_Chunk* chunk = Mix_LoadWAV_RW(App->fs->Load(path), 1);

	if(chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.push_back(chunk);
		ret = fx.size();
	}

	return ret;
}

// Play WAV
bool M_Audio::PlayFx(unsigned int id, int repeat)
{
	bool ret = false;

	if (!enabled)
		return false;

	

	if(id > 0 && id <= fx.size())
	{
		std::list<Mix_Chunk*>::iterator item = fx.begin();
		for (int n = 0; n < id - 1; n++)
		{
			item++;
			if (item == fx.end())
			{
				LOG("Error while trying to play an fx.");
				break;
			}
		}
		Mix_PlayChannel(-1, (*item), repeat);

		//This is the original code, keeping it until we can confirm new method works properly
		//Mix_PlayChannel(-1, fx[id - 1], repeat);
	}

	return ret;
}

void M_Audio::SetVolume(uint volume, e_music_channels channel)
{
	if (!enabled)
		return;

	if (channel != CHANNEL_FX)
	{
		Mix_Volume(channel, volume);
	}
	else
	{
		int musicVolume = Mix_Volume(CHANNEL_MUSIC, -1);
		Mix_Volume(-1, volume);
		Mix_Volume(CHANNEL_MUSIC, musicVolume);
	}
}

int M_Audio::GetVolume(e_music_channels channel)
{
	if (!enabled)
		return 0;
	return Mix_Volume(channel, -1);
}