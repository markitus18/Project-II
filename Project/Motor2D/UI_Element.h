#ifndef __UI__ELEMENT__
#define __UI__ELEMENT__

#include "j1Module.h"


#include "C_Animation.h"
#include "M_Fonts.h"
#include "M_Textures.h"


#define KEY_REPEAT_DELAY 0.1f


/*--------------------------------Basic UI_ class--------------------------------*/

class UI_Element
{
protected:
	bool active;

	UI_Element*				parent;
	C_DynArray<UI_Element*>	childs;

	C_List<j1Module*> listeners;
	GUI_EVENTS		lastEvent;
public:
	int				layer;
	bool			movable;
	SDL_Rect		localPosition;
	SDL_Rect		collider;
	bool			useCamera = false;

public:
	//Constructor
	UI_Element(int posX, int posY, int width, int heigth, SDL_Rect _collider = { 0, 0, 0, 0 }, bool _active = true, uint _layer = 0);

	//Destructor
	~UI_Element()
	{}

	bool Update(float dt);
	void InputManager();

	void AddListener(j1Module* toAdd);
	bool HasListeners();

	//Getters
	const bool GetActive();
	SDL_Rect GetWorldPosition();
	SDL_Rect GetColliderWorldPosition();
	GUI_EVENTS GetLastEvent();

	//Setters
	void SetActive(bool _active);
	void SetParent(UI_Element* _parent);

	void ForceLastEvent(GUI_EVENTS _event);

private:
	virtual bool PersonalUpdate(float dt) { return true;  }

protected:
	void SendEvent(GUI_EVENTS event);
	virtual void OnEvent(GUI_EVENTS event){};
};



/*--------------------------------Rectangle--------------------------------*/

class UI_Rect : public UI_Element
{
	//Attributes
protected:
	uint R;
	uint G;
	uint B;
	uint A;

public:
	//Constructor
	UI_Rect(int x, int y, int w, int h, uint r = 255, uint g = 255, uint b = 255, uint a = 255, SDL_Rect _collider = { 0, 0, 0, 0 });
	//Destructor
	~UI_Rect();

	//Methods
	virtual bool PersonalUpdate(float dt);
	virtual bool Draw();
};




/*--------------------------------Text on screen--------------------------------*/

class UI_Label : public UI_Element
{
	//Attributes
private:
	C_String text;
	SDL_Texture* texture;
	_TTF_Font* typo = NULL;
	int R = 255;
	int G = 255;
	int B = 255;

public:
	//Constructor
	UI_Label(int x, int y, int w, int h, char* _text, _TTF_Font* _typo = NULL, SDL_Rect _collider = { 0, 0, 0, 0 });

	//Destructor
	~UI_Label()
	{}

	//Methods
	bool PersonalUpdate(float dt);

	bool Draw();
	bool SetText(char* _text, int _R = -1, int _G = -1, int _B = -1);
	bool SetText(C_String _text, int _R = -1, int _G = -1, int _B = -1);
	C_String GetText() { return text; }
	SDL_Texture* GetTexture() { return texture; }
	_TTF_Font* GetFont() { return typo; }
	void SetColor(int _R, int _G, int _B) { R = _R; G = _G; B = _B; SetText(text, R, G ,B); }

};



/*--------------------------------Image--------------------------------*/

class UI_Image : public UI_Element
{
	//Attributes
protected:
	SDL_Texture* texture;
	SDL_Rect* rect;

public:
	//Constructor
	UI_Image(int x, int y, int w, int h, SDL_Rect _rect, char* path, SDL_Rect _collider = { 0, 0, 0, 0 });
	UI_Image(int x, int y, int w, int h, char* path, SDL_Rect _collider = { 0, 0, 0, 0 });
	UI_Image(int x, int y, int w, int h, SDL_Texture* _texture, SDL_Rect _rect, SDL_Rect _collider = { 0, 0, 0, 0 });
	UI_Image(int x, int y, int w, int h, SDL_Texture* _texture, SDL_Rect _collider = { 0, 0, 0, 0 });
	//Destructor
	~UI_Image();

	//Methods
	virtual bool PersonalUpdate(float dt);
	virtual bool Draw();
	SDL_Texture* GetTexture() { return texture; }

};

/*--------------------------------Two state button--------------------------------*/
/*Buttons similar to the ones in starcraft*/
class UIButton2 : public UI_Element
{
public:
	//Constructors

	UIButton2(int x, int y, int w, int h, char* path, const SDL_Rect& button, const  SDL_Rect& clicked, bool _toRender = false, const SDL_Rect _collider = { 0, 0, 0, 0 });

