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

	void MoveCamera(int x, int y);

	// Actual object rendering
	bool Blit(const SDL_Texture* texture, int x, int y, bool useCamera = true, const SDL_Rect* section = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, float speed = 1.0f, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX) const;
	bool Blit(const SDL_Texture* texture, const SDL_Rect* onScreenPosition, bool useCamera = true, const SDL_Rect* section = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, SDL_Color tint = { 255, 255, 255, 0 }, float speed = 1.0f, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX);
	bool DrawQuad(const SDL_Rect& rect, bool useCamera = true, Uint8 r = 255, Uint8 g = 255, Uint8 b = 255, Uint8 a = 255, bool filled = true) const;
	bool DrawLine(int x1, int y1, int x2, int y2, bool useCamera = true, Uint8 r = 255, Uint8 g = 255, Uint8 b = 255, Uint8 a = 255) const;
	bool DrawCircle(int x1, int y1, int redius, bool useCamera = true, Uint8 r = 255, Uint8 g = 255, Uint8 b = 255, Uint8 a = 255) const;

	bool IsSpriteDrawable(const C_Sprite*) const;
	bool IsRectDrawable(const C_Rect*) const;
	bool IsLineDrawable(const C_Line*) const;
	bool IsCircleDrawable(const C_Circle*) const;

	// Adding debug objects to render lately
	void AddSprite( C_Sprite*, C_Sprite_Type);
	void AddRect(const SDL_Rect& rect, bool useCamera = true, Uint8 r = 255, Uint8 g = 255, Uint8 b = 255, Uint8 a = 255, bool filled = true);
	void AddDebugRect(const SDL_Rect& rect, bool useCamera = true, Uint8 r = 255, Uint8 g = 255, Uint8 b = 255, Uint8 a = 255, bool filled = true);
	void AddLine(int x1, int y1, int x2, int y2, bool useCamera = true, Uint8 r = 255, Uint8 g = 255, Uint8 b = 255, Uint8 a = 255);
	void AddCircle(int x1, int y1, int radius, bool useCamera = true, Uint8 r = 255, Uint8 g = 255, Uint8 b = 255, Uint8 a = 255);

	// Set background color
	void SetBackgroundColor(SDL_Color color);

	SDL_Renderer*	renderer;
	SDL_Rect		camera;
	SDL_Rect		viewport;
	SDL_Color		background;

private:
	std::multimap<int, C_Sprite>	spriteList_decals;
	std::multimap<int,  C_Sprite>	spriteList_scene;
	std::multimap<int, C_Sprite>	spriteList_flyers;
	std::multimap<int, C_Sprite>	spriteList_fx;
	std::multimap<int,  C_Sprite>	spriteList_GUI;
	std::multimap<int, C_Sprite>	spriteList_OverGui;
	std::multimap<int, C_Sprite>	spriteList_Cursor;

	std::vector<C_Line> lineList;
	std::vector<C_Rect> rectList;
	std::vector<C_Rect> rectDebugList;
	std::vector<C_Circle> circleList;

public:
	bool movingCamera;
private:
	iPoint cameraMoveStart = { 0, 0 };
	iPoint cameraMoveEnd = { 0, 0 };
};

#endif // __j1RENDER_H__