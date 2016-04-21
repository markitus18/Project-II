#ifndef __FOG_OF_WAR__
#define __FOG_OF_WAR__

#include "j1Module.h"

enum e_Fog_State
{
	FOG_VISIBLE,
	FOG_NOT_VISIBLE
};

class Fog_Map
{
public:
	Fog_Map(int w, int h);
	~Fog_Map();

	void DrawCircle(int _x, int _y, int radius, e_Fog_State state = FOG_VISIBLE);
	void SetAll(e_Fog_State state);
	void CopyTo(Fog_Map* output);

	bool isVisible(int x, int y);
	uint GetWidth() { return w; }
	uint GetHeight() { return h; }

	uint maxAlpha = 255;
	bool draw = true;
private:
	e_Fog_State** map;
	uint w = 0;
	uint h = 0;
};

class M_FogOfWar : public j1Module
{
public:

	M_FogOfWar(bool start_enabled);

	// Destructor
	~M_FogOfWar();

	bool Awake(pugi::xml_node& conf);

	bool Start();
	
	bool Update(float dt);

	bool CleanUp();


	bool SetUp(uint graphicalW, uint graphicalH, uint mapW = 100, uint mapH = 100, uint nMaps = 1);
	void EraseMaps();

	void Draw();

	//map: map to draw the circle on. Leave on -1 to draw on all.
	void DrawCircle(int x, int y, uint radius, e_Fog_State state = FOG_VISIBLE, int map = -1);
	void ClearMap(int map = -1);
	bool Copy(uint from, uint to);

	bool IsVisible(int x, int y);

private:
	int CreateMap(int w, int h, int maxAlpha = 255);

public:
	bool globalVision = false;
	std::vector<Fog_Map*> maps;
private:
	bool ready = false;
	int tileW, tileH = 0;

};

#endif 