	UIButton2(int x, int y, int w, int h, SDL_Texture* back, const SDL_Rect& button, const  SDL_Rect& clicked, bool _toRender = false, const SDL_Rect _collider = { 0, 0, 0, 0 });

	//Destructor
	~UIButton2(){}

	//Methods

	bool PersonalUpdate(float dt);

	bool Draw();

	const SDL_Texture* GetBackTexture() { return back; }

	SDL_Rect GetCurrentRect(int index) { if (index >= 0 && index < 4){ return rect[index]; } return{ 0, 0, 0, 0 }; }

public:

	bool avaliable;

private:

	//Todo: Text label description

//Order* order;

	//The image that will actually change
	SDL_Texture* back;
	SDL_Rect rect[2];
};


/*--------------------------------Three state button--------------------------------*/

class UI_Button : public UI_Element
{
	//Attributes
private:
	SDL_Texture* texture;
	SDL_Rect rect[3];

public:
	//Constructor
	UI_Button(int x, int y, int w, int h, char* path, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect _collider = { 0, 0, 0, 0 });
	UI_Button(int x, int y, int w, int h, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect _collider = { 0, 0, 0, 0 });
	//Destructor
	~UI_Button(){}

	//Methods
	virtual bool PersonalUpdate(float dt);
	virtual bool Draw();
	SDL_Texture* GetTexture() { return texture; }
	SDL_Rect GetCurrentRect(int index) { if (index >= 0 && index < 4){ return rect[index]; } return{ 0, 0, 0, 0 }; }

};



/*--------------------------------Animation--------------------------------*/

class UI_AnimatedImage : public UI_Image
{
	//Attributes
private:
	Animation animation;

public:
	//Constructor
	UI_AnimatedImage(int x, int y, int w, int h, char* path, SDL_Rect* _rect, uint nFrames, float speed, SDL_Rect _collider = { 0, 0, 0, 0 });
	UI_AnimatedImage(int x, int y, int w, int h, SDL_Rect* _rect, uint nFrames, float speed, SDL_Rect _collider = { 0, 0, 0, 0 });
	//Destructor
	~UI_AnimatedImage(){}

	//Methods
	bool PersonalUpdate(float dt);

};

/*--------------------------------Core for a collapsable window--------------------------------*/

class UI_Collapse : public UI_Element
{
private:
	UI_Element* linkedElement;
	SDL_Rect images[2];
	bool changed = false;
public:
	UI_Collapse(int x, int y, int w, int h, UI_Element* toColapse, SDL_Rect closed, SDL_Rect open, SDL_Rect _collider = { 0, 0, 0, 0 });

	bool PersonalUpdate(float dt);
};


/*--------------------------------Progress bar--------------------------------*/

class UI_ProgressBar : public UI_Element
{
	SDL_Rect rect;
	int* maxData;
	int* currentData;
	UI_Label label;
	SDL_Texture* texture;
public:
	int labelX;
	
	UI_ProgressBar(int x, int y, int w, int h, SDL_Texture* texture, SDL_Rect _image, int* _maxData, int* _currentData);
	UI_ProgressBar(int x, int y, int w, int h, char* path, SDL_Rect _image, int* _maxData, int* _currentData);

	bool PersonalUpdate(float dt);

	SDL_Texture* GetTexture();
	void SetTexture(SDL_Texture* text);
	void SetRect(SDL_Rect _rect);
};


/*--------------------------------Input Text--------------------------------*/
class UI_InputText : public UI_Element
{
public:
	UI_InputText(int x, int y, int w, int h, char* _defaultText, SDL_Rect collider, int offsetX = 0, int offsetY = 0);
	~UI_InputText();

	//Text management
	void UpdateCursorPosition();
	void RenderCursor();
	void GetNewInput(char* text);
	void DeleteCharacterOnCursor();
	void DeleteNextCharacterToCursor();
	void ManageTextInput();
	void UpdateTextTexture();
	C_String GetString() const;
	void DeleteText();

	bool PersonalUpdate(float dt);

	void OnEvent(GUI_EVENTS event);
public:

	UI_Label			text;

	char*				defaultText;
	C_List<char>		textList;
	C_List_item<char>*	currentChar = NULL;

	//Text 
	bool				defaultOn = true;
	int					maxCharacters = 30;
	bool				hiddenText = false;
	bool				textChanged = false;

	bool				cursorNeedUpdate = true;
	iPoint				cursorStart;
	int					cursorBlinkSpeed = 2;
	int					cursorPosition = 0;
};



# endif