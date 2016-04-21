#include "M_FogOfWar.h"
#include "j1App.h"
#include "M_Render.h"

// -------------- Structure Fog Map -----------------------------------------------------------------------------

Fog_Map::Fog_Map(const int _w, const int _h)
{
	w = _w;
	h = _h;
	//Creating a new array of states
	map = new e_Fog_State*[h];
	if (w)
	{
		map[0] = new e_Fog_State[h * w];
		for (int i = 1; i < h; ++i)
		{
			map[i] = map[0] + i * w;
		}
	}

	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			map[x][y] = FOG_NOT_VISIBLE;
		}
	}
}

Fog_Map::~Fog_Map()
{
	if (w)
	{
		delete[] map[0];
	}
	delete[] map;
}

void Fog_Map::DrawCircle(int _x, int _y, int radius, e_Fog_State state)
{
	//We'll check a square around the center (_x, _y) of size radius*2

	//We find the top left corner
	int x = _x - radius;
	CAP(x, 0, w);
	int y = _y - radius;
	CAP(y, 0, h);

	//Checking all the cells in the square with two "for"s to check them all
	for (; y < _y + radius; y++)
	{
		//Making sure the cell is still in the map
		if (y < h)
		{
			for (; x < _x + radius && x < w; x++)
			{
				if (map[x][y] != state)
				{
					//Getting an aproximate distance from the center and comparing it to the radius to decide if it's part of the circle or not
					int distance = (x - _x)*(x - _x) + (y - _y) * (y - _y);
					if (distance < radius * radius)
					{
						map[x][y] = state;
					}
				}
			}

		}
		//Resetting the x coordinate
		x = _x - radius;
		CAP(x, 0, w);
	}

}

void Fog_Map::SetAll(e_Fog_State state)
{
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			map[x][y] = state;
		}
	}
}

void Fog_Map::CopyTo(Fog_Map* output)
{
	if (output == NULL)
	{
		return;
	}
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			output->map[x][y] = map[x][y];
		}
	}


}

bool Fog_Map::isVisible(int x, int y)
{
	if (x >= 0 && x < w && y >= 0 && y < h)
	{
		if (map[x][y] == FOG_VISIBLE)
		{
			return true;
		}
	}
	return false;
}





// -------------- Module Fog of War -----------------------------------------------------------------------------

M_FogOfWar::M_FogOfWar(bool start_enabled) : j1Module(start_enabled)
{
	name.create("Fog of war");
}

// Destructor
M_FogOfWar::~M_FogOfWar()
{}

// Called before render is available
bool M_FogOfWar::Awake(pugi::xml_node& config)
{
	LOG("Loading Fog of War Config");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool M_FogOfWar::Start()
{
	globalVision = false;
	ready = false;
	return true;
}

// Called every frame
bool M_FogOfWar::Update(float dt)
{
	return true;
}

bool M_FogOfWar::CleanUp()
{
	EraseMaps();
	return true;
}

bool M_FogOfWar::SetUp(uint graphicalW, uint graphicalH, uint mapW, uint mapH, uint nMaps)
{
	if (ready)
	{
		EraseMaps();
	}
	for (uint n = 0; n < nMaps; n++)
	{
		CreateMap(mapW, mapH);
	}
	tileW = ceil((float)graphicalW / (float)mapW);
	tileH = ceil((float)graphicalH / (float)mapH);
	ready = true;
	return true;
}

void M_FogOfWar::EraseMaps()
{
	for (int n = 0; n < maps.size(); n++)
	{
		RELEASE(maps[n]);
	}
	maps.clear();
	ready = false;
}

void M_FogOfWar::Draw()
{
	if (!globalVision)
	{
		//Tiles to draw (culling)
		int startX = App->render->camera.x / (tileW * 2);
		int startY = App->render->camera.y / (tileH * 2);

		int endX = startX + App->render->camera.w / (tileW * 2) + 1;
		int endY = startY + App->render->camera.h / (tileH * 2) + 1;

		//Drawing all fog maps
		for (std::vector<Fog_Map*>::reverse_iterator currentMap = maps.rbegin(); currentMap != maps.rend(); currentMap++)
		{
			if ((*currentMap)->draw)
			{
				for (int y = startY; y <= endY && y < (*currentMap)->GetHeight(); y++)
				{
					for (int x = startX; x <= endX && x < (*currentMap)->GetWidth(); x++)
					{
						if (!(*currentMap)->isVisible(x, y))
						{
							App->render->AddRect({ x * tileW, y * tileH, tileW, tileH }, true, 0, 0, 0, (*currentMap)->maxAlpha);
						}
						else if (!(*currentMap)->isVisible(x + 1, y) || !(*currentMap)->isVisible(x - 1, y) || !(*currentMap)->isVisible(x, y + 1) || !(*currentMap)->isVisible(x, y - 1))
						{
							App->render->AddRect({ x * tileW, y * tileH, tileW, tileH }, true, 0, 0, 0, (*currentMap)->maxAlpha / 2);
						}
					}
				}
			}
		}
	}
}

void M_FogOfWar::DrawCircle(int x, int y, uint radius, e_Fog_State state, int map)
{
	if (!globalVision)
	{
		int tileX = floor(x / tileW);
		int tileY = floor(y / tileH);
		int tileRadius = floor(radius / ((tileW + tileH) / 2));

		if (map >= 0)
		{
			if (map < maps.size())
			{
				maps[map]->DrawCircle(tileX, tileY, tileRadius, state);
			}
		}
		else
		{
			for (int n = 0; n < maps.size(); n++)
			{
				maps[n]->DrawCircle(tileX, tileY, tileRadius, state);
			}
		}
	}
}

void M_FogOfWar::ClearMap(int map)
{
	if (map >= 0)
	{
		if (map < maps.size())
		{
			maps[map]->SetAll(FOG_NOT_VISIBLE);
		}
	}
	else
	{
		for (int n = 0; n < maps.size(); n++)
		{
			maps[n]->SetAll(FOG_NOT_VISIBLE);
		}
	}
}

bool M_FogOfWar::Copy(uint from, uint to)
{
	if (ready && (from >= 0 && from < maps.size()) && (to >= 0 && to < maps.size()))
	{
		return false;
	}
	maps[from]->CopyTo(maps[to]);
	return true;
}

bool M_FogOfWar::IsVisible(int x, int y)
{
	bool ret = true;
	if (!globalVision)
	{
		int tileX = floor(x / tileW);
		int tileY = floor(y / tileH);
		for (int n = 0; n < maps.size() && ret; n++)
		{
			if (maps[n]->isVisible(tileX, tileY) == false)
			{
				ret = false;
			}
		}
	}
	return ret;
}

int M_FogOfWar::CreateMap(int w, int h, int maxAlpha)
{
	int ret = -1;

	Fog_Map* tmp = new Fog_Map(w, h);
	if (ret)
	{
		tmp->maxAlpha = maxAlpha;
		ret = maps.size();
		maps.push_back(tmp);
	}
	return ret;
}