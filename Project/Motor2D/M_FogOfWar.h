#ifndef __FOG_OF_WAR__
#define __FOG_OF_WAR__

#include "j1Module.h"

class Fog_Map
{
public:
	Fog_Map(int w, int h);
	~Fog_Map();

	//Draw a circle onto the map
	void DrawCircle(int _x, int _y, int radius, bool visible = true);

	//Set all tiles to a value
	void SetAll(bool visible);

	//Copy this map's tiles to another map
	void CopyTo(Fog_Map* output);


	//Returns if a certain tile is visible or not
	bool isVisible(int x, int y);

	//Number of width tiles
	uint GetWidth() { return w; }
	//Number of height tiles
	uint GetHeight() { return h; }

	//Soften the edges of a certain section of the map
	void SoftenSection(int x1, int y1, int x2, int y2, float fadeRatio = 1.5f);

	//Alpha the non-visible tiles will have
	uint maxAlpha = 255;

	uint** map;

	//Defines if this map will be drawn
	bool draw = true;
private:
	uint w = 0;
	uint h = 0;
};

class M_FogOfWar : public j1Module
{
public:

	M_FogOfWar(bool start_enabled);

	// Destructor
	~M_FogOfWar();

	bool CleanUp();


	bool SetUp(uint graphicalW, uint graphicalH, uint mapW = 100, uint mapH = 100, uint nMaps = 1);
	void SetMinimap(int x, int y, int w, int h, int spacing = 5);

	void EraseMaps();

	void Draw();

	//map: map to draw the circle on. Leave on -1 to draw on all.
	void DrawCircle(int x, int y, uint radius, bool visible = true, int map = -1);
	void ClearMap(int map = -1);
	bool Copy(uint from, uint to);

	bool IsVisible(int x, int y);

private:
	int CreateMap(int w, int h, int maxAlpha = 255);

public:
	std::vector<Fog_Map*> maps;
	bool globalVision;
private:
	bool ready = false;
	int tileW, tileH = 0;

	int minimapSpacing;
	bool readyMinimap;
	C_Sprite minimapImage;
	SDL_Surface* surface;

};

#endif 