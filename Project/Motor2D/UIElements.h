#ifndef _UIELEMENTS_H__
#define _UIELEMENTS_H__

enum UI_Event
{
	MOUSE_ENTER = 0,
	MOUSE_DOWN,
	MOUSE_UP,
	MOUSE_EXIT,
	SCROLL_CHANGE,
	RETURN_DOWN,
};

enum ScrollBar_Types
{
	VERTICAL = 0,
	HORIZONTAL,
};

class UIElement
{
public:
	UIElement();
	~UIElement();

	//UI Event Controllers
	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	virtual void OnMouseDown();
	virtual void OnMouseUp();
	virtual void OnGetFocus();
	virtual void OnLooseFocus();

	void Deactivate();
	void Activate();
	void CheckInput();

	//Position management -------
	void SetLocalPosition(int, int);
	void SetGlobalPosition(int, int);
	void SetCollider(SDL_Rect rect);
	void SetCollider(int, int, int, int);

	void Center(UIElement*);
	void Center(iPoint);
	void Center_x(UIElement*);
	void Center_x(int);
	void Center_y(UIElement*);
	void Center_y(int);

	void Align(UIElement*);
	void Align_x(UIElement*);
	void Align_y(UIElement*);
	//---------------------------

	void SetParent(UIElement* parent);
	void SetHoveringTexture(SDL_Texture*);
	void SetLayer(int lay);
	void SetIgnoreCamera();

	iPoint GetLocalPosition() const;
	SDL_Rect GetWorldRect() const;

	virtual bool Update(float dt);

public:
	p2SString name;
	bool active = true;
	bool interactive = true;
	bool dragable = false;
	int layer = 0;

	SDL_Texture* hoveringTexture;
	bool mouseWasIn = false;
	bool mouseWasClicked = false;

	bool ignoreCamera = false;
	j1Module* listener;
	UIElement* parent;
	p2DynArray<UIElement*> childs;

private:
	SDL_Rect collider;
};

//Class UIImage --------------------------------------
class UIImage : public UIElement
{
public:
	UIImage();
	UIImage(char* name, iPoint position, const SDL_Texture* newTexture, p2DynArray<SDL_Rect>& rects);
	~UIImage();

	bool Update(float dt);

public:
	p2DynArray<SDL_Rect> rects;
	const SDL_Texture* texture;
	int currentRect = 0;


};

//Class UILabel --------------------------------------
class UILabel : public UIElement
{
public:
	UILabel();
	UILabel(char* newName, iPoint newPosition, char* newText);
	~UILabel();

	void SetNewTexture(char* text, SDL_Color color = { 255, 255, 255 }, TTF_Font* font = NULL);

	void OnMouseEnter();
	void OnMouseExit();
	bool Update(float dt);

public:
	p2SString text;
	SDL_Texture* texture;
	TTF_Font* font;
	SDL_Color color;

	bool drawLine;
};

//Class UIButton -------------------------------------
class UIButton : public UIElement
{
public:
	UIButton();
	UIButton(char* name, iPoint position, UIImage* image, UILabel* label = NULL, UILabel* label2 = NULL);
	~UIButton();

	void OnMouseEnter();
	void OnMouseExit();
	void OnMouseDown();
	void OnMouseUp();

	bool Update(float dt);

public:
	UIImage*	image;
	UILabel*	label;
	UILabel*	label2;
};

//Class UIInputText ----------------------------------
class UIInputText : public UIElement
{
public:
	UIInputText();
	UIInputText(char* name, SDL_Rect rect, UIImage* image, UILabel* defaultLabel, int offsetX = 0, int offsetY = 0);
	~UIInputText();

	void OnMouseEnter();
	void OnMouseExit();
	void OnMouseDown();
	void OnMouseUp();
	void OnGetFocus();
	void OnLooseFocus();

	//Text management
	void UpdateCursorPosition();
	void RenderCursor();
	void GetNewInput(char* text);
	void DeleteCharacterOnCursor();
	void DeleteNextCharacterToCursor();
	void ManageInput();
	void UpdateTextTexture();
	char* GetString() const;
	void DeleteText();

	bool Update(float dt);

public:
	UIImage*			image;

	TTF_Font*			font;
	SDL_Texture*		text_texture;
	UILabel*			defaultText;

	char*	text;
	p2List<char>	textList;
	p2List_item<char>* currentChar = NULL;

	//Text 
	bool				defaultOn = true;
	int					maxCharacters = 30;
	bool				hiddenText = false;
	SDL_Rect			textRect;
	bool				textChanged = false;
	int					offsetX;
	int					offsetY;
	int					textStartDifference;

	int					textDisplacement;
	bool				cursorNeedUpdate = true;
	iPoint				cursorStart;
	int					cursorBlinkSpeed = 2;
	int					cursorPosition = 0;
};

class UICheckingButton : public UIElement
{
public:
	UICheckingButton();
	UICheckingButton(char* name, iPoint position, UIImage* image1, UIImage* image2);
	~UICheckingButton();

	void OnMouseDown();
	void OnMouseUp();

public:
	bool		checked = false;

	UIImage*	defImage;
	UIImage*	checkedImage;

};

//Class UI Scroll Bar
class UIScrollBar : public UIElement
{
public:
	UIScrollBar();
	UIScrollBar(char* name, iPoint position, UIElement* bar, UIElement* thumb);
	~UIScrollBar();

	bool Update(float dt);

	void OnMouseDown();
	void OnMouseUp();
	void OnMouseExit();

	void OnGetFocus();
	void OnLooseFocus();

	void CheckThumbMovement(int &, int &);
	void CheckInputMovement(int &, int &, float);
	void CheckBarMovement(int &, int &, float);

	void SetValue(float value);
	float GetValue();

public:
	UIElement*		bar;
	UIElement*		thumb;

	ScrollBar_Types type = VERTICAL;
	bool			moved = false;
	bool			thumbClicked = false;
	bool			barClicked = false;
	int				offsetL;
	int				offsetR;
	int				offsetU;
	int				offsetD;

	int				arrowsValue = 1;
	int				barValue = 1;

	int				thumbClickOffset;
};

class UIRect : public UIElement
{
public:
	UIRect();
	UIRect(char* newName, SDL_Rect newRect, int newR, int newG, int newB, int newA = 255, bool newfilled = true);
	~UIRect();

	bool Update(float dt);

public:
	int r;
	int g;
	int b;
	int a;
	bool filled;
};

class UIBar : public UIElement
{
public:
	UIBar();
	UIBar(char* newNane, UIElement* background, UIElement* fillImage, int*, int*);
	~UIBar();

	bool Update(float dt);

private:
	UIElement* background;
	UIElement* fillImage;

	int  maxW;
	int* maxValue;
	int* currValue;

public:

};
#endif //_UIELEMENTS_H__