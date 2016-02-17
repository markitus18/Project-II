#include "UI_Element.h"

#include "j1Input.h"

#include "j1Gui.h"

#include "j1Console.h"

//////////////////////////////////////////////////////////
//														//
//					Ctrl+M+O							//
//			or suffer the consequences					//
//														//
//////////////////////////////////////////////////////////



// --------------- UI_ELEMENT --------------------------------------------------------

#pragma region UI_ELEMENT

UIElement::UIElement(UIElementType _type, int _id, int posX, int posY, int width, int heigth, SDL_Rect _collider, bool _active, uint _layer) : type(_type), active(_active), id(_id), movable(false)
{
	layer = _layer;
	localPosition.x = posX;
	localPosition.y = posY;
	localPosition.w = width;
	localPosition.h = heigth;
	lastEvent = UI_NONE;
	parent = NULL;
	collider = _collider;
	if (collider.w == 0 || collider.h == 0)
	{
		collider.x = 0;
		collider.y = 0;
		collider.w = width;
		collider.h = heigth;
	}
}

bool UIElement::Update(float dt)
{
	bool ret = true;
	if (active == true)
	{
		PersonalUpdate(dt);

		if (App->gui->debug == true)
		{
			if (App->gui->focus == this)
			{
				App->render->DrawQuad(GetColliderWorldPosition(), 50, 255, 50, 50);
			}
			else
			{
				App->render->DrawQuad(GetColliderWorldPosition(), 255, 50, 50, 50);
			}
			SDL_Rect pos = GetWorldPosition();
			App->render->DrawLine(pos.x, pos.y, pos.x + pos.w, pos.y, 100, 100, 255, 100);
			App->render->DrawLine(pos.x + pos.w, pos.y, pos.x + pos.w, pos.y + pos.h, 100, 100, 255, 100);
			App->render->DrawLine(pos.x + pos.w, pos.y + pos.h, pos.x, pos.y + pos.h, 100, 100, 255, 100);
			App->render->DrawLine(pos.x, pos.y, pos.x, pos.y + pos.h, 100, 100, 255, 100);
		}
	}
	return ret;
}

void UIElement::InputManager()
{
	if (active == true)
	{
		GUI_EVENTS currentEvent = UI_NONE;
		int mouseX, mouseY;
		int motionX, motionY;
		App->input->GetMousePosition(mouseX, mouseY);
		App->input->GetMouseMotion(motionX, motionY);
		SDL_Rect worldPos = GetColliderWorldPosition();

		if (mouseX > worldPos.x && mouseX < worldPos.x + worldPos.w && mouseY > worldPos.y && mouseY < worldPos.y + worldPos.h)
		{
			if ((lastEvent == UI_MOUSE_EXIT || lastEvent == UI_KEYBOARD_FOCUSED) && !App->input->GetMouseButtonDown(1))
			{
				currentEvent = UI_MOUSE_ENTER;
			}
		}
		else if (lastEvent != UI_KEYBOARD_FOCUSED || motionX != 0 || motionY != 0)
		{
			currentEvent = UI_MOUSE_EXIT;
		}
		if (lastEvent != UI_MOUSE_EXIT && currentEvent != UI_MOUSE_EXIT && App->input->GetMouseButtonDown(1))
		{
			currentEvent = UI_MOUSE_DOWN;
		}
		else if (lastEvent == UI_MOUSE_DOWN && !App->input->GetMouseButtonDown(1))
		{
			currentEvent = UI_MOUSE_UP;
		}
		if (lastEvent == UI_KEYBOARD_CLICK && App->input->GetKey(SDL_SCANCODE_RETURN) != KEY_REPEAT)
		{
			currentEvent = UI_KEYBOARD_FOCUSED;
		}
		else if (lastEvent == UI_KEYBOARD_FOCUSED && App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			currentEvent = UI_KEYBOARD_CLICK;
		}
		if (lastEvent == UI_KEYBOARD_CLICK || lastEvent == UI_KEYBOARD_FOCUSED)
		{
			if (currentEvent == UI_MOUSE_EXIT)
			{
				App->gui->focus = NULL;
			}
		}

		if (lastEvent != currentEvent && currentEvent != UI_NONE)
		{
			if (listeners.start != NULL)
			{
				SendEvent(currentEvent);
				if (currentEvent == UI_MOUSE_DOWN)
				{
					App->gui->focus = this;
				}
			}
			lastEvent = currentEvent;
		}
	}
}

