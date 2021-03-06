#include "M_Map.h"
#include "j1App.h"
#include "M_Render.h"
#include "M_FileSystem.h"
#include "M_Textures.h"
#include "M_InputManager.h"
#include "M_Console.h"

M_Map::M_Map(bool start_enabled) : j1Module(start_enabled), map_loaded(false)
{
	name.create("map");
}

// Destructor
M_Map::~M_Map()
{}

// Called before render is available
bool M_Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	App->console->AddCommand(&c_Map_Render);
	App->console->AddCommand(&c_Map_Debug);

	return ret;
}

// Called before the first frame
bool M_Map::Start()
{
	return true;
}

// Called every frame
bool M_Map::Update(float dt)
{
	return true;
}

void M_Map::Draw()
{
	if (map_loaded == false)
		return;

	App->render->CapCamera();
	std::vector<MapLayer*>::iterator layer = data.layers.begin();
	for (uint i = 0; i < data.layers.size() && layer != data.layers.end(); i++)
	{
		if ((*layer)->properties.GetProperty("Draw") != 0)
		{
			iPoint start = App->render->ScreenToWorld(0, 0);
			start.x /= data.tile_width;
			start.y /= data.tile_height;
			CAP(start.x, 0, data.width - 1);
			CAP(start.y, 0, data.height - 1);

			int endY = start.y + (App->render->camera.h / App->events->GetScale() / (data.tile_height)) + 1;
			int endX = start.x + (App->render->camera.w / App->events->GetScale() / (data.tile_width)) + 2;

			for (int y = start.y; y < endY && y < data.width; ++y)
			{
				for (int x = start.x; x < endX && x < data.height; ++x)
				{
					int tile_id = (*layer)->Get(x, y);
					if (tile_id > 0)
					{
						TileSet* tileset = GetTilesetFromTileId(tile_id);

						if (tileset != NULL)
						{
							SDL_Rect r = tileset->GetTileRect(tile_id);
							iPoint pos = MapToWorld(x, y);
							SDL_Rect tmp = { pos.x, pos.y, tileset->tile_width, tileset->tile_height };
							App->render->Blit(tileset->texture, &tmp, true, &r, SDL_FLIP_NONE, SDL_Color{ (255), (255), (255), ((*layer)->opacity) });
						}
					}
				}
			}
		}
		layer++;
	}
}

TileSet* M_Map::GetTilesetFromTileId(int id) const
{
	std::vector<TileSet*>::const_iterator set = data.tilesets.begin();
	while (id > (*set)->firstgid + (*set)->tileCount - 1 && set != data.tilesets.end())
	{
		set++;
	}

	return (*set);
}

