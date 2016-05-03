#ifndef __M_GUI_H__
#define __M_GUI_H__

#include "j1Module.h"

#include "C_Point.h"
#include "C_String.h"

#include "UI_Element.h"

#define N_GUI_LAYERS 3

enum SelectTypes;
enum Grid_Type;
enum Unit_Type;
enum Building_Type;
class Building;
class Grid3x3;
// ---------------------------------------------------
class M_GUI : public j1Module
{
public:

	M_GUI(bool);

	// Destructor
	virtual ~M_GUI();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	//Update
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp(); 

	void OnGui(UI_Element* element, GUI_EVENTS event);

	//void DeleteUIElement(UI_Element* element);

	/* Calls the correct destructor and delete it's
	   UI_Element from the list*/
	template<typename UI_TYPE>
	void DeleteUIElement(UI_TYPE & _element)
	{
		if (App->gui->focus == _element)
			App->gui->focus = NULL;
		UI_Elements.remove(_element);
		RELEASE(_element);
	}

	// Factory Methods -----------------
#pragma region Factory


	/*
	Draw a section of a loaded image.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Texture: Already loaded texture.
	--rect: Section of the file to draw.
	Returns a pointer to the created image
	*/
	UI_Image* CreateUI_Image(SDL_Rect position, SDL_Texture* texture, SDL_Rect rect, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Create a rectangle.
	--Position: Position on screen.
	--R,G,B: Color.
	--A: Alpha.
	Returns a pointer to the created rectangle.
	*/
	UI_Rect* CreateUI_Rect(SDL_Rect position, uint r = 255, uint g = 255, uint b = 255, uint a = 255, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Load an image and draw diferent sections depending on mouse interaction.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Path: Path of the file to load.
	--Button: Section of the file drawn normally.
	--Hover: Section of the file drawn on mouse collision.
	--Clicked: Section of the file drawn when clicked.
	Width and Height of the sections it's recommended to be the same for each section.
	Returns a pointer to the created button.
	*/
	UI_Button* CreateUI_Button(SDL_Rect position, char* path, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Draw diferent sections of the atlas on mouse interaction.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Button: Section of the file drawn normally.
	--Hover: Section of the file drawn on mouse collision.
	--Clicked: Section of the file drawn when clicked.
	Width and Height of the sections it's recommended to be the samefor each section.
	Returns a pointer to the created button.
	*/
	UI_Button* CreateUI_Button(SDL_Rect position, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Creates a 2 states button using a pre loaded texture with an icon over it
	(Most optimal method)
	- Tex: Texture of the button frame
	- Button: Rect that will be displayed normally
	- Clicked: Rect that will be displayed on click
	- toRender: Determines if this button will be rendered or not
	*/
	UI_Button2* CreateUI_Button2(const SDL_Rect& position, SDL_Texture* tex, const SDL_Rect& button, const SDL_Rect& clicked, bool _toRender = false, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Print a text.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Text: String to print.
	--Alineation: alineation of the text.
	--Typo: Font that will be used.
	Returns a pointer to the created label.
	*/
	UI_Label* CreateUI_Label(SDL_Rect position, char* text, _TTF_Font* typo = NULL, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Create a button than will activate and deactivate an element when clicked.
	--Position: Position on screen. If width or height are 0, size won't change.
	--ToLink: Element to activate/deactivate. UICollapse will be set as that object's parent.
	--Collapsed: Section of the atlas shown while item is deactivated.
	--Collapsed: Section of the atlas shown while item is activated.
	*/
	UI_Collapse* CreateUI_Collapse(SDL_Rect position, UI_Element* toLink, SDL_Rect collapsed, SDL_Rect opened, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Create an image that will reveal relatively to a pair of numbers.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Texture: Texture that will be used.
	--CurrentData / MaxData: Numbers which relationship determine the % of the bar shown.
	--Image: section of the image to show on 100% completion. Ignore to show whole image.
	¡¡¡Warning!!!
	This can take up to 0.2/3 ms per frame.
	*/
	UI_ProgressBar* CreateUI_ProgressBar(SDL_Rect position, SDL_Texture* texture, int* maxData, int* currentData, SDL_Rect image = { 0, 0, 0, 0 });

	UI_HPBar* CreateUI_HPBar(SDL_Rect position, SDL_Texture* hp_tex, SDL_Texture* shield_tex, SDL_Texture* back_tex, int* maxData, int* currentData, int* maxShield, int* currShield);
	// Gui creation functions

	/*
	Create box that can recieve inputs.
	--X/Y: position of the InputBox.
	--Default Text: Text that will be shown when no input is recieved.
	--Collider: Interactable zone.
	--OffestX/Y: Offset of the text from the 0,0 of the position of the collider.
	*/
	UI_InputText* CreateUI_InputText(int x, int y, char* _defaultText, SDL_Rect collider,int offsetX = 0, int offsetY = 0);
#pragma endregion

	bool SetCurrentGrid( Grid3x3 * newCurrent);
	bool SetCurrentGrid(Grid_Type _type);
	bool SetCurrentGrid(Unit_Type _unit, bool multiple = false);
	bool SetCurrentGrid(Building_Type _type);

	const Grid3x3* GetCurrentGrid();
	SDL_Texture* GetAtlas() const;
	void SendNewInput(char* text);

	//Production queue

	void setProductionQueue(const Building*);
	void addQueueSlot(Unit_Type);
	void removeQueueSlot(Building*);

	void UI_Unselect();
	void UI_SelectUnit(Unit_Type);
public:
	//Properties
	bool debug = false;
	bool inputEnabled = false;

	SelectTypes current_select;

	std::list<UI_Element*> UI_Elements;

	UI_Element* focus;
private:
	Grid3x3 * currentGrid;
	SDL_Texture* atlas;
	C_String atlas_file_name;
};

#endif // __M_GUI_H__