void UIElement::SendEvent(GUI_EVENTS event)
{
	p2List_item<j1Module*>* item = listeners.start;
	while (item)
	{
		item->data->GUIEvent(this, event);
		item = item->next;
	}
}

SDL_Rect UIElement::GetWorldPosition()
{
	SDL_Rect ret = localPosition;
	if (parent)
	{
		SDL_Rect parentPos = parent->GetWorldPosition();
		ret.x += parentPos.x;
		ret.y += parentPos.y;
		return ret;
	}
	return ret;
}

SDL_Rect UIElement::GetColliderWorldPosition()

{
	SDL_Rect ret = collider;
	ret.x += localPosition.x;
	ret.y += localPosition.y;
	if (parent)
	{
		SDL_Rect parentPos = parent->GetWorldPosition();
		ret.x += parentPos.x;
		ret.y += parentPos.y;
	}
	return ret;
}

void UIElement::ForceLastEvent(GUI_EVENTS _event)
{
	if (lastEvent != _event) { SendEvent(_event);  lastEvent = _event; }
}

GUI_EVENTS UIElement::GetLastEvent()
{
	return lastEvent;
}

void UIElement::AddListener(j1Module* toAdd)
{
	listeners.add(toAdd);
}

bool UIElement::HasListeners()
{
	if (listeners.start) { return true; } return false;
}

void UIElement::SetActive(bool _active)
{
	if (active != _active)
	{
		active = _active;
		for (uint n = 0; n < childs.Count(); n++)
		{
			childs[n]->SetActive(_active);
		}
		if (active)
		{
			SendEvent(UI_ACTIVATED);
		}
		else
		{
			SendEvent(UI_DEACTIVATED);
		}
	}
}

const bool UIElement::GetActive()
{
	return active;
}

UIElementType UIElement::GetType()
{
	return type;
}

int UIElement::GetId()
{
	return id;
}

void UIElement::SetParent(UIElement* _parent)
{
	parent = _parent;
	_parent->childs.PushBack(this);
}

#pragma endregion


// --------------- UI_ANIMATION --------------------------------------------------------

#pragma region UI_ANIMATION

UIAnimatedImage::UIAnimatedImage(int _id, int x, int y, int w, int h, char* path, SDL_Rect* _rect, uint nFrames, float speed, SDL_Rect _collider) : UIImage(_id, x, y, w, h, _rect[0], path, _collider)
{
	for (uint n = 0; n < nFrames && _rect; n++, _rect++)
	{
		animation.frames.PushBack(*_rect);
	}
	animation.speed = speed;
	animation.loop = true;
}

UIAnimatedImage::UIAnimatedImage(int _id, int x, int y, int w, int h, SDL_Rect* _rect, uint nFrames, float speed, SDL_Rect _collider) : UIImage(_id, x, y, w, h, _rect[0], _collider)
{
	for (uint n = 0; n < nFrames && _rect; n++, _rect++)
	{
		animation.frames.PushBack(*_rect);
	}
	animation.speed = speed;
	animation.loop = true;
}

bool UIAnimatedImage::PersonalUpdate(float dt)
{
	SDL_Rect frame = animation.GetCurrentFrame(dt);
	rect->x = frame.x; rect->y = frame.y; rect->h = frame.h; rect->w = frame.w;

	if (animation.Finished() && lastEvent != UI_ANIMATION_END)
	{
		SendEvent(UI_ANIMATION_END);
		lastEvent = UI_ANIMATION_END;
	}

	if (!Draw())
	{
		LOG("No se pudo dibujar la textura.");
	}
	return true;
}

#pragma endregion


//--------------- UI_BUTTON --------------------------------------------------------

#pragma region UI_BUTTON

UIButton::UIButton(int _id, int x, int y, int w, int h, char* path, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect _collider) : UIElement(UI_Button, _id, x, y, w, h, _collider)
{
	texture = App->tex->Load(path);
	rect[0] = button;
	rect[1] = hover;
	rect[2] = clicked;
}

UIButton::UIButton(int _id, int x, int y, int w, int h, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect _collider) : UIElement(UI_Button, _id, x, y, w, h, _collider)
{
	texture = NULL;
	rect[0] = button;
	rect[1] = hover;
	rect[2] = clicked;
}

