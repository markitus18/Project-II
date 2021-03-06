#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "j1Module.h"

// ----------------------------------------------------
struct Properties
{
	std::list<C_String>	names;
	C_DynArray<int>		values;

	int GetProperty(char* p)
	{
		std::list<C_String>::iterator item = names.begin();
		uint ret = 0;
		while (item != names.end())
		{
			if ((*item) == p)
			{
				return values[ret];
			}
			ret++;
			item++;
		}
		return 0;
	}

	void SetProperty(char* p, int newValue)
	{
		std::list<C_String>::iterator item = names.begin();
		uint ret = 0;
		while (item != names.end())
		{
			if ((*item) == p)
			{
				values[ret] = newValue;
			}
			ret++;
			item++;
		}
	}
};

// ----------------------------------------------------
struct MapLayer
{
	C_String	name;
	int			width;
	int			height;
	uint*		data;
	Properties	properties;
	uint		opacity = 255;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		delete[] data;
	//		RELEASE_ARRAY(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y*width) + x];
	}

	inline void Set(int x, int y, int newId)
	{
		if (y*width + x >= 0 && y*width + x <= width*height)
			data[y*width + x] = newId;
	}
};
// ----------------------------------------------------
struct Tile
{
	int id;
	Properties properties;
};

struct TileSet
{
	SDL_Rect GetTileRect(int id) const;
	Tile* GetTileFromId(int tileId) const;

	C_String			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	int					tileCount;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
	Tile*				tileData;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	MapTypes			type;
	std::vector<TileSet*>	tilesets;
	std::vector<MapLayer*>layers;
};

// ----------------------------------------------------
class M_Map : public j1Module
{
public:

	M_Map(bool);

	// Destructor
	virtual ~M_Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before first frame
	bool Start();
	
	// Called every frame
	bool Update(float dt);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;

	void ChangeTile(int x, int y, int id);
	TileSet* GetTilesetFromTileId(int id) const;

	iPoint GetMapSize();
	iPoint GetMapSizeScaled();

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetProperties(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);



public:

	MapData data;

private:

	pugi::xml_document	map_file;
	C_String			folder;
	bool				map_loaded;

#pragma region Commands
	struct C_Map_Render : public Command
	{
		C_Map_Render() : Command("map_render", "Enable / Disable map render", 1, "map_r", "Map"){}
		void function(const C_DynArray<C_String>* arg);
	};
	C_Map_Render c_Map_Render;

	struct C_Map_Debug : public Command
	{
		C_Map_Debug() : Command("map_debug", "Enable / Disable map debug", 1, "map_d", "Map"){}
		void function(const C_DynArray<C_String>* arg);
	};
	C_Map_Debug c_Map_Debug;
};

#endif // __j1MAP_H__