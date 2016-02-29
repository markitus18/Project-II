/*
class UIElement
{
public:

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

	void SetCamera(bool set);

public:
	C_String name;
	bool active = true;
	bool interactive = true;

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
	bool filled;
};
*/