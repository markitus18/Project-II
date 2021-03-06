#include "M_Fonts.h"

#include "j1App.h"
#include "M_Textures.h"
#include "M_FileSystem.h"

#include "SDL_TTF\include\SDL_ttf.h"
#pragma comment( lib, "SDL_ttf/libx86/SDL2_ttf.lib" )

M_Fonts::M_Fonts(bool start_enabled) : j1Module(start_enabled)
{
	name.create("fonts");
}

// Destructor
M_Fonts::~M_Fonts()
{}

// Called before render is available
bool M_Fonts::Awake(pugi::xml_node& conf)
{
	LOG("Init True Type Font library");
	bool ret = true;

	if(TTF_Init() == -1)
	{
		LOG("SDL_ttf could not initialize! SDL_ttf Error: %s", TTF_GetError());
		ret = false;
	}
	else
	{
		const char* path = conf.child("default_font").attribute("file").as_string(DEFAULT_FONT);
		int size = conf.child("default_font").attribute("size").as_int(DEFAULT_FONT_SIZE);
		default = Load(path, size);
	}

	return ret;
}

// Called before quitting
bool M_Fonts::CleanUp()
{
	LOG("Freeing True Type fonts and library");
	std::list<TTF_Font*>::iterator item;

	for(item = fonts.begin(); item != fonts.end(); item++)
	{
		TTF_CloseFont((*item));
	}

	fonts.clear();
	TTF_Quit();
	return true;
}

// Load new texture from file path
TTF_Font* const M_Fonts::Load(const char* path, int size)
{
	TTF_Font* font = TTF_OpenFontRW(App->fs->Load(path), 1, size);

	if(font == NULL)
	{
		LOG("Could not load TTF font with path: %s. TTF_OpenFont: %s", path, TTF_GetError());
	}
	else
	{
		//LOG("Successfully loaded font %s size %d", path, size);
		fonts.push_back(font);
	}

	return font;
}

void M_Fonts::Unload(TTF_Font* font)
{
	if (font != NULL)
	{
		std::list<TTF_Font*>::iterator item = fonts.begin();

		while (item != fonts.end())
		{
			if (*item == font)
			{
				TTF_Font* font = (*item);
				item = fonts.erase(item);
				TTF_CloseFont(font);
			}
			else
			{
				item++;
			}

		}
	}
}

// Print text using font
SDL_Texture* M_Fonts::Print(const char* text, SDL_Color color, TTF_Font* font)
{
	SDL_Texture* ret = NULL;
	SDL_Surface* surface = TTF_RenderText_Blended((font) ? font : default, text, color);

	if(surface == NULL)
	{
		LOG("Unable to render text surface! SDL_ttf Error: %s", TTF_GetError());
	}
	else
	{
		ret = App->tex->LoadSurface(surface);
		if (ret == NULL)
		{
			SDL_FreeSurface(surface);
		}
	}

	return ret;
}

// calculate size of a text
bool M_Fonts::CalcSize(const char* text, int& width, int& height, _TTF_Font* font) const
{
	bool ret = false;
	
	if(TTF_SizeText((font) ? font : default, text, &width, &height) != 0)
		LOG("Unable to calc size of text surface! SDL_ttf Error: %s", TTF_GetError());
	else
		ret = true;

	return ret;
}

_TTF_Font* M_Fonts::GetDefaultFont()
{
	return default;
}