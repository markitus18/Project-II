#include "M_FogOfWar.h"
#include "j1App.h"
#include "M_Render.h"

// -------------- Structure Fog Map -----------------------------------------------------------------------------

Fog_Map::Fog_Map(const int _w, const int _h)
{
	w = _w;
	h = _h;
	//Creating a new two dimensional array of tiles
	map = new uint*[h];
	if (w)
	{
		map[0] = new uint[h * w];
		for (int i = 1; i < h; ++i)
		{
			map[i] = map[0] + i * w;
		}
	}

	//Initializing them all non-visible
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			map[x][y] = maxAlpha;
		}
	}
}

Fog_Map::~Fog_Map()
{
	//Erasing the map
	if (w)
	{
		delete[] map[0];
	}
	delete[] map;
}

void Fog_Map::DrawCircle(int _x, int _y, int radius, bool visible)
{
	//We'll check a square around the center (_x, _y) of size radius * 2 

	//We find the top left corner tile of the square
	int x = _x - radius;
	CAP(x, 1, w - 2);
	int y = _y - radius;
	CAP(y, 1, h - 2);

	//We define if we'll be making the tiles visibles or non-visibles
	uint opacityToSet = 0;
	if (visible == false)
	{
		opacityToSet = maxAlpha;
	}

	//Checking all the cells in the square with two "for"s to traverse them all
	for (; y < _y + radius; y++)
	{
		//Making sure the cell is still in the map
		if (y < h - 1)
		{
			for (; x < _x + radius && x < w - 1; x++)
			{
				if (map[x][y] != opacityToSet)
				{
					//Getting an aproximate distance from the center and comparing it to the radius to decide if it's part of the circle or not
					int distance = (x - _x)*(x - _x) + (y - _y) * (y - _y);
					if (distance < radius * radius)
					{
						map[x][y] = opacityToSet;
					}
				}
			}

		}
		//Resetting the x coordinate.  We're not reseting it in the "for" statement because we need to CAP the x values before using them.
		x = _x - radius;
		CAP(x, 1, w - 2);
	}

}

void Fog_Map::SetAll(bool visible)
{
	//We define if we'll be making the tiles visibles or non-visibles
	uint opacityToSet = 0;
	if (visible == false)
	{
		opacityToSet = maxAlpha;
	}

	//Setting all the tiles to the correspondant value
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			map[x][y] = opacityToSet;
		}
	}
}

void Fog_Map::CopyTo(Fog_Map* output)
{
	if (output == NULL)
	{
		return;
	}
	//Copying the map
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
	//Checking the requested tile is in the map
	if (x >= 0 && x < w && y >= 0 && y < h)
	{
		//This comparison defines the amount of Alpha a tile must have to decide if it's either visible or not
		if (map[x][y] < maxAlpha / 2)
		{
			return true;
		}
	}
	return false;
}

void Fog_Map::SoftenSection(int x1, int y1, int x2, int y2, float fadeRatio)
{
	//Capping the fade to healthy values
	CAP(fadeRatio, 1.3f, 100);

	//Making sure the section requested is in the map
	CAP(x1, 1, w - 2);
	CAP(x2, x1, w - 2);

	CAP(y1, 1, h - 2);
	CAP(y2, y1, h - 2);

	/*
	We'll need to go through the requested section twice:
		- First from top right to bottom left.
		- Then from bottom left to top right.
	If we didn't do this, the sections would only soften in two directions.
	To clearly see why we're doing this, comment one of the two loops and compare the fog borders.
	*/

	//From top right to bottom left
	for (int y = y1; y <= y2; y++)
	{
		for (int x = x1; x <= x2; x++)
		{
			int myAlpha = map[x][y];
			if (map[x + 1][y] > myAlpha * fadeRatio)
				myAlpha = map[x + 1][y] / fadeRatio;
			if (map[x - 1][y] > myAlpha * fadeRatio)
				myAlpha = map[x - 1][y] / fadeRatio;
			if (map[x][y + 1] > myAlpha * fadeRatio)
				myAlpha = map[x][y + 1] / fadeRatio;
			if (map[x][y - 1] > myAlpha * fadeRatio)
				myAlpha = map[x][y - 1] / fadeRatio;
			map[x][y] = myAlpha;
		}
	}

	//From bottom left to top right
	for (int y = y2; y >= y1; y--)
	{
		for (int x = x2; x >= x1; x--)
		{
			int myAlpha = map[x][y];
			if (map[x + 1][y] > myAlpha * fadeRatio)
				myAlpha = map[x + 1][y] / fadeRatio;
			if (map[x - 1][y] > myAlpha * fadeRatio)
				myAlpha = map[x - 1][y] / fadeRatio;
			if (map[x][y + 1] > myAlpha * fadeRatio)
				myAlpha = map[x][y + 1] / fadeRatio;
			if (map[x][y - 1] > myAlpha * fadeRatio)
				myAlpha = map[x][y - 1] / fadeRatio;
			map[x][y] = myAlpha;
		}
	}

}