bool UIButton::PersonalUpdate(float dt)
{
	if (!Draw())
	{
		LOG("No se pudo dibujar la textura.");
	}
	return true;
}

bool UIButton::Draw()
{
	SDL_Rect toDraw;
	switch (lastEvent)
	{
	case UI_MOUSE_ENTER: {}
	case UI_KEYBOARD_FOCUSED: {}
	case UI_MOUSE_UP: {toDraw = rect[1]; localPosition.w = rect[1].w; localPosition.h = rect[1].h; break; }
	case UI_KEYBOARD_CLICK: {}
	case UI_MOUSE_DOWN: { toDraw = rect[2]; localPosition.w = rect[2].w; localPosition.h = rect[2].h; break; }
	default: { toDraw = rect[0]; localPosition.w = rect[0].w; localPosition.h = rect[0].h; break; }
	}
	if (texture)
	{
		App->render->Blit(texture, &GetWorldPosition(), &toDraw);
		return true;
	}
	else if (App->render->Blit(App->gui->GetAtlas(), &GetWorldPosition(), &toDraw))
	{
		return true;
	}
	return false;
}

#pragma endregion


// --------------- UI_IMAGE --------------------------------------------------------

#pragma region UI_IMAGE

UIImage::UIImage(int _id, int x, int y, int w, int h, SDL_Rect _rect, char* path, SDL_Rect _collider) : UIElement(UI_Image, _id, x, y, w, h, _collider)
{
	texture = App->tex->Load(path);
	rect = new SDL_Rect(_rect);
}

UIImage::UIImage(int _id, int x, int y, int w, int h, char* path, SDL_Rect _collider) : UIElement(UI_Image, _id, x, y, w, h, _collider)
{
	texture = App->tex->Load(path);
	rect = NULL;
}

UIImage::UIImage(int _id, int x, int y, int w, int h, SDL_Rect _rect, SDL_Rect _collider) : UIElement(UI_Image, _id, x, y, w, h, _collider)
{
	texture = NULL;
	rect = new SDL_Rect(_rect);
}

UIImage::~UIImage()
{
	RELEASE(rect);
}


bool UIImage::PersonalUpdate(float dt)
{
	if (!Draw())
	{
		LOG("No se pudo dibujar la textura.");
	}
	return true;
}

bool UIImage::Draw()
{
	if (texture)
	{
		App->render->Blit(texture, &GetWorldPosition(), rect);
		return true;
	}
	else if (App->render->Blit(App->gui->GetAtlas(), &GetWorldPosition(), rect))
	{
		return true;
	}
	return false;
}

#pragma endregion


// --------------- UI_RECTANGLE --------------------------------------------------------

#pragma region UI_RECTANGLE

UIRect::UIRect(int _id, int x, int y, int w, int h, uint r, uint g, uint b, uint a, SDL_Rect _collider) : UIElement(UI_Rect, _id, x, y, w, h, _collider)
{
	R = r;
	G = g;
	B = b;
	A = a;
}

UIRect::~UIRect()
{
}


bool UIRect::PersonalUpdate(float dt)
{
	if (!Draw())
	{
		LOG("No se pudo dibujar la textura.");
	}
	return true;
}

bool UIRect::Draw()
{
	App->render->DrawQuad(GetWorldPosition(), R, G, B, A);
	return true;
}

#pragma endregion



// --------------- UI_LABEL --------------------------------------------------------

#pragma region UI_LABEL

UILabel::UILabel(int _id, int x, int y, int w, int h, char* _text, UI_LabelAlineation _alineation, _TTF_Font* _typo, SDL_Rect _collider) : UIElement(UI_Label, _id, x, y, w, h, _collider)
{
	text = _text;
	typo = _typo;
	SetText(text);
	alineation = _alineation;
	R = B = G = 255;
}

bool UILabel::PersonalUpdate(float dt)
{
	if (!Draw())
	{
		LOG("No se pudo dibujar la textura.");
	}
	return true;
}

bool UILabel::Draw()
{
	if (texture)
	{
		App->render->Blit(texture, &GetWorldPosition());
		return true;
	}
	return false;
}