iPoint M_Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint M_Map::WorldToMap(int x, int y) const
{
	iPoint ret(0, 0);

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

Tile* TileSet::GetTileFromId(int tileId) const
{
	Tile* tile = NULL;
	bool ret = false;
	for (int i = 0; i < tileCount && !ret; i++)
	{
		if (tileData[i].id == tileId)
		{
			tile = &tileData[i];
			ret = true;
		}
	}

	return tile;
}

// Called before quitting
bool M_Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	std::vector<TileSet*>::iterator item;
	item = data.tilesets.begin();

	while (item != data.tilesets.end())
	{
		RELEASE_ARRAY((*item)->tileData);

		RELEASE((*item));
		item++;
	}
	data.tilesets.clear();

	// Remove all layers
	std::vector<MapLayer*>::iterator item2;
	item2 = data.layers.begin();

	while (item2 != data.layers.end())
	{
		RELEASE((*item2));
		item2++;
	}
	data.layers.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool M_Map::Load(const char* file_name)
{
	bool ret = true;
	C_String tmp("%s%s", folder.GetString(), file_name);

	char* buf;
	int size = App->fs->Load(tmp.GetString(), &buf);
	pugi::xml_parse_result result = map_file.load_buffer(buf, size);



	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}
	else
		RELEASE_ARRAY(buf);	

	// Load general info ----------------------------------------------
	if (ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for (tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if (ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if (ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.push_back(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for (layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if (ret == true)
			data.layers.push_back(lay);
	}

	if (ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		std::vector<TileSet*>::iterator item = data.tilesets.begin();
		while (item != data.tilesets.end())
		{
			TileSet* s = (*item);
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item++;
		}

		std::vector<MapLayer*>::iterator item_layer = data.layers.begin();
		while (item_layer != data.layers.end())
		{
			MapLayer* l = (*item_layer);
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer++;
		}
	}

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool M_Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if (map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();

		C_String orientation(map.attribute("orientation").as_string());

		if (orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if (orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if (orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool M_Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->tileCount = tileset_node.attribute("tilecount").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	LoadTilesetProperties(tileset_node, set);

	if (offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool M_Map::LoadTilesetProperties(pugi::xml_node& tileset_node, TileSet* set)
{
	//TODO
	//This isn't erased and it's causing leaks
	set->tileData = new Tile[set->tileCount];
	

	int i = 0;
	for (pugi::xml_node tile = tileset_node.child("tile"); tile; tile = tile.next_sibling("tile"))
	{
		set->tileData[i].id = tile.attribute("id").as_int() + set->firstgid;
		pugi::xml_node atr;
		for (atr = tile.child("properties").child("property"); atr; atr = atr.next_sibling("property"))
		{
			set->tileData[i].properties.names.push_back(atr.attribute("name").as_string());
			int value = atr.attribute("value").as_int();
			set->tileData[i].properties.values.PushBack(atr.attribute("value").as_int());
		}
		i++;
	}

	return true;
}

bool M_Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if (image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if (set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if (set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool M_Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	LoadProperties(node, layer->properties);
	pugi::xml_node layer_data = node.child("data");

	if (layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for (pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;
}

// Load a group of properties from a node and fill a list with it
bool M_Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = true;

	pugi::xml_node atr;
	for (atr = node.child("properties").child("property"); atr; atr = atr.next_sibling("property"))
	{
		properties.names.push_back(atr.attribute("name").as_string());
		properties.values.PushBack(atr.attribute("value").as_int());
	}
	return ret;
}

void M_Map::ChangeTile(int x, int y, int id)
{
	if (x >= 0 && x < data.height && y >= 0 && y < data.width)
	{
		std::vector<MapLayer*>::iterator layer = data.layers.begin();
		layer++;
		(*layer)->Set(x, y, id);
	}

}

#pragma region Commands
void M_Map::C_Map_Render::function(const C_DynArray<C_String>* arg)
{
	if (arg->Count() > 1)
	{
		C_String str = arg->At(1)->GetString();
		if (str == "enable")
		{
			App->map->data.layers.front()->properties.SetProperty("Draw", 1);
			LOG("-- Map: render mode enabled");
		}
		else if (str == "disable")
		{
			App->map->data.layers.front()->properties.SetProperty("Draw", 0);
			LOG("-- Map: render mode disabled");
		}
		else
			LOG("map_render: unexpected command '%s', expecting enable / disable", arg->At(1)->GetString());
	}
	else
		LOG("'%s': not enough arguments, expecting enable / disable", arg->At(0)->GetString());
}

void M_Map::C_Map_Debug::function(const C_DynArray<C_String>* arg)
{
	if (arg->Count() > 1)
	{
		std::vector<MapLayer*>::iterator layer = App->map->data.layers.begin();
		layer++;


		C_String str = arg->At(1)->GetString();
		if (str == "enable")
		{
			(*layer)->properties.SetProperty("Draw", 1);
			LOG("-- Map: debug mode enabled");
		}
		else if (str == "disable")
		{
			(*layer)->properties.SetProperty("Draw", 0);
			LOG("-- Map: debug mode disabled");
		}
		else
			LOG("map_debug: unexpected command '%s', expecting enable / disable", arg->At(1)->GetString());
	}
	else
		LOG("'%s': not enough arguments, expecting enable / disable", arg->At(0)->GetString());
}

iPoint M_Map::GetMapSize()
{
	return { data.width * data.tile_width, data.height * data.tile_height };
}

iPoint M_Map::GetMapSizeScaled()
{
	return { data.width * data.tile_width * 2, data.height * data.tile_height * 2 };
}

#pragma endregion
