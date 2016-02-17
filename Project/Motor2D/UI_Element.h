#ifndef __UI_ELEMENT__
#define __UI_ELEMENT__

#include "j1Module.h"
#include "j1App.h"

#include "j1Animation.h"
#include "j1Timer.h"

#include "j1Fonts.h"
#include "j1Textures.h"
#include "j1Render.h"

#include "p2DynArray.h"

#define KEY_REPEAT_DELAY 0.1f


/*--------------------------------Basic UI class--------------------------------*/

class UIElement
{
protected:
	bool active;
	UIElementType type;
	int id;

	UIElement* parent;
	p2DynArray<UIElement*> childs;

	p2List<j1Module*> listeners;
	GUI_EVENTS lastEvent;
public:
	int layer;
	bool movable;
	SDL_Rect localPosition;
	SDL_Rect collider;

public:
	//Constructor
	UIElement(UIElementType _type, int _id, int posX, int posY, int width, int heigth, SDL_Rect _collider = { 0, 0, 0, 0 }, bool _active = true, uint _layer = 0);

	//Destructor
	~UIElement()
	{}

	bool Update(float dt);
	void InputManager();

	void AddListener(j1Module* toAdd);
	bool HasListeners();

	//Getters
	const bool GetActive();
	SDL_Rect GetWorldPosition();
	SDL_Rect GetColliderWorldPosition();
	UIElementType GetType();
	GUI_EVENTS GetLastEvent();
	int GetId();

	//Setters
	void SetActive(bool _active);
	void SetParent(UIElement* _parent);

	void ForceLastEvent(GUI_EVENTS _event);

private:
	virtual bool PersonalUpdate(float dt) { return true;  }

protected:
	void SendEvent(GUI_EVENTS event);
};



/*--------------------------------Rectangle--------------------------------*/

class UIRect : public UIElement
{
	//Attributes
protected:
	uint R;
	uint G;
	uint B;
	uint A;

public:
	//Constructor
	UIRect(int _id, int x, int y, int w, int h, uint r = 255, uint g = 255, uint b = 255, uint a = 255, SDL_Rect _collider = { 0, 0, 0, 0 });
	//Destructor
	~UIRect();

	//Methods
	virtual bool PersonalUpdate(float dt);
	virtual bool Draw();

};




/*--------------------------------Text on screen--------------------------------*/

class UILabel : public UIElement
{
	//Attributes
private:
	p2SString text;
	SDL_Texture* texture;
	UI_LabelAlineation alineation;
	_TTF_Font* typo = NULL;
	int R = 255;
	int G = 255;
	int B = 255;

public:
	//Constructor
	UILabel(int _id, int x, int y, int w, int h, char* _text, UI_LabelAlineation _alineation = UI_AlignLeft, _TTF_Font* _typo = NULL, SDL_Rect _collider = { 0, 0, 0, 0 });

	//Destructor
	~UILabel()
	{}

	//Methods
	bool PersonalUpdate(float dt);

	bool Draw();
	bool SetText(char* _text, int _R = -1, int _G = -1, int _B = -1);
	bool SetText(p2SString _text, int _R = -1, int _G = -1, int _B = -1);
	void SetAlineation(UI_LabelAlineation _alineation);
	p2SString GetText() { return text; }
	UI_LabelAlineation GetAlineation() { return alineation; }
	SDL_Texture* GetTexture() { return texture; }
	_TTF_Font* GetFont() { return typo; }
	void SetColor(int _R, int _G, int _B) { R = _R; G = _G; B = _B; SetText(text, R, G ,B); }

};



/*--------------------------------Image--------------------------------*/

class UIImage : public UIElement
{
	//Attributes
protected:
	SDL_Texture* texture;
	SDL_Rect* rect;

public:
	//Constructor
	UIImage(int _id, int x, int y, int w, int h, SDL_Rect _rect, char* path, SDL_Rect _collider = { 0, 0, 0, 0 });
	UIImage(int _id, int x, int y, int w, int h, char* path, SDL_Rect _collider = { 0, 0, 0, 0 });
	UIImage(int _id, int x, int y, int w, int h, SDL_Rect _rect, SDL_Rect _collider = { 0, 0, 0, 0 });
	//Destructor
	~UIImage();