// -------------- Module Fog of War -----------------------------------------------------------------------------

M_FogOfWar::M_FogOfWar(bool start_enabled) : j1Module(start_enabled)
{
	name.create("Fog of war");
}

// Destructor
M_FogOfWar::~M_FogOfWar()
{}


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
	minimapX = minimapY = -1;
	tileW = ceil((float)graphicalW / (float)mapW);
	tileH = ceil((float)graphicalH / (float)mapH);
	ready = true;
	globalVision = false;
	return true;
}

void M_FogOfWar::SetMinimap(int x, int y, int w, int h, int spacing)
{
	if (ready)
	{
		minimapX = x;
		minimapY = y;
		minimapTileW = ceil(((float)w / (float)maps[0]->GetWidth()) * spacing);
		minimapTileH = ceil(((float)h / (float)maps[0]->GetHeight()) * spacing);
		minimapSpacing = spacing;
	}
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
	//Cheking if the module has been SetUp
	if (ready == false || globalVision == true)
		return;

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
			(*currentMap)->SoftenSection(startX, startY, endX, endY);
			for (int y = startY; y <= endY && y < (*currentMap)->GetHeight(); y++)
			{
				for (int x = startX; x <= endX && x < (*currentMap)->GetWidth(); x++)
				{
					App->render->AddRect({ x * tileW, y * tileH, tileW, tileH }, true, 0, 0, 0, (*currentMap)->map[x][y]);
				}
			}
			/*if (minimapX != -1)
			{
				for (int y = 0; y < (*currentMap)->GetHeight(); y += minimapSpacing)
				{
					for (int x = 0; x < (*currentMap)->GetWidth(); x += minimapSpacing)
					{
						App->render->AddDebugRect({ minimapX + floor(((float)x / (float)minimapSpacing)) *minimapTileW, minimapY + floor(((float)y / (float)minimapSpacing)) * minimapTileH, minimapTileH, minimapTileH }, false, 0, 0, 0, (*currentMap)->map[x][y]);
					}
				}
			}*/
		}
	}
}

void M_FogOfWar::DrawCircle(int x, int y, uint radius, bool visible, int map)
{
	//Cheking if the module has been SetUp
	if (ready == false)
		return;
	
	int tileX = floor(x / tileW);
	int tileY = floor(y / tileH);
	int tileRadius = floor(radius / ((tileW + tileH) / 2));

	if (map >= 0)
	{
		if (map < maps.size())
		{
			if (visible)
			{
				maps[map]->DrawCircle(tileX, tileY, tileRadius, visible);
			}
			else
			{
				maps[map]->DrawCircle(tileX, tileY, tileRadius, visible);
			}
		}
	}
	else
	{
		for (int n = 0; n < maps.size(); n++)
		{
			if (visible)
			{
				maps[n]->DrawCircle(tileX, tileY, tileRadius, visible);
			}
			else
			{
				maps[n]->DrawCircle(tileX, tileY, tileRadius, visible);
			}
		}
	}

}

void M_FogOfWar::ClearMap(int map)
{
	//Cheking if the module has been SetUp
	if (ready == false)
		return;

	if (map >= 0)
	{
		if (map < maps.size())
		{
			maps[map]->SetAll(false);
		}
	}
	else
	{
		for (int n = 0; n < maps.size(); n++)
		{
			maps[n]->SetAll(false);
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
	//Cheking if the module has been SetUp
	if (ready == false)
		return false;
	if (globalVision)
		return true;

	bool ret = true;
	int tileX = floor(x / tileW);
	int tileY = floor(y / tileH);
	for (int n = 0; n < maps.size() && ret; n++)
	{
		if (maps[n]->isVisible(tileX, tileY) == false)
		{
			ret = false;
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