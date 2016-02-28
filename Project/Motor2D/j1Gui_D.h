#ifndef __j1Gui_D_H__
#define __j1Gui_D_H__

#include "j1Module.h"
#include "j1App.h"

#include "p2Point.h"
#include "p2SString.h"

#include "UI_D_Element.h"

#define N_GUI_LAYERS 3

// ---------------------------------------------------
class j1Gui_D : public j1Module
{
public:

	j1Gui_D(bool);

	// Destructor
	virtual ~j1Gui_D();

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

	void OnGui(UI_D_Element* element, GUI_EVENTS event);
	// Factory Methods -----------------

	/*
	Load an image and draw a section of it.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Path: Path of the file to load.
	--rect: Section of the file to draw.
	Returns a pointer to the created image
	*/
	UI_D_Image* CreateUI_D_Image(SDL_Rect position, char* path, SDL_Rect rect, SDL_Rect collider);

	/*
	Load an image and draw it whole.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Path: Path of the file to load.
	Returns a pointer to the created image
	*/
	UI_D_Image* CreateUI_D_Image(SDL_Rect position, char* path, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Draw a section of the atlas.
	--Position: Position on screen. If width or height are 0, size won't change.
	--rect: Section of the file to draw.
	Returns a pointer to the created image
	*/
	UI_D_Image* CreateUI_D_Image(SDL_Rect position, SDL_Texture* texture, SDL_Rect rect, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Create a rectangle.
	--Position: Position on screen.
	--R,G,B: Color.
	--A: Alpha.
	Returns a pointer to the created rectangle.
	*/
	UI_D_Rect* CreateUI_D_Rect(SDL_Rect position, uint r = 255, uint g = 255, uint b = 255, uint a = 255, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Load an image and draw diferent sections depending on mouse interaction.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Path: Path of the file to load.
	--Button: Section of the file drawn normally.
	--Hover: Section of the file drawn on mouse collision.
	--Clicked: Section of the file drawn when clicked.
	Width and Height of the sections it's recommended to be the same.
	Returns a pointer to the created button.
	*/
	UI_D_Button* CreateUI_D_Button(SDL_Rect position, char* path, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Draw diferent sections of the atlas on mouse interaction.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Button: Section of the file drawn normally.
	--Hover: Section of the file drawn on mouse collision.
	--Clicked: Section of the file drawn when clicked.
	Width and Height of the sections it's recommended to be the same.
	Returns a pointer to the created button.
	*/
	UI_D_Button* CreateUI_D_Button(SDL_Rect position, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Load an image and draw an animation through diferents sections of it.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Path: Path of the file to load.
	--Rect[]: Array of sections that will constitute each frame.
	--nFrames: number of frames loaded.
	--Speed: Velocity of the animation.
	Returns a pointer to the created animation.
	*/
	UI_D_AnimatedImage* CreateUI_D_AnimatedImage(SDL_Rect position, char* path, SDL_Rect rect[], uint nFrames, float _speed, SDL_Rect collider = { 0, 0, 0, 0 });
	/*
	Draw an animation through diferents sections of the atlas.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Rect[]: Array of sections that will constitute each frame.
	--nFrames: number of frames loaded.
	--Speed: Velocity of the animation.
	Returns a pointer to the created animation.
	*/
	UI_D_AnimatedImage* CreateUI_D_AnimatedImage(SDL_Rect position, SDL_Rect rect[], uint nFrames, float _speed, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Print a text.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Text: String to print.
	--Alineation: alineation of the text.
	--FontPath: Path to the font to load.
	--FontSize: Size of the font.
	Returns a pointer to the created label.
	*/
	UI_D_Label* CreateUI_D_Label(SDL_Rect position, char* text, char* fontPath, int fontSize = 20, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Print a text.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Text: String to print.
	--Alineation: alineation of the text.
	--Typo: Font that will be used.
	Returns a pointer to the created label.
	*/
	UI_D_Label* CreateUI_D_Label(SDL_Rect position, char* text, _TTF_Font* typo = NULL, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Create a button than will activate and deactivate an element when clicked.
	--Position: Position on screen. If width or height are 0, size won't change.
	--ToLink: Element to activate/deactivate. UICollapse will be set as that object's parent.
	--Collapsed: Section of the atlas shown while item is deactivated.
	--Collapsed: Section of the atlas shown while item is activated.
	*/
	UI_D_Collapse* CreateUI_D_Collapse(SDL_Rect position, UI_D_Element* toLink, SDL_Rect collapsed, SDL_Rect opened, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Create an image that will reveal relatively to a pair of numbers.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Image: Section of the atlas to show on 100% completion.
	--CurrentData / MaxData: Numbers which relationship determine the % of the bar shown.
	¡¡¡Warning!!!
	This can take up to 0.2/3 ms per frame.
	*/
	UI_D_ProgressBar* CreateUI_D_ProgressBar(SDL_Rect position, SDL_Texture* texture, int* maxData, int* currentData, SDL_Rect image = { 0, 0, 0, 0 });
	UI_D_ProgressBar* CreateUI_D_ProgressBar(SDL_Rect position, char* path, int* maxData, int* currentData, SDL_Rect image = { 0, 0, 0, 0 });
	// Gui creation functions

	UI_D_InputText* CreateUI_D_InputText(int x, int y, char* _defaultText, SDL_Rect collider,int offsetX = 0, int offsetY = 0);

	SDL_Texture* GetAtlas() const;
	void SendNewInput(char* text);
	//Properties
	bool debug = false;
	bool inputEnabled = false;
	p2List<UI_D_Element*> UI_D_Elements;

	UI_D_Element* focus;

private:
	SDL_Texture* atlas;
	p2SString atlas_file_name;
};

#endif // __j1Gui_D_H__