#ifndef __UI__ELEMENT__
#define __UI__ELEMENT__

#include "j1Module.h"


#include "M_Fonts.h"
#include "M_Textures.h"


#define KEY_REPEAT_DELAY 0.1f
enum Building_Type;
class Order;
/*--------------------------------Basic UI_ class--------------------------------*/

class UI_Element
{
protected:
	bool active;

	UI_Element*				parent;
	std::list<UI_Element*>	childs;

	std::list<j1Module*>	listeners;
	GUI_EVENTS				lastEvent;

public:

	C_Sprite		sprite;
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
	const bool IsActive() const;
	SDL_Rect GetWorldPosition() const;
	SDL_Rect GetColliderWorldPosition() const;
	GUI_EVENTS GetLastEvent() const;

	//Setters
	virtual void SetActive(bool _active);
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
	std::string text = "";
	_TTF_Font* typo = NULL;
	int R = 255;
	int G = 255;
	int B = 255;

public:
	//Constructor
	UI_Label(int x, int y, int w, int h, char* _text, _TTF_Font* _typo = NULL, SDL_Rect _collider = { 0, 0, 0, 0 });

	//Destructor
	~UI_Label();

	//Methods
	bool PersonalUpdate(float dt);

	bool Draw();
	bool SetText(const char* _text, int _R = -1, int _G = -1, int _B = -1);
	std::string GetText() { return text; }
	SDL_Texture* GetTexture() { return sprite.texture; }
	_TTF_Font* GetFont() { return typo; }
	void SetColor(int _R, int _G, int _B) { R = _R; G = _G; B = _B; SetText(text.c_str(), R, G ,B); }

	void UpdateSprite();
};



/*--------------------------------Image--------------------------------*/

class UI_Image : public UI_Element
{
	//Attributes
protected:
//	SDL_Rect rect;

public:
	//Constructor
	UI_Image(int x, int y, int w, int h, SDL_Texture* _texture, SDL_Rect _rect, SDL_Rect _collider = { 0, 0, 0, 0 });
	UI_Image(int x, int y, int w, int h, SDL_Texture* _texture, SDL_Rect _collider = { 0, 0, 0, 0 });

	//Destructor
	~UI_Image();

	//Methods
	void SetRect(const SDL_Rect& );

	SDL_Rect getRect();

	bool PersonalUpdate(float dt);
	bool Draw();
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

	bool PersonalUpdate(float dt);

	bool Draw();

	const SDL_Texture* GetBackTexture() { return sprite.texture; }

	SDL_Rect GetCurrentRect(int index) { if (index >= 0 && index < 4){ return rect[index]; } return{ 0, 0, 0, 0 }; }

	void OnEvent(GUI_EVENTS event);
	void SetActive(bool _active);
	void SetHoverImage(UI_Image* image, bool setAsParent = false);
	void SetRequiresImage(UI_Image*);
	//Use this method when initializing a button that needs a build
	void InitRequiredBuilding(Building_Type, bool = false);
	void changeStateOnBuilding(Building_Type, bool = false);
private:
	//Use this when the required building has been built
	void SetEnabled(bool);
public:

	bool enabled = true;

	Building_Type required_build;
	//UI_Rect* avaliable = NULL;

	UI_Image* hoverImage = NULL;
	
	UI_Image* requiresImage = NULL;

	UI_Image* son;

	Order* order;

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
	bool PersonalUpdate(float dt);
	bool Draw();
	SDL_Texture* GetTexture() { return sprite.texture; }
	SDL_Rect GetCurrentRect(int index) { if (index >= 0 && index < 4){ return rect[index]; } return{ 0, 0, 0, 0 }; }

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
protected:
	int* maxData;
	int* currentData;
public:
	SDL_Rect rect;
public:
	
	UI_ProgressBar(int x, int y, int w, int h, SDL_Texture* texture, SDL_Rect _image, int* _maxData, int* _currentData);
	bool PersonalUpdate(float dt);

	SDL_Texture* GetTexture();
	void SetTexture(SDL_Texture* text);
	void SetRect(SDL_Rect _rect);
	void SetPointers(int* _maxData, int* _currentData)
	{
		maxData = _maxData;
		currentData = _currentData;
	}
};

class UI_ProgressBar_F : public UI_Element
{
protected:
	float* maxData;
	float* currentData;
public:
	SDL_Rect rect;
public:

	UI_ProgressBar_F(int x, int y, int w, int h, SDL_Texture* texture, SDL_Rect _image, float* _maxData, float* _currentData);
	bool PersonalUpdate(float dt);

	SDL_Texture* GetTexture();
	void SetTexture(SDL_Texture* text);
	void SetRect(SDL_Rect _rect);
	void SetPointers(float* _maxData, float* _currentData)
	{
		maxData = _maxData;
		currentData = _currentData;
	}
};

class UI_HPBar : public UI_Element
{
private:
	int* currHP;
	int* maxHP;
	int* currShield;
	int* maxShield;

	C_Sprite shield;
	C_Sprite back;

public:
	SDL_Rect rect;

public:
	UI_HPBar(int x, int y, int w, int h, SDL_Texture* hp, SDL_Texture* shield, SDL_Texture* back, int* _currHP, int* _maxHP, int* _currShield, int* _shield);
	bool PersonalUpdate(float dt);
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

	std::string			defaultText = "";
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