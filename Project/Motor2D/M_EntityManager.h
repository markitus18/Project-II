#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "j1Module.h"

class Unit;
class Building;
enum Unit_Type;
enum Unit_State;
enum Building_Type;

struct UnitC_SpriteData
{
	SDL_Texture* texture;

	int size;
	float animationSpeed;

	int run_line_start;
	int run_line_end;

	int idle_line_start;
	int idle_line_end;

	int atack_line_start;
	int atack_line_end;
};

struct C_SpritesData
{
	std::vector<Unit_Type>		unitType;
	std::vector<UnitC_SpriteData> data;

	UnitC_SpriteData* GetData(Unit_Type);
	void GetStateLimits(Unit_Type type, Unit_State state, int& min, int& max);
};

class M_EntityManager : public j1Module
{
public:
	M_EntityManager(bool);
	~M_EntityManager();

	bool PreStart(pugi::xml_node& node);
	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void DoUnitLoop(float dt);
	void UpdateSelectionRect();

	void ManageInput();

	Unit* CreateUnit(int x, int y, Unit_Type);
	bool deleteUnit(std::list<Unit*>::iterator);

	bool IsUnitSelected(std::list<Unit*>::const_iterator) const;

	void SendNewPath(int x, int y);

	SDL_Texture* GetTexture(Unit_Type);
	SDL_Texture* GetTexture(Building_Type);
	void UpdateC_SpriteRect(Unit* unit, SDL_Rect& rect, SDL_RendererFlip& flip, float dt);
	void UpdateCurrentFrame(Unit* unit);
	//	bool addBuilding(Entity& _entity);
	//	bool deleteBuilding();

	void DrawDebug();
private:

	void SelectUnit(std::list<Unit*>::iterator);
	void UnselectUnit(std::list<Unit*>::iterator);

	void LoadC_SpritesData();
	//should be priv
public:
	bool continuous = true;
	bool smooth = true;

	bool selectUnits = false;

	SDL_Rect selectionRect;
	SDL_Rect groupRect;
	SDL_Rect destinationRect;

	std::list<Unit*> unitList;
	std::list<Unit*> selectedUnits;
	std::list<Unit*> unitsToDelete;

	SDL_Texture* darkT_tex;
	SDL_Texture* arbiter_tex;
	SDL_Texture* unit_base;
	SDL_Texture* path_tex;
	SDL_Texture* hpBar_empty;
	SDL_Texture* hpBar_filled;
	SDL_Texture* hpBar_mid;
	SDL_Texture* hpBar_low;

	SDL_Texture* building_base;

	//Collision variables
	int currentPriority = 1;

private:

	C_SpritesData spritesData;
	void AddUnit(Unit* unit);
	//C_List<Building*> buildingList;

};

#endif //_ENTITYMANAGER_H__