#ifndef __j1SceneGUI_H__
#define __j1SceneGUI_H__

#include "j1Module.h"
#include "j1Console.h"

struct SDL_Texture;

class UIImage;
class UILabel;
class UIButton;
class UIInputText;
class UICheckingButton;
class UIScrollBar;

class j1SceneGUI : public j1Module
{

public:

	j1SceneGUI(bool);

	// Destructor
	virtual ~j1SceneGUI();

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

	bool CheckLoginData();
	void RegisterNewUser();
	void LoadWelcomeText();


	void UpdateValueText(float value);

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

	bool logStart;

	SDL_Texture* debug_tex = NULL;
	UIElement* elementHold = NULL;

	//GUI Elements and management
	
	UIImage* login_image = NULL;
	UILabel* login_label = NULL;
	UIButton* login_button = NULL;

	UIImage* window_image = NULL;
	UILabel* window_label = NULL;
	UIButton* window_button = NULL;

	UILabel* nickLabel = NULL;
	UIImage* nickButton_image = NULL;
	UILabel* nickButton_label = NULL;
	UIInputText* nickButton_button = NULL;

	UILabel* passwordLabel = NULL;
	UIImage* passwordButton_image = NULL;
	UILabel* passwordButton_label = NULL;
	UIInputText* passwordButton_button = NULL;

	UILabel* connectionErrorLabel = NULL;

	UILabel* registerLabel = NULL;
	bool nick_button_clicked = false;
	
	UIImage* checkingButton_defImage;
	UIImage* checkingButton_checkedImage;
	UICheckingButton* checkingButton;
	
	UILabel* welcomeLabel = NULL;
	
	UIImage* scrollBar_bar;
	UIImage* scrollBar_thumb;
	UIScrollBar* scrollBar;

	UILabel* scrollBar_value;
	
	p2DynArray<UILabel*> configLabels;

#pragma region Commands
	struct CloseGUI : public Command
	{
		CloseGUI() : Command("close_gui", "Close GUI window", 0, NULL, "Scene"){}
		void function(const p2DynArray<p2SString>* arg);
	};
	CloseGUI command_closeGUI;

	struct OpenGUI : public Command
	{
		OpenGUI() : Command("open_gui", "Open GUI window", 0, NULL, "Scene"){}
		void function(const p2DynArray<p2SString>* arg);
	};
	OpenGUI command_openGUI;

	struct MoveLabels : public Command
	{
		MoveLabels() : Command("move_labels", "Move labels from scene:", 0, NULL, "Scene"){}
		void function(const p2DynArray<p2SString>* arg);
	};
	MoveLabels move_labels;

	struct SaveLabels : public Command
	{
		SaveLabels() : Command("save_labels", "Save labels from scene:", 0, NULL, "Scene"){}
		void function(const p2DynArray<p2SString>* arg);
	};
	SaveLabels save_labels;

	struct LoadLabels : public Command
	{
		LoadLabels() : Command("load_labels", "Save labels from scene:", 0, NULL, "Scene"){}
		void function(const p2DynArray<p2SString>* arg);
	};
	LoadLabels load_labels;

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

#endif // __j1SceneGUI_H__