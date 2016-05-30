#ifndef __j1FONTS_H__
#define __j1FONTS_H__

#include "j1Module.h"
#include "SDL\include\SDL_pixels.h"

#define DEFAULT_FONT "fonts/open_sans/OpenSans-Regular.ttf"
#define DEFAULT_FONT_SIZE 10

struct SDL_Texture;
struct _TTF_Font;

class M_Fonts : public j1Module
{
public:

	M_Fonts(bool);

	// Destructor
	virtual ~M_Fonts();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Load Font
	_TTF_Font* const Load(const char* path, int size = 12);
	void Unload(_TTF_Font*);

	// Create a surface from text
	SDL_Texture* Print(const char* text, SDL_Color color = {255, 255, 255, 255}, _TTF_Font* font = NULL);

	bool CalcSize(const char* text, int& width, int& height, _TTF_Font* font = NULL) const;

	_TTF_Font* GetDefaultFont();

public:

	std::list<_TTF_Font*>	fonts;
	_TTF_Font*			default;
};


#endif // __j1FONTS_H__