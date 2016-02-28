#ifndef __UI_D__ELEMENT__
#define __UI_D__ELEMENT__

#include "j1Module.h"


#include "j1Animation.h"
#include "j1Fonts.h"
#include "j1Textures.h"


#define KEY_REPEAT_DELAY 0.1f


/*--------------------------------Basic UI_D_ class--------------------------------*/

class UI_D_Element
{
protected:
	bool active;

	UI_D_Element* parent;
	p2DynArray<UI_D_Element*> childs;

	p2List<j1Module*> listeners;
	GUI_EVENTS lastEvent;
public:
	int layer;
	bool movable;
	SDL_Rect localPosition;
	SDL_Rect collider;
	bool useCamera = false;

public:
	//Constructor
	UI_D_Element(int posX, int posY, int width, int heigth, SDL_Rect _collider = { 0, 0, 0, 0 }, bool _active = true, uint _layer = 0);

	//Destructor
	~UI_D_Element()
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
	void SetParent(UI_D_Element* _parent);

	void ForceLastEvent(GUI_EVENTS _event);

private:
	virtual bool PersonalUpdate(float dt) { return true;  }

protected:
	void SendEvent(GUI_EVENTS event);
	virtual void OnEvent(GUI_EVENTS event){};
};



/*--------------------------------Rectangle--------------------------------*/

class UI_D_Rect : public UI_D_Element
{
	//Attributes
protected:
	uint R;
	uint G;
	uint B;
	uint A;

public:
	//Constructor
	UI_D_Rect(int x, int y, int w, int h, uint r = 255, uint g = 255, uint b = 255, uint a = 255, SDL_Rect _collider = { 0, 0, 0, 0 });
	//Destructor
	~UI_D_Rect();

	//Methods
	virtual bool PersonalUpdate(float dt);
	virtual bool Draw();
};




/*--------------------------------Text on screen--------------------------------*/

class UI_D_Label : public UI_D_Element
{
	//Attributes
private:
	p2SString text;
	SDL_Texture* texture;
	_TTF_Font* typo = NULL;
	int R = 255;
	int G = 255;
	int B = 255;

public:
	//Constructor
	UI_D_Label(int x, int y, int w, int h, char* _text, _TTF_Font* _typo = NULL, SDL_Rect _collider = { 0, 0, 0, 0 });

	//Destructor
	~UI_D_Label()
	{}

	//Methods
	bool PersonalUpdate(float dt);

	bool Draw();
	bool SetText(char* _text, int _R = -1, int _G = -1, int _B = -1);
	bool SetText(p2SString _text, int _R = -1, int _G = -1, int _B = -1);
	p2SString GetText() { return text; }
	SDL_Texture* GetTexture() { return texture; }
	_TTF_Font* GetFont() { return typo; }
	void SetColor(int _R, int _G, int _B) { R = _R; G = _G; B = _B; SetText(text, R, G ,B); }

};



/*--------------------------------Image--------------------------------*/

class UI_D_Image : public UI_D_Element
{
	//Attributes
protected:
	SDL_Texture* texture;
	SDL_Rect* rect;

public:
	//Constructor
	UI_D_Image(int x, int y, int w, int h, SDL_Rect _rect, char* path, SDL_Rect _collider = { 0, 0, 0, 0 });
	UI_D_Image(int x, int y, int w, int h, char* path, SDL_Rect _collider = { 0, 0, 0, 0 });
	UI_D_Image(int x, int y, int w, int h, SDL_Texture* _texture, SDL_Rect _rect, SDL_Rect _collider = { 0, 0, 0, 0 });
	UI_D_Image(int x, int y, int w, int h, SDL_Texture* _texture, SDL_Rect _collider = { 0, 0, 0, 0 });
	//Destructor
	~UI_D_Image();

	//Methods
	virtual bool PersonalUpdate(float dt);
	virtual bool Draw();
	SDL_Texture* GetTexture() { return texture; }

};



/*--------------------------------Three state button--------------------------------*/

class UI_D_Button : public UI_D_Element
{
	//Attributes
private:
	SDL_Texture* texture;
	SDL_Rect rect[3];

public:
	//Constructor
	UI_D_Button(int x, int y, int w, int h, char* path, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect _collider = { 0, 0, 0, 0 });
	UI_D_Button(int x, int y, int w, int h, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect _collider = { 0, 0, 0, 0 });
	//Destructor
	~UI_D_Button(){}

	//Methods
	virtual bool PersonalUpdate(float dt);
	virtual bool Draw();
	SDL_Texture* GetTexture() { return texture; }
	SDL_Rect GetCurrentRect(int index) { if (index >= 0 && index < 4){ return rect[index]; } return{ 0, 0, 0, 0 }; }

};



/*--------------------------------Animation--------------------------------*/

class UI_D_AnimatedImage : public UI_D_Image
{
	//Attributes
private:
	Animation animation;

public:
	//Constructor
	UI_D_AnimatedImage(int x, int y, int w, int h, char* path, SDL_Rect* _rect, uint nFrames, float speed, SDL_Rect _collider = { 0, 0, 0, 0 });
	UI_D_AnimatedImage(int x, int y, int w, int h, SDL_Rect* _rect, uint nFrames, float speed, SDL_Rect _collider = { 0, 0, 0, 0 });
	//Destructor
	~UI_D_AnimatedImage(){}

	//Methods
	bool PersonalUpdate(float dt);

};

/*--------------------------------Core for a collapsable window--------------------------------*/

class UI_D_Collapse : public UI_D_Element
{
private:
	UI_D_Element* linkedElement;
	SDL_Rect images[2];
	bool changed = false;
public:
	UI_D_Collapse(int x, int y, int w, int h, UI_D_Element* toColapse, SDL_Rect closed, SDL_Rect open, SDL_Rect _collider = { 0, 0, 0, 0 });

	bool PersonalUpdate(float dt);
};


/*--------------------------------Progress bar--------------------------------*/

class UI_D_ProgressBar : public UI_D_Element
{
	SDL_Rect rect;
	int* maxData;
	int* currentData;
	UI_D_Label label;
	SDL_Texture* texture;
public:
	int labelX;
	
	UI_D_ProgressBar(int x, int y, int w, int h, SDL_Texture* texture, SDL_Rect _image, int* _maxData, int* _currentData);
	UI_D_ProgressBar(int x, int y, int w, int h, char* path, SDL_Rect _image, int* _maxData, int* _currentData);
	bool PersonalUpdate(float dt);
	SDL_Texture* GetTexture();
	void SetRect(SDL_Rect _rect);
};

//Class UIInputText ----------------------------------
class UI_D_InputText : public UI_D_Element
{
public:
	UI_D_InputText(int x, int y, int w, int h, char* _defaultText, SDL_Rect collider, int offsetX = 0, int offsetY = 0);
	~UI_D_InputText();

	//Text management
	void UpdateCursorPosition();
	void RenderCursor();
	void GetNewInput(char* text);
	void DeleteCharacterOnCursor();
	void DeleteNextCharacterToCursor();
	void ManageTextInput();
	void UpdateTextTexture();
	p2SString GetString() const;
	void DeleteText();

	bool PersonalUpdate(float dt);

	void OnEvent(GUI_EVENTS event);
public:

	UI_D_Label			text;

	char*				defaultText;
	p2List<char>		textList;
	p2List_item<char>*	currentChar = NULL;

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