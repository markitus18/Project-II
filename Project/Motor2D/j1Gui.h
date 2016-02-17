#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "j1App.h"

#include "UI_Element.h"

#define N_GUI_LAYERS 3

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

	//Update
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	bool Load(pugi::xml_node& data);

	bool Save(pugi::xml_node& data) const;


	void KeyboardFocus();

	// Factory Methods -----------------

	/*
	Load an image and draw a section of it.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Path: Path of the file to load.
	--rect: Section of the file to draw.
	Returns a pointer to the created image
	*/
	UIImage* CreateUIImage(SDL_Rect position, char* path, SDL_Rect rect, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Load an image and draw it whole.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Path: Path of the file to load.
	Returns a pointer to the created image
	*/
	UIImage* CreateUIImage(SDL_Rect position, char* path, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Draw a section of the atlas.
	--Position: Position on screen. If width or height are 0, size won't change.
	--rect: Section of the file to draw.
	Returns a pointer to the created image
	*/
	UIImage* CreateUIImage(SDL_Rect position, SDL_Rect rect, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Create a rectangle.
	--Position: Position on screen.
	--R,G,B: Color.
	--A: Alpha.
	Returns a pointer to the created rectangle.
	*/
	UIRect* CreateUIRect(SDL_Rect position, uint r = 255, uint g = 255, uint b = 255, uint a = 255, SDL_Rect collider = { 0, 0, 0, 0 });

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
	UIButton* CreateUIButton(SDL_Rect position, char* path, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Draw diferent sections of the atlas on mouse interaction.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Button: Section of the file drawn normally.
	--Hover: Section of the file drawn on mouse collision.
	--Clicked: Section of the file drawn when clicked.
	Width and Height of the sections it's recommended to be the same.
	Returns a pointer to the created button.
	*/
	UIButton* CreateUIButton(SDL_Rect position, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Load an image and draw an animation through diferents sections of it.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Path: Path of the file to load.
	--Rect[]: Array of sections that will constitute each frame.
	--nFrames: number of frames loaded.
	--Speed: Velocity of the animation.
	Returns a pointer to the created animation.
	*/
	UIAnimatedImage* CreateUIAnimatedImage(SDL_Rect position, char* path, SDL_Rect rect[], uint nFrames, float _speed, SDL_Rect collider = { 0, 0, 0, 0 });
	/*
	Draw an animation through diferents sections of the atlas.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Rect[]: Array of sections that will constitute each frame.
	--nFrames: number of frames loaded.
	--Speed: Velocity of the animation.
	Returns a pointer to the created animation.
	*/
	UIAnimatedImage* CreateUIAnimatedImage(SDL_Rect position, SDL_Rect rect[], uint nFrames, float _speed, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Print a text.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Text: String to print.
	--Alineation: alineation of the text.
	--FontPath: Path to the font to load.
	--FontSize: Size of the font.
	Returns a pointer to the created label.
	*/
	UILabel* CreateUILabel(SDL_Rect position, char* text, char* fontPath = "", int fontSize = 20, UI_LabelAlineation _alineation = UI_AlignLeft, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Print a text.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Text: String to print.
	--Alineation: alineation of the text.
	--Typo: Font that will be used.
	Returns a pointer to the created label.
	*/
	UILabel* CreateUILabel(SDL_Rect position, char* text, UI_LabelAlineation _alineation, _TTF_Font* typo = NULL, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Create a button and an editable label.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Button: Standard section of the atlas to show.
	--Focused: Section of the atlas to show while focused and/or writting.
	--Text: Text displayed before any editing is done.
	--Margin: X displacement from the left side of the button.
	--Typo: Typography that will be used.
	*/
	UIInputBox* CreateUIInputBox(SDL_Rect position, SDL_Rect button, SDL_Rect focused, char* text = "", int margin = 0, UI_LabelAlineation _alineation = UI_AlignLeft, _TTF_Font* typo = NULL, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Create a button than will activate and deactivate an element when clicked.
	--Position: Position on screen. If width or height are 0, size won't change.
	--ToLink: Element to activate/deactivate. UICollapse will be set as that object's parent.
	--Collapsed: Section of the atlas shown while item is deactivated.
	--Collapsed: Section of the atlas shown while item is activated.
	*/
	UICollapse* CreateUICollapse(SDL_Rect position, UIElement* toLink, SDL_Rect collapsed, SDL_Rect opened, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Create an image that will reveal relatively to a pair of numbers.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Image: Section of the atlas to show on 100% completion.
	--CurrentData / MaxData: Numbers which relationship determine the % of the bar shown.
	¡¡¡Warning!!!
	This can take up to 0.2/3 ms per frame.
	*/
	UIProgressBar* CreateUIProgressBar(SDL_Rect position, SDL_Rect image, int* maxData, int* currentData);

	/*
	Create a slider that returs a value between 0 and 1.
	--Position: Position on screen. If width or height are 0, size won't change.
	--Listener: Module that will be hearing value changes and any interaction with the slider.
	--Bar: Section of the atlas to display as the sliding bar.
	--Thumb: Section of the atlas to display as the thumb.
	--Right/Left Offset: Distance from the limits of the collider where the thumb won't go in.
	--Thumb offset: thumb height offset.
	--Colliders: Interactable zone of each element, relative to the object x, y position.
	*/
	UISlider* j1Gui::CreateUISlider(SDL_Rect position, j1Module* listener, SDL_Rect bar, SDL_Rect thumb, int _leftOffest = 0, int _rightOfset = 0, int _thumbOffset = 0, SDL_Rect _barCollider = { 0, 0, 0, 0 }, SDL_Rect _thumbCollider = { 0, 0, 0, 0 });

	// Gui creation functions

	SDL_Texture* GetAtlas() const;

	//Properties
	bool debug = false;
	p2List<UIElement*> UIElements;
	p2List<UIElement*> keyboardFocuseable;

	UIElement* focus;

private:
	SDL_Texture* atlas;
	p2SString atlas_file_name;
};

#endif // __j1GUI_H__