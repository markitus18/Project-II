#include "M_Window.h"
#include "j1App.h"

M_Window::M_Window(bool start_enabled) : j1Module(start_enabled)
{
	window = NULL;
	screen_surface = NULL;
	name.create("window");
}

// Destructor
M_Window::~M_Window()
{
}

// Called before render is available
bool M_Window::Awake(pugi::xml_node& config)
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		Uint32 flags = SDL_WINDOW_SHOWN;
		bool fullscreen = config.child("fullscreen").attribute("value").as_bool(false);
		bool borderless = config.child("borderless").attribute("value").as_bool(false);
		bool resizable = config.child("resizable").attribute("value").as_bool(false);
		bool fullscreen_window = config.child("fullscreen_window").attribute("value").as_bool(false);

		width = config.child("resolution").attribute("width").as_int(640);
		height = config.child("resolution").attribute("height").as_int(480);
		scale = config.child("resolution").attribute("scale").as_int(1);

		if(fullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(fullscreen_window == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(App->GetTitle(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

bool M_Window::Update(float dt)
{
	return true;
}
// Called before quitting
bool M_Window::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

// Set new window title
void M_Window::SetTitle(const char* new_title)
{
	//title.create(new_title);
	//SDL_SetWindowTitle(window, new_title);
}

void M_Window::GetWindowSize(int* width, int* height) const
{
	SDL_GetWindowSize(window, width, height);
}

void M_Window::GetOriginalWindowSize(int* width, int* height) const
{
	*width = this->width;
	*height = this->height;
}

uint M_Window::GetScale() const
{
	return scale;
}

void M_Window::SetScale(uint newScale)
{
	if (newScale >= 1)
	{
		scale = newScale;
	}
}