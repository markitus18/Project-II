#ifndef __j1RENDER_H__
#define __j1RENDER_H__

#include "j1Module.h"

class M_Render : public j1Module
{
public:

	M_Render(bool);

	// Destructor
	virtual ~M_Render();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// Utils
	void SetViewPort(const SDL_Rect& rect);
	void ResetViewPort();
	iPoint ScreenToWorld(int x, int y) const;

	// Draw & Blit
	bool Blit(const SDL_Texture* texture, int x, int y, bool useCamera = true, const SDL_Rect* section = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, float speed = 1.0f, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX) const;
	bool Blit(const SDL_Texture* texture, const SDL_Rect* onScreenPosition, bool useCamera = true, const SDL_Rect* section = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, float speed = 1.0f, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX);
	bool DrawQuad(const SDL_Rect& rect, bool useCamera = true, Uint8 r = 255, Uint8 g = 255, Uint8 b = 255, Uint8 a = 255, bool filled = true) const;
	bool DrawLine(int x1, int y1, int x2, int y2, bool useCamera = true, Uint8 r = 255, Uint8 g = 255, Uint8 b = 255, Uint8 a = 255) const;
	bool DrawCircle(int x1, int y1, int redius, bool useCamera = true, Uint8 r = 255, Uint8 g = 255, Uint8 b = 255, Uint8 a = 255) const;

	// Set background color
	void SetBackgroundColor(SDL_Color color);

public:

	SDL_Renderer*	renderer;
	SDL_Rect		camera;
	SDL_Rect		viewport;
	SDL_Color		background;
};

#endif // __j1RENDER_H__