bool UILabel::SetText(char* _text, int _R, int _G, int _B)
{
	if (_R == -1) { _R = R; }
	if (_G == -1) { _G = G; }
	if (_B == -1) { _B = B; }
	text = _text;
	if (texture)
	{
		App->tex->UnLoad(texture);
	}
	if (text != "")
	{
		texture = App->font->Print(_text, SDL_Color{ _R, _G, _B }, typo);
	}
	else
	{
		texture = App->font->Print(" ", SDL_Color{ _R, _G, _B }, typo);
	}
	if (texture)
	{
		return true;
	}
	return false;
}

bool UILabel::SetText(p2SString _text, int _R, int _G, int _B)
{
	if (_R == -1) { _R = R; }
	if (_G == -1) { _G = G; }
	if (_B == -1) { _B = B; }
	text = _text;
	if (texture)
	{
		App->tex->UnLoad(texture);
	}
	if (text != "")
	{
		texture = App->font->Print(_text.GetString(), SDL_Color{ _R, _G, _B }, typo);
	}
	else
	{
		texture = App->font->Print(" ", SDL_Color{ _R, _G, _B }, typo);
	}
	if (texture)
	{
		return true;
	}
	return false;
}

void UILabel::SetAlineation(UI_LabelAlineation _alineation)
{
	if (alineation != _alineation)
	{
		switch (alineation)
		{
		case UI_AlignLeft:
		{
			if (_alineation == UI_AlignCenter) { localPosition.x += localPosition.w / 2; }
			if (_alineation == UI_AlignRight) { localPosition.x += localPosition.w; }
			break;
		}
		case UI_AlignCenter:
		{
			if (_alineation == UI_AlignLeft) { localPosition.x -= localPosition.w / 2; }
			if (_alineation == UI_AlignRight) { localPosition.x += localPosition.w / 2; }
			break;
		}
		case UI_AlignRight:
		{
			if (_alineation == UI_AlignLeft) { localPosition.x -= localPosition.w; }
			if (_alineation == UI_AlignCenter) { localPosition.x -= localPosition.w / 2; }
			break;
		}
		}

	}
	alineation = _alineation;
}

#pragma endregion


// --------------- UI_INPUT_BOX --------------------------------------------------------

#pragma region UI_INPUT_BOX

UIInputBox::UIInputBox(int _id, int x, int y, int w, int h, SDL_Rect _button, SDL_Rect focused, char* _text, int _margin, UI_LabelAlineation alineation, _TTF_Font* typo, SDL_Rect _collider) : UIElement(UI_InputBox, _id, x, y, w, h, _collider), label(_id, 0, 0, 0, 0, _text, alineation, typo), button(_id, 0, 0, 0, 0, _button, focused, focused)
{
	margin = _margin;
	inputEnabled = false;
	cursorPosition = 0;
	label.SetParent(this);
	button.SetParent(this);
	keyTimer.Start();

	textString = new char[maxCharacters];
	
	int textW, textH;
	SDL_QueryTexture(label.GetTexture(), NULL, NULL, &textW, &textH);

	label.localPosition.y = button.GetCurrentRect(0).h / 2 - textH / 2;
	label.localPosition.x = margin;

	SDL_Rect labelPos = label.GetWorldPosition();
	cursor.x = 0;
	cursor.y = 0;
	cursor.h = textH;
	cursor.w = CURSOR_WIDTH;

	label.SetText(label.GetText(), 150, 150, 150);

}

bool UIInputBox::PersonalUpdate(float dt)
{
	button.PersonalUpdate(dt);
	button.ForceLastEvent(lastEvent);
	label.PersonalUpdate(dt);
	label.ForceLastEvent(lastEvent);

	if (App->gui->focus == this)
	{
		if (!inputEnabled)
		{
			App->input->EnableTextInput();
			inputEnabled = true;
			drawCursor = true;
			cursorTimer.Start();
		}
		ManageWritting();
		ManageCursor();

		if (drawCursor)
		{
			if (!Draw())
			{
				LOG("No se pudo dibujar la textura.");
			}
		}
	}
	else if (inputEnabled)
	{
		inputEnabled = false;
	}

	return true;
}

bool UIInputBox::Draw()
{
	App->render->DrawQuad(cursor, 255, 255, 255);
	return true;
}

