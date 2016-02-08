#ifndef __j1SceneMap_H__
#define __j1SceneMap_H__

#include "j1Module.h"
#include "j1Console.h"

struct SDL_Texture;

class UIImage;
class UILabel;
class UIButton;
class UIInputText;
class UICheckingButton;
class UIScrollBar;

class j1SceneMap : public j1Module
{

public:

	j1SceneMap(bool);

	// Destructor
	virtual ~j1SceneMap();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void ManageInput(float dt);

	void LoadGUI();
	void OnGUI(UI_Event _event, UIElement* _element);

	bool SaveDrag(pugi::xml_node) const;
	bool LoadDrag(pugi::xml_node);

public:
	int currentTile_x;
	int currentTile_y;

	bool startTileExists;
	bool endTileExists;
	iPoint startTile;
	iPoint endTile;

	bool renderInstructions = false;

private:

	SDL_Texture* debug_tex = NULL;

#pragma region Commands

	struct C_SaveGame : public Command
	{
		C_SaveGame() : Command("save_game", "Save current game", 0, NULL, "Scene"){}
		void function(const p2DynArray<p2SString>* arg);
	};
	C_SaveGame c_SaveGame;

	struct C_LoadGame : public Command
	{
		C_LoadGame() : Command("load_game", "Load current game", 0, NULL, "Scene"){}
		void function(const p2DynArray<p2SString>* arg);
	};
	C_LoadGame c_LoadGame;

#pragma endregion
};

#endif // __j1SceneMap_H__