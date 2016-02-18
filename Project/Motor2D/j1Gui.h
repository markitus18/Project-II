#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "j1Console.h"

#include "SDL_TTF\include\SDL_ttf.h"

#define CURSOR_WIDTH 1


class UIElement;
class UIImage;
class UILabel;
class UIButton;
class UIInputText;
class UICheckingButton;
class UIScrollBar;
class UIRect;
class UIBar;

enum ScrollBar_Types;

// ---------------------------------------------------
class j1Gui : public j1Module
{
public:

	j1Gui(bool);

	// Destructor
	virtual ~j1Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop
	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	UIButton* CreateScreenButton();
	UIImage* CreateImage(char* name, iPoint position, p2DynArray<SDL_Rect> rect, UIElement* parent = NULL, bool active = true, j1Module* listener = NULL, char* path = NULL, bool interactive = false);
	UILabel* CreateText(char* name, iPoint position, char* text, UIElement* parent = NULL, bool active = true, j1Module* listener = NULL, TTF_Font* font = NULL, SDL_Color color = { 255, 255, 255 }, bool interactive = false);
	UIButton* CreateButton(char* name, iPoint position, UIImage* image, UIElement* parent, bool active = true, j1Module* listener = NULL);
	UICheckingButton* CreateCheckingButton(char* name, iPoint position, UIImage* image, UIImage* image2, UIElement* parent, bool active = true, j1Module* listener = NULL);
	UIScrollBar* CreateScrollBar(char* name, iPoint position, UIElement* image, UIElement* thumb, UIElement* parent, ScrollBar_Types type, int offsetL, int offsetR, int offsetU, int offsetD, bool active = true, j1Module* listener = NULL);
	UIInputText* CreateInputText(char* name, SDL_Rect rect, UIImage* image, UILabel* label, UIElement* parent, int offsetX, int offsetY, bool active = true, j1Module* listener = NULL);
	UIRect* CreateRect(char* name, SDL_Rect rect, int newR, int newG, int newB, int newA = 255, bool newFilled = true, UIElement* parent = NULL, bool active = true, j1Module* listener = NULL);
	UIBar* CreateBar(char* newName, UIElement* background, UIElement* fillImage, int*, int*);
	void DeleteElement(UIElement* element);

	const UIElement* GetMouseHover() const;
	const SDL_Texture* GetAtlas() const;

	//Focus manager
	void SetFocus(UIElement*);
	void SetHovering(UIElement*);
	UIElement* GetFocus() const;
	UIElement* GetNextFocus();
	void DrawFocus() const;
	void ClickFocus();
	void UnclickFocus();
	void LooseFocus();

	//Functions not in use yet
	void setLastMouseIn(UIElement*);
	void quitLastMouseIn(UIElement*);

	void SendNewInput(char* text);
	void DrawCursorInput();
	void DrawHoveringImage();

	UIElement* GetScreen() const;

public:
	bool sendInput = true;
	bool focusClicked;
	bool debugMode;
	bool inputRecieved = false;
	bool inputEnabled = false;

private:
	SDL_Texture* atlas;
	p2SString atlas_file_name;

	SDL_Texture* cursorInput;
	p2SString cursorInput_file_name;
	SDL_Rect cursorInput_rect;

	p2List<UIElement*> uiElements;
	UIElement* screen_button;
	UIElement* focus;
	UIElement* hoveringElement;
	
#pragma region Commands
	struct C_UIDebug : public Command
	{
		C_UIDebug() : Command("gui_debug", "Enable / Disable GUI debug", 1, NULL, "GUI"){}
		void function(const p2DynArray<p2SString>* arg);
	};
	C_UIDebug c_UIDebug;
#pragma endregion
	
	
};

#endif // __j1GUI_H__