void UIInputBox::ManageCursor()
{
	if (cursorTimer.ReadSec() > 1)
	{
		cursorTimer.Start();
		drawCursor = !drawCursor;
	}
	if (cursorPosition < 0) { cursorPosition = 0; }
	else if (cursorPosition > text.Count()) { cursorPosition = text.Count(); }
	if (drawCursor)
	{
		//CURSOR PLACEMENT
		SDL_Rect labelPos = label.GetWorldPosition();
		cursor.x = labelPos.x;
		cursor.y = labelPos.y;

		p2SString string = textString;
		string.Cut(cursorPosition, string.Length() + 2);
		int w, h;
		App->font->CalcSize(string.GetString(), w, h, label.GetFont());
		cursor.x += w;
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT && keyTimer.ReadSec() > 0.25f))
	{
		cursorPosition++;
		drawCursor = true;
		cursorTimer.Start();
		keyTimer.Start();
	}
	else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT && keyTimer.ReadSec() > 0.25f))
	{
		cursorPosition--;
		drawCursor = true;
		cursorTimer.Start();
		keyTimer.Start();
	}
}

void UIInputBox::ManageWritting()
{
	//EDITING TEXT
	bool modified = false;
	bool added = false;

	if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN || (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_REPEAT && keyTimer.ReadSec() > KEY_REPEAT_DELAY))
	{
		if (cursorPosition >= 1 && text.Count() > 0)
		{
			for (uint n = cursorPosition - 1; n < text.Count() - 1; n++)
			{
				text[n] = text[n + 1];
			}
			char erased;
			text.Pop(erased);
			cursorPosition--;
			modified = true;
			drawCursor = true;
			cursorTimer.Start();
			keyTimer.Start();
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN || (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_REPEAT && keyTimer.ReadSec() > KEY_REPEAT_DELAY))
	{
		if (cursorPosition <= text.Count() - 1 && text.Count() > 0)
		{
			for (uint n = cursorPosition; n < text.Count() - 1; n++)
			{
				text[n] = text[n + 1];
			}
			char erased;
			text.Pop(erased);
			modified = true;
			drawCursor = true;
			cursorTimer.Start();
			keyTimer.Start();
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_HOME) == KEY_DOWN)
	{
		cursorPosition = 0;
		drawCursor = true;
		cursorTimer.Start();
	}
	if (App->input->GetKey(SDL_SCANCODE_END) == KEY_DOWN)
	{
		cursorPosition = text.Count();
		drawCursor = true;
		cursorTimer.Start();
	}

	if (text.Count() < maxCharacters)
	{
		const char* inputToAdd = App->input->textInput.GetString();
		for (uint n = 0; n < App->input->textInput.Length(); n++)
		{
			text.Insert(inputToAdd[n], cursorPosition);
			cursorPosition++;
			modified = true;
			added = true;
		}
	}
	if (modified)
	{
		uint n = 0;
		for (; n < text.Count() && n < maxCharacters; n++)
		{
			if (!hideText || (n == cursorPosition-1 && added) )
			{
				textString[n] = text[n];
			}
			else
			{
				textString[n] = '*';
			}
		}
		textString[n] = '\0';

		label.SetText(textString);

		SendEvent(UI_TEXT_CHANGED);
	}

}

void UIInputBox::Clear()
{
	while (text.Count() > 0)
	{
		text.Clear();
		cursorPosition = 0;
		drawCursor = true;
		cursorTimer.Start();
		keyTimer.Start();

		label.SetText("");
		SendEvent(UI_TEXT_CHANGED);
	}
}

#pragma endregion


// --------------- UI_COLLAPSE --------------------------------------------------------

#pragma region UI_COLLAPSE

UICollapse::UICollapse(int _id, int x, int y, int w, int h, UIElement* toColapse, SDL_Rect closed, SDL_Rect open, SDL_Rect _collider) : UIElement(UI_Collapse, _id, x, y, w, h, _collider)
{
	linkedElement = toColapse;
	images[0] = open;
	images[1] = closed;
}

bool UICollapse::PersonalUpdate(float dt)
{
	if (linkedElement->GetActive())
	{
		App->render->Blit(App->gui->GetAtlas(), &GetWorldPosition(), &images[0]);
	}
	else
	{
		App->render->Blit(App->gui->GetAtlas(), &GetWorldPosition(), &images[1]);
	}
	if (lastEvent == UI_MOUSE_DOWN && changed == false)
	{
		linkedElement->SetActive(!linkedElement->GetActive());
		changed = true;
	}
	if (lastEvent != UI_MOUSE_DOWN)
	{
		changed = false;
	}

	return true;
}

