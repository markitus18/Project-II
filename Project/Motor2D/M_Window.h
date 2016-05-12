#ifndef __j1WINDOW_H__
#define __j1WINDOW_H__

#include "j1Module.h"

class M_Window : public j1Module
{
public:

	M_Window(bool);

	// Destructor
	virtual ~M_Window();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Changae title
	void SetTitle(const char* new_title);

	// Retrive window size
	void GetWindowSize(int* width, int* height) const;
	void GetOriginalWindowSize(int* width, int* height) const;

	// Retrieve window scale
	uint GetScale() const;
	uint GetRealScale() const;
	void SetScale(uint);

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

private:
	C_String	title;
	uint		width;
	uint		height;
	uint		scale;
};

#endif // __j1WINDOW_H__