	//Methods
	virtual bool PersonalUpdate(float dt);
	virtual bool Draw();
	SDL_Texture* GetTexture() { return texture; }

};



/*--------------------------------Three state button--------------------------------*/

class UIButton : public UIElement
{
	//Attributes
private:
	SDL_Texture* texture;
	SDL_Rect rect[3];

public:
	//Constructor
	UIButton(int _id, int x, int y, int w, int h, char* path, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect _collider = { 0, 0, 0, 0 });
	UIButton(int _id, int x, int y, int w, int h, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect _collider = { 0, 0, 0, 0 });
	//Destructor
	~UIButton(){}

	//Methods
	virtual bool PersonalUpdate(float dt);
	virtual bool Draw();
	SDL_Texture* GetTexture() { return texture; }
	SDL_Rect GetCurrentRect(int index) { if (index >= 0 && index < 4){ return rect[index]; } return{ 0, 0, 0, 0 }; }

};



/*--------------------------------Animation--------------------------------*/

class UIAnimatedImage : public UIImage
{
	//Attributes
private:
	Animation animation;

public:
	//Constructor
	UIAnimatedImage(int _id, int x, int y, int w, int h, char* path, SDL_Rect* _rect, uint nFrames, float speed, SDL_Rect _collider = { 0, 0, 0, 0 });
	UIAnimatedImage(int _id, int x, int y, int w, int h, SDL_Rect* _rect, uint nFrames, float speed, SDL_Rect _collider = { 0, 0, 0, 0 });
	//Destructor
	~UIAnimatedImage(){}

	//Methods
	bool PersonalUpdate(float dt);

};



/*--------------------------------Text input + Button associated with it--------------------------------*/

class UIInputBox : public UIElement
{
	//Attributes
private:
	UIButton button;
	p2DynArray<char> text;
	char* textString = NULL;

	SDL_Rect cursor;
	int margin;
	uint cursorPosition;
	bool inputEnabled;

	j1Timer cursorTimer;
	j1Timer keyTimer;
	bool drawCursor = true;
public:
	UILabel label;
	bool hideText = false;
	uint maxCharacters = 28;

	//Constructor
	UIInputBox(int _id, int x, int y, int w, int h, SDL_Rect _button, SDL_Rect focused, char* _text, int _margin, UI_LabelAlineation alineation = UI_AlignLeft, _TTF_Font* typo = NULL, SDL_Rect _collider = { 0, 0, 0, 0 });
	//Destructor
	~UIInputBox()
	{
		if (textString) { delete[] textString; }
	}

	//Methods
	bool PersonalUpdate(float dt);
	void Clear();
private:
	bool Draw();

	void ManageWritting();
	void ManageCursor();

};



/*--------------------------------Core for a collapsable window--------------------------------*/

class UICollapse : public UIElement
{
private:
	UIElement* linkedElement;
	SDL_Rect images[2];
	bool changed = false;
public:
	UICollapse(int _id, int x, int y, int w, int h, UIElement* toColapse, SDL_Rect closed, SDL_Rect open, SDL_Rect _collider = { 0, 0, 0, 0 });

	bool PersonalUpdate(float dt);
};



/*--------------------------------Progress bar--------------------------------*/

class UIProgressBar : public UIElement
{
	SDL_Rect image;
	int* maxData;
	int* currentData;
	UILabel label;
public:
	int labelX;
	
	UIProgressBar(int _id, int x, int y, int w, int h, SDL_Rect _image, int* _maxData, int* _currentData);

	bool PersonalUpdate(float dt);
};



/*--------------------------------Slider--------------------------------*/

class UISlider : public UIElement
{
	int leftOffset = 0;
	int rightOffset = 0;
	UIImage bar;
	UIImage* thumb;
	float valor = 0.0f;

public:

	UIImage* GetBar() { return &bar; }
	UIImage* GetThumb() { return thumb; }

	float GetValor() { return valor; }

	UISlider(int _id, int x, int y, int w, int h, SDL_Rect bar, UIImage* thumb, int _leftOffest = 0, int _rightOfset = 0, SDL_Rect _barCollider = { 0, 0, 0, 0 });

	bool PersonalUpdate(float dt);
	bool Draw();
};



# endif