#pragma endregion


// --------------- UI_PROGRESS_BAR --------------------------------------------------------

#pragma region UI_PROGRESS_BAR

UIProgressBar::UIProgressBar(int _id, int x, int y, int w, int h, SDL_Rect _image, int* _maxData, int* _currentData) : UIElement(UI_ProgressBar, _id, x, y, w, h), label(_id, _image.w / 2, _image.h / 2 - 17, w, h, "0/0", UI_AlignCenter)
{
	image = _image;
	maxData = _maxData;
	currentData = _currentData;
	label.SetParent(this);
}

bool UIProgressBar::PersonalUpdate(float dt)
{
	char buf[46];
	sprintf_s(buf, sizeof(char) * 46, "%i  /  %i\0", *currentData, *maxData);



	label.SetText(buf);

	SDL_Rect toDraw = image;
	toDraw.w *= ((float)*currentData / (float)*maxData);
	
	App->render->Blit(App->gui->GetAtlas(), GetWorldPosition().x, GetWorldPosition().y, &toDraw);
	label.Draw();


	return true;
}

#pragma endregion



// --------------- UI_SLIDER --------------------------------------------------------

#pragma region UI_SLIDER

UISlider::UISlider(int _id, int x, int y, int w, int h, SDL_Rect _bar, UIImage* _thumb, int _leftOffest, int _rightOfset, SDL_Rect _barCollider) : UIElement(UI_Slider, _id, x, y, w, h), bar(_id, 0, 0, w, h, _bar, _barCollider)
{
	rightOffset = _rightOfset;
	leftOffset = _leftOffest;
	collider = _barCollider;
	thumb = _thumb;
	bar.SetParent(this);
	thumb->SetParent(this);
}

bool UISlider::PersonalUpdate(float dt)
{
	bar.Update(dt);
	float startingValor = valor;		
	
	//Dragging thumb with mouse
	if (thumb->GetLastEvent() == UI_MOUSE_DOWN)
	{
		int x, y;
		App->input->GetMouseMotion(x, y);
		thumb->localPosition.x += x;
	}
	if (App->gui->focus == this)
	{
		//Move thumb with RIGHT / LEFT keys
		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			thumb->localPosition.x += ceil(100.0f * dt);
		}
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			thumb->localPosition.x -= ceil(100.0f * dt);
		}
		//Move thumb when clicking on the bar
		if (lastEvent == UI_MOUSE_DOWN)
		{
			int x, thumbCenter;
			App->input->GetMousePosition(x, thumbCenter);
			thumbCenter = thumb->GetWorldPosition().x + thumb->collider.w / 2;
			//Don't move the thumb if the distance to it's center it's too small
			if (abs(x - thumbCenter) > 3)
			{
				if (x < thumbCenter)
				{
					thumb->localPosition.x -= ceil(100.0f * dt);
				}
				else if (x > thumbCenter)
				{
					thumb->localPosition.x += ceil(100.0f * dt);
				}
			}
		}
	}

	//Limiting thumb movement
	if (thumb->localPosition.x + thumb->collider.w / 2 < collider.x + leftOffset)
	{
		thumb->localPosition.x = collider.x + leftOffset - thumb->collider.w / 2;
	}
	else if (thumb->localPosition.x + thumb->collider.w / 2 > collider.x + collider.w - rightOffset)
	{
		thumb->localPosition.x = collider.x + collider.w - rightOffset - thumb->collider.w / 2;
	}

	//Getting the valor from 0 to 1
	float thumbPosition = thumb->localPosition.x + thumb->collider.w / 2.0f - leftOffset;
	float barLength = float(collider.w - leftOffset - rightOffset);
	valor = thumbPosition / barLength;
	valor -= 0.01f;
	if (valor < 0) { valor = 0.0f; }
	if (valor > 1) { valor = 1.0f; }
	//If it has changed, send an event
	if (valor != startingValor)
	{
		SendEvent(UI_SLIDER_VALUE_CHANGED);
	}
	return true;
}
bool UISlider::Draw()
{
	
	return true;
}

#pragma endregion