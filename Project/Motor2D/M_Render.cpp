#include "j1App.h"
#include "M_Window.h"
#include "M_Render.h"
#include "Intersections.h"
#include "M_Console.h"
#include <math.h>

#define VSYNC true

M_Render::M_Render(bool start_enabled) : j1Module(start_enabled)
{
	name.create("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
M_Render::~M_Render()
{}

// Called before render is available
bool M_Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");
	bool ret = true;
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;

	if (config.child("vsync").attribute("value").as_bool(true) == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
	}

	renderer = SDL_CreateRenderer(App->win->window, -1, flags);

	if (renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = App->win->screen_surface->w;
		camera.h = App->win->screen_surface->h;
		camera.x = 0;
		camera.y = 0;
	}

	App->console->AddCVar("camera_x", &camera.x);
	App->console->AddCVar("camera_y", &camera.y);

	SDL_RenderGetViewport(renderer, &viewport);

	return ret;
}

// Called before the first frame
bool M_Render::Start()
{
	LOG("render start");
	// back background

	return true;
}

// Called each loop iteration
bool M_Render::PreUpdate()
{
	SDL_RenderClear(renderer);
	return true;
}

bool M_Render::PostUpdate(float dt)
{
	if (movingCamera)
	{
		C_Vec2<float> path = { (float)(cameraMoveEnd.x - cameraMoveStart.x), (float)(cameraMoveEnd.y - cameraMoveStart.y) };
		C_Vec2<float> pathLeft = { (float)(cameraMoveEnd.x - camera.x), (float)(cameraMoveEnd.y - camera.y) };

		if (pathLeft.x >= 1 || pathLeft.y >= 1)
		{
			float tmp = ((path.GetModule() - pathLeft.GetModule()) / path.GetModule()) * 3.14159265359;

			float speedMultiplier = sin(tmp);
			CAP(speedMultiplier, 0.1, 1);

			pathLeft.Normalize();
			pathLeft *= ((float)movingCameraSpeed * dt *speedMultiplier * 100.0f);
			camera.x += pathLeft.x;
			camera.y += pathLeft.y;
			if ((cameraMoveEnd.x - camera.x) * (cameraMoveEnd.x - camera.x) + (cameraMoveEnd.y - camera.y) * (cameraMoveEnd.y - camera.y) <= pathLeft.GetModule() * pathLeft.GetModule() * 2)
			{
				movingCamera = false;
				camera.x = cameraMoveEnd.x;
				camera.y = cameraMoveEnd.y;
			}
		}
		else
		{
			movingCamera = false;
			camera.x = cameraMoveEnd.x;
			camera.y = cameraMoveEnd.y;
		}

	}
	else if (shakePoints.empty() == false)
	{
		MoveCamera(shakePoints.back().x, shakePoints.back().y, shakingIntensity);
		shakePoints.pop_back();
	}

	performanceTimer.Start();
	//Scene sprites iteration
	std::multimap<int, C_Sprite>::const_iterator itDecal = spriteList_decals.begin();
	while (itDecal != spriteList_decals.end())
	{
		Blit((*itDecal).second.texture, &(*itDecal).second.position, (*itDecal).second.useCamera, &(*itDecal).second.section, (*itDecal).second.flip, (*itDecal).second.tint);
		itDecal++;
	}
	spriteList_decals.clear();

	std::multimap<int, C_Sprite>::const_iterator it = spriteList_scene.begin();
	while (it != spriteList_scene.end())
	{
		Blit((*it).second.texture, &(*it).second.position, (*it).second.useCamera, &(*it).second.section, (*it).second.flip, (*it).second.tint);
		it++;
	}
	spriteList_scene.clear();

	//Flying units iteration
	std::multimap<int, C_Sprite>::const_iterator itFly = spriteList_flyers.begin();
	while (itFly != spriteList_flyers.end())
	{
		Blit((*itFly).second.texture, &(*itFly).second.position, (*itFly).second.useCamera, &(*itFly).second.section, (*itFly).second.flip, (*itFly).second.tint);
		itFly++;
	}
	spriteList_flyers.clear();

	//Fx iteration
	std::multimap<int, C_Sprite>::const_iterator itFx = spriteList_fx.begin();
	while (itFx != spriteList_fx.end())
	{
		Blit((*itFx).second.texture, &(*itFx).second.position, (*itFx).second.useCamera, &(*itFx).second.section, (*itFx).second.flip, (*itFx).second.tint);
		itFx++;
	}
	spriteList_fx.clear();

	//Rects iteration
	std::vector<C_Rect>::const_iterator rect_it = rectList.begin();
	while (rect_it != rectList.end())
	{
		if (IsRectDrawable(&(*rect_it)))
			DrawQuad((*rect_it).rect, (*rect_it).useCamera, (*rect_it).r, (*rect_it).g, (*rect_it).b, (*rect_it).a, (*rect_it).filled);
		rect_it++;
	}
	rectList.clear();

	//UI Sprites iteration
	std::multimap<int, C_Sprite>::const_iterator it2 = spriteList_GUI.begin();
	while (it2 != spriteList_GUI.end())
	{
		Blit((*it2).second.texture, &(*it2).second.position, (*it2).second.useCamera, &(*it2).second.section, (*it2).second.flip, (*it2).second.tint);
		it2++;
	}
	spriteList_GUI.clear();

	//Over UI sprites iteration
	it2 = spriteList_OverGui.begin();
	while (it2 != spriteList_OverGui.end())
	{
		Blit((*it2).second.texture, &(*it2).second.position, (*it2).second.useCamera, &(*it2).second.section, (*it2).second.flip, (*it2).second.tint);
		it2++;
	}
	spriteList_OverGui.clear();

	//Debug Rects iteration
	std::vector<C_Rect>::const_iterator rect_D_it = rectDebugList.begin();
	while (rect_D_it != rectDebugList.end())
	{
		DrawQuad((*rect_D_it).rect, (*rect_D_it).useCamera, (*rect_D_it).r, (*rect_D_it).g, (*rect_D_it).b, (*rect_D_it).a, (*rect_D_it).filled);
		rect_D_it++;
	}
	rectDebugList.clear();

	//UI Sprites iteration
	it2 = spriteList_Cursor.begin();
	while (it2 != spriteList_Cursor.end())
	{
		Blit((*it2).second.texture, &(*it2).second.position, (*it2).second.useCamera, &(*it2).second.section, (*it2).second.flip, (*it2).second.tint);
		it2++;
	}
	spriteList_Cursor.clear();

	//Lines iteration
	std::vector<C_Line>::const_iterator line_it = lineList.begin();
	while (line_it != lineList.end())
	{
		DrawLine((*line_it).x1, (*line_it).y1, (*line_it).x2, (*line_it).y2, (*line_it).useCamera, (*line_it).r, (*line_it).g, (*line_it).b, (*line_it).a);
		line_it++;
	}
	lineList.clear();

	//Circles iteration
	std::vector<C_Circle>::const_iterator circle_it = circleList.begin();
	while (circle_it != circleList.end())
	{
		DrawCircle((*circle_it).x, (*circle_it).y, (*circle_it).radius, (*circle_it).useCamera, (*circle_it).r, (*circle_it).g, (*circle_it).b, (*circle_it).a);
		circle_it++;
	}
	circleList.clear();
	//LOG("Render draw took %f ms", performanceTimer.ReadMs());
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool M_Render::CleanUp()
{

	std::multimap<int, C_Sprite>::const_iterator it = spriteList_scene.begin();
	while (it != spriteList_scene.end())
	{
		//(*it).second.inList = false;
		it++;
	}
	spriteList_scene.clear();


	std::multimap<int, C_Sprite>::const_iterator it2 = spriteList_GUI.begin();
	while (it2 != spriteList_GUI.end())
	{
		//(*it2).second.inList = false;
		it2++;
	}
	spriteList_GUI.clear();

	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

// Load Game State
bool M_Render::Load(pugi::xml_node& data)
{
	camera.x = data.child("camera").attribute("x").as_int();
	camera.y = data.child("camera").attribute("y").as_int();
	movingCamera = false;
	return true;
}

// Save Game State
bool M_Render::Save(pugi::xml_node& data) const
{
	pugi::xml_node cam = data.append_child("camera");

	cam.append_attribute("x") = camera.x;
	cam.append_attribute("y") = camera.y;

	return true;
}

void M_Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void M_Render::SetCameraLimits(iPoint minimum, iPoint maximum)
{
	minimumCamera = minimum;
	maximumCamera = maximum;
}

void M_Render::CapCamera()
{
	CAP(camera.x, minimumCamera.x, maximumCamera.x);
	CAP(camera.y, minimumCamera.y, maximumCamera.y);
}

void M_Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void M_Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

iPoint M_Render::ScreenToWorld(int x, int y) const
{
	iPoint ret;
	int scale = App->win->GetScale();

	ret.x = (x + camera.x / scale);
	ret.y = (y + camera.y / scale);

	return ret;
}

void M_Render::MoveCamera(int x, int y, int speed)
{
		CAP(x, minimumCamera.x, maximumCamera.x);
		CAP(y, minimumCamera.y, maximumCamera.y);
		cameraMoveStart = { camera.x, camera.y };
		cameraMoveEnd = { x, y };
		movingCameraSpeed = speed;
		movingCamera = true;
}

void M_Render::ShakeCamera(int radius, int nTicks, int intensity)
{
	if (shakePoints.empty() == false || radius > shakingRadius)
	{
		shakePoints.clear();
	}
	for (int n = 0; n < nTicks; n++)
	{
		shakingRadius = radius;
		shakingIntensity = intensity;
		int posX = camera.x + rand() % (radius * 2) - radius;
		int posY = camera.y + rand() % (radius * 2) - radius;
		CAP(posY, minimumCamera.x, maximumCamera.x);
		CAP(posX, minimumCamera.y, maximumCamera.y);

		shakePoints.push_back({ posX, posY });
	}
	
}

// Blit to screen
bool M_Render::Blit(const SDL_Texture* texture, int x, int y, bool useCamera, const SDL_Rect* section, SDL_RendererFlip flip, float speed, double angle, int pivot_x, int pivot_y)
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_Rect rect;

	rect.x = (int)x * scale;
	rect.y = (int)y * scale;

	if (useCamera)
	{
		CapCamera();
		rect.x += (int)(-camera.x * speed);
		rect.y += (int)(-camera.y * speed);
	}

	if (section != NULL && (section->w != 0 && section->h != 0))
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture((SDL_Texture*)texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if (pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	if (SDL_RenderCopyEx(renderer, (SDL_Texture*)texture, section, &rect, angle, p, flip) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool M_Render::Blit(const SDL_Texture* texture, const SDL_Rect* onScreenPosition, bool useCamera, const SDL_Rect* section, SDL_RendererFlip flip, SDL_Color tint, float speed, double angle, int pivot_x, int pivot_y)
{
	bool ret = true;
	if (texture == NULL)
	{
		LOG("Passed a NULL texture to render.");
			return false;
	}
	
	uint scale = App->win->GetScale();

	SDL_Rect rect;

	rect.x = (int)onScreenPosition->x * scale;
	rect.y = (int)onScreenPosition->y * scale;

	if (useCamera)
	{
		CapCamera();
		rect.x += (int)(-camera.x * speed);
		rect.y += (int)(-camera.y * speed);
	}

	rect.w = onScreenPosition->w;
	rect.h = onScreenPosition->h;

	if (onScreenPosition->w == 0 && onScreenPosition->h == 0)
	{
		if (section != NULL && (section->w != 0 && section->h != 0))
		{
			rect.w = section->w;
			rect.h = section->h;
		}
		else
		{
			SDL_QueryTexture((SDL_Texture*)texture, NULL, NULL, &rect.w, &rect.h);
		}
	}
	else
	{
		rect.w = onScreenPosition->w;
		rect.h = onScreenPosition->h;
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if (pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}
	SDL_SetTextureColorMod((SDL_Texture*)texture, tint.r ,  tint.g, tint.b);
	int k = SDL_SetTextureAlphaMod((SDL_Texture*)texture, tint.a);
	if (section != NULL && section->w != 0 && section->h != 0)
	{
		if (SDL_RenderCopyEx(renderer, (SDL_Texture*)texture, section, &rect, angle, p, flip) != 0)
		{
			LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
			ret = false;
		}
	}
	else
		if (SDL_RenderCopyEx(renderer, (SDL_Texture*)texture, NULL, &rect, angle, p, flip) != 0)
		{
			LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
			ret = false;
		}
	return ret;
}

bool M_Render::DrawQuad(const SDL_Rect& rect, bool useCamera, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	rec.x = (int)(rect.x * scale);
	rec.y = (int)(rect.y * scale);
	rec.w *= scale;
	rec.h *= scale;

	if (useCamera)
	{
		rec.x -= camera.x;
		rec.y -= camera.y;
	}


	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool M_Render::DrawLine(int x1, int y1, int x2, int y2, bool useCamera, Uint8 r, Uint8 g, Uint8 b, Uint8 a) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if (useCamera)
		result = SDL_RenderDrawLine(renderer, -camera.x + x1 * scale, -camera.y + y1 * scale, -camera.x + x2 * scale, -camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool M_Render::DrawCircle(int x, int y, int radius, bool useCamera, Uint8 r, Uint8 g, Uint8 b, Uint8 a) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	if (useCamera)
	{
		x *= scale;
		y *= scale;
		x -= camera.x;
		y -= camera.y;
	}

	for (uint i = 0; i < 360; ++i)
	{
		points[i].x = (int)((x ) + radius * cos(i * factor) * scale);
		points[i].y = (int)((y) + radius * sin(i * factor) * scale);
	}


	result = SDL_RenderDrawPoints(renderer, points, 360);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool M_Render::IsSpriteDrawable(const C_Sprite* sprite) const
{
	return true;
}

bool M_Render::IsRectDrawable(const C_Rect* rect) const
{
	/*int scale = App->win->GetScale();
	SDL_Rect windowRect = { 0, 0, 0, 0 };
	windowRect.w = camera.w;
	windowRect.h = camera.h;
	
	SDL_Rect spriteRect = rect->rect;
	spriteRect.x *= scale;
	spriteRect.y *= scale;
	spriteRect.w *= scale;
	spriteRect.h *= scale;
	
	if (rect->useCamera)
	{
		spriteRect.x -= (int)(camera.x);
		spriteRect.y -= (int)(camera.y);
	}

	return I_Rect_Rect(windowRect, rect->rect);*/
	return true;
}

bool M_Render::IsLineDrawable(const C_Line* line) const
{
	return true;
}

bool M_Render::IsCircleDrawable(const C_Circle* circle) const
{
	return true;
}

void M_Render::AddSprite( C_Sprite* sprite, C_Sprite_Type type)
{
	if (!sprite->texture)
	{
		return;
	}

	switch (type)
	{
	case (DECAL) :
	{
		sprite->inList = true;
		sprite->list = &spriteList_scene;
		std::pair<int, C_Sprite> toAdd((*sprite).y_ref, *sprite);
		spriteList_decals.insert(toAdd);

		break;
	}
	case (SCENE) :
	{
		sprite->inList = true;
		sprite->list = &spriteList_scene;
		std::pair<int, C_Sprite> toAdd((*sprite).y_ref, *sprite);
		spriteList_scene.insert(toAdd);

		break;
	}
	case (FLYER) :
	{
		sprite->inList = true;
		sprite->list = &spriteList_flyers;
		std::pair<int, C_Sprite> toAdd((*sprite).layer, *sprite);
		spriteList_flyers.insert(toAdd);
		break;
	}
	case (FX) :
	{
		sprite->inList = true;
		sprite->list = &spriteList_fx;
		std::pair<int, C_Sprite> toAdd((*sprite).layer, *sprite);
		spriteList_fx.insert(toAdd);
		break;
	}
	case (GUI) :
	{
		sprite->inList = true;
		sprite->list = &spriteList_GUI;
		std::pair<int, C_Sprite> toAdd((*sprite).layer, *sprite);
		spriteList_GUI.insert(toAdd);
		break;
	}
	case (OVER_GUI) :
	{
		sprite->inList = true;
		sprite->list = &spriteList_OverGui;
		std::pair<int, C_Sprite> toAdd((*sprite).layer, *sprite);
		spriteList_OverGui.insert(toAdd);
		break;
	}
	case (CURSOR) :
	{
		sprite->inList = true;
		sprite->list = &spriteList_Cursor;
		std::pair<int, C_Sprite> toAdd((*sprite).layer, *sprite);
		spriteList_Cursor.insert(toAdd);
		break;
	}
	}
}



void M_Render::AddRect(const SDL_Rect& rect, bool useCamera, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled)
{
	C_Rect c_rect(rect, r, g, b, a, useCamera, filled);

	if (c_rect.rect.w < 0)
	{
		c_rect.rect.x += c_rect.rect.w;
		c_rect.rect.w = -c_rect.rect.w;
	}
	if (c_rect.rect.h < 0)
	{
		c_rect.rect.y += c_rect.rect.h;
		c_rect.rect.h = -c_rect.rect.h;
	}
	rectList.push_back(c_rect);
}

void M_Render::AddDebugRect(const SDL_Rect& rect, bool useCamera, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled)
{
	C_Rect c_rect(rect, r, g, b, a, useCamera, filled);
	if (c_rect.rect.w < 0)
	{
		c_rect.rect.x += c_rect.rect.w;
		c_rect.rect.w = -c_rect.rect.w;
	}
	if (c_rect.rect.h < 0)
	{
		c_rect.rect.y += c_rect.rect.h;
		c_rect.rect.h = -c_rect.rect.h;
	}
	rectDebugList.push_back(c_rect);
}


void M_Render::AddLine(int x1, int y1, int x2, int y2, bool useCamera, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	C_Line line(x1, y1, x2, y2, r, g, b, a, useCamera);
	lineList.push_back(line);
}


void M_Render::AddCircle(int x1, int y1, int radius, bool useCamera, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	C_Circle circle(x1, y1, radius, r, g, b, a, useCamera);
	circleList.push_back(circle);
}