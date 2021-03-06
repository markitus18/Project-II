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
bool M_Audio::Start()
{
	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s", SDL_GetError());
		enabled = false;
		ret = false;
	}

	// load support for the JPG and PNG image formats  <---- WTF�?
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		enabled = false;
		ret = false;
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError());
		enabled = false;
		ret = false;
	}
	if (ret)
	{
		Mix_Volume(-1, 128);
		Mix_VolumeMusic(60);
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
		//Mix_FreeChunk(music);
		Mix_FreeMusic(music);
	}

	std::vector<Mix_Chunk*>::iterator item;
	for(item = fx.begin(); item != fx.end(); item++)
		Mix_FreeChunk((*item));

	fx.clear();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

void M_Audio::ClearLoadedFX()
{
	std::vector<Mix_Chunk*>::iterator item;
	for (item = fx.begin(); item != fx.end(); item++)
		Mix_FreeChunk((*item));
	fx.clear();
	fx.shrink_to_fit();
}

// Play a music file
bool M_Audio::PlayMusic(const char* path, float fade_time)
{
	bool ret = true;

	if (!enabled)
		return false;

	if (music != NULL)
	{
		Mix_FreeMusic(music);
		music = NULL;
	}

	music = Mix_LoadMUS_RW(App->fs->Load(path), 1);

	if(music == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s", path, Mix_GetError());
		ret = false;
	}
	else if (musicChannel = Mix_PlayMusic(music, -1) < 0)
	{
		LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
		ret = false;
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
		Mix_PauseMusic();
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
	//	LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
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

	if (!enabled)
		return false;

	

	if(id > 0 && id <= fx.size())
	{

		//Mix_Chunk* tmp = fx[id];
		//Mix_PlayChannel(-1, tmp, repeat);

		//This is the original code, keeping it until we can confirm new method works properly
		Mix_PlayChannel(-1, fx[id - 1], repeat);
	}

	return true;
}

void M_Audio::SetVolume(uint volume)
{
	if (!enabled)
		return;

	Mix_Volume(-1, volume);
}

int M_Audio::GetVolume()
{
	if (!enabled)
		return 0;
	return Mix_Volume(-1, -1);
}