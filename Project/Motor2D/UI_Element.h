#ifndef __UI__ELEMENT__
#define __UI__ELEMENT__

#include "j1Module.h"


#include "C_Animation.h"
#include "M_Fonts.h"
#include "M_Textures.h"


#define KEY_REPEAT_DELAY 0.1f

class Order;
/*--------------------------------Basic UI_ class--------------------------------*/

class UI_Element
{
protected:
	bool active;

	UI_Element*				parent;
	C_DynArray<UI_Element*>	childs;

	std::list<j1Module*>	listeners;
	GUI_EVENTS				lastEvent;

public:

	C_Sprite				sprite;
	bool			movable;
	SDL_Rect		localPosition;
	SDL_Rect		collider;

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
	void SetLayer(uint layer);

	void ForceLastEvent(GUI_EVENTS _event);
	void SetAsFocus();
	virtual void UpdateSprite();

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
	bool PersonalUpdate(float dt);
	bool Draw();
};




/*--------------------------------Text on screen--------------------------------*/

class UI_Label : public UI_Element
{
	//Attributes
private:
	C_String text;
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
	bool SetText(C_String _text, int _R = -1, int _G = -1, int _B = -1);
	C_String GetText() { return text; }
	SDL_Texture* GetTexture() { return sprite.texture; }
	_TTF_Font* GetFont() { return typo; }
	void SetColor(int _R, int _G, int _B) { R = _R; G = _G; B = _B; SetText(text, R, G ,B); }

	void UpdateSprite();
};



/*--------------------------------Image--------------------------------*/

class UI_Image : public UI_Element
{
	//Attributes
protected:
	SDL_Rect rect;

public:
	//Constructor
	UI_Image(int x, int y, int w, int h, SDL_Texture* _texture, SDL_Rect _rect, SDL_Rect _collider = { 0, 0, 0, 0 });
	UI_Image(int x, int y, int w, int h, SDL_Texture* _texture, SDL_Rect _collider = { 0, 0, 0, 0 });

	//Destructor
	~UI_Image();

	//Methods
	void SetRect(const SDL_Rect& );

	SDL_Rect getRect();

	virtual bool PersonalUpdate(float dt);
	virtual bool Draw();
	SDL_Texture* GetTexture() { return sprite.texture; }

};

/*--------------------------------Two state button--------------------------------*/
/*Buttons similar to the ones in starcraft*/
class UI_Button2 : public UI_Element
{
public:
	//Constructors
	UI_Button2(int x, int y, int w, int h, SDL_Texture* back, const SDL_Rect& button, const  SDL_Rect& clicked, const SDL_Rect _collider = { 0, 0, 0, 0 });

	//Destructor
	~UI_Button2();

	//Methods

	virtual bool PersonalUpdate(float dt);

	virtual bool Draw();

	const SDL_Texture* GetBackTexture() { return sprite.texture; }

	SDL_Rect GetCurrentRect(int index) { if (index >= 0 && index < 4){ return rect[index]; } return{ 0, 0, 0, 0 }; }

	void OnEvent(GUI_EVENTS event);
	
public:

	bool avaliable;
	UI_Image* son;
	Order* order;
private:
	//Todo: Text label description

	//The image that will actually change

	SDL_Rect rect[2];
};


/*--------------------------------Three state button--------------------------------*/

class UI_Button : public UI_Element
{
	//Attributes
private:
	SDL_Rect rect[3];

public:
	//Constructor
	UI_Button(int x, int y, int w, int h, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect _collider = { 0, 0, 0, 0 });
	//Destructor
	~UI_Button(){}

	//Methods
	virtual bool PersonalUpdate(float dt);
	virtual bool Draw();
	SDL_Texture* GetTexture() { return sprite.texture; }
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
	int* maxData;
	int* currentData;
	SDL_Rect rect;
public:
	
	UI_ProgressBar(int x, int y, int w, int h, SDL_Texture* texture, SDL_Rect _image, int* _maxData, int* _currentData);

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
	std::list<char>		textList;
	std::list<char>::iterator    currentChar;

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