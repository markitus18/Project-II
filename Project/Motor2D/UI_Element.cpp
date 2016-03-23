#include "UI_Element.h"

#include "M_Input.h"

#include "M_GUI.h"
#include "M_Render.h"
#include "j1App.h"

#include "SDL_ttf\include\SDL_ttf.h"



//////////////////////////////////////////////////////////
//														//
//					Ctrl+M+O							//
//			or suffer the consequences					//
//														//
//////////////////////////////////////////////////////////



// --------------- UI__ELEMENT --------------------------------------------------------

#pragma region UI__ELEMENT

UI_Element::UI_Element(int posX, int posY, int width, int heigth, SDL_Rect _collider, bool _active, uint _layer) : active(_active), movable(false)
{
	sprite.layer = _layer;
	localPosition = { posX, posY, width, heigth };
	lastEvent = UI_NONE;
	parent = NULL;
	collider = _collider;
	if (collider.w == 0 || collider.h == 0)
	{
		collider = { 0, 0, width, heigth };
	}
}

bool UI_Element::Update(float dt)
{
	bool ret = true;
	if (active == true)
	{
		PersonalUpdate(dt);

		if (App->gui->debug == true)
		{
			if (App->gui->focus == this)
			{
				App->render->AddRect(GetColliderWorldPosition(), sprite.useCamera, 50, 255, 50, 50);
			}
			else
			{
				App->render->AddRect(GetColliderWorldPosition(), sprite.useCamera, 255, 50, 50, 50);
			}
			SDL_Rect pos = GetWorldPosition();
			App->render->AddRect(pos, sprite.useCamera, 100, 100, 255, 100);
		}
	}
	return ret;
}

void UI_Element::InputManager()
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
		if (App->gui->focus == this && App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			SendEvent(UI_KEYBOARD_CLICK);
		}
		/*if (lastEvent == UI_KEYBOARD_CLICK || lastEvent == UI_KEYBOARD_FOCUSED)
		{
			if (currentEvent == UI_MOUSE_EXIT)
			{
				App->gui->focus = NULL;
			}
		}*/

		if (lastEvent != currentEvent && currentEvent != UI_NONE)
		{

			if (!listeners.empty())
			{
				SendEvent(currentEvent);
				OnEvent(currentEvent);
				if (currentEvent == UI_MOUSE_DOWN)
				{
					if (App->gui->focus != this)
					{
						SendEvent(UI_GET_FOCUS);
						OnEvent(UI_GET_FOCUS);
						if (App->gui->focus)
						{
							App->gui->focus->ForceLastEvent(UI_LOST_FOCUS);
						}
					}
					App->gui->focus = this;
				}
			}
			lastEvent = currentEvent;
		}

		if (App->gui->focus == this && lastEvent == UI_MOUSE_EXIT && App->input->GetMouseButtonDown(1))
		{
			App->gui->focus = NULL;
			SendEvent(UI_LOST_FOCUS);
			OnEvent(UI_LOST_FOCUS);
		}
	}
}

void UI_Element::SendEvent(GUI_EVENTS event)
{
	std::list<j1Module*>::iterator item;
	item = listeners.begin();

	while (item != listeners.end())
	{
		(*item)->OnGUI(event, this);
		item++;
	}
}

SDL_Rect UI_Element::GetWorldPosition()
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

SDL_Rect UI_Element::GetColliderWorldPosition()

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

void UI_Element::ForceLastEvent(GUI_EVENTS _event)
{
	if (lastEvent != _event) { SendEvent(_event);  OnEvent(_event); lastEvent = _event; }
}

void UI_Element::UpdateSprite()
{
	sprite.position = GetWorldPosition();
}

GUI_EVENTS UI_Element::GetLastEvent()
{
	return lastEvent;
}

void UI_Element::AddListener(j1Module* toAdd)
{
	listeners.push_back(toAdd);
}

bool UI_Element::HasListeners()
{
	return !listeners.empty();
}

void UI_Element::SetActive(bool _active)
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
			SendEvent(UIEACTIVATED);
		}
	}
}

const bool UI_Element::GetActive()
{
	return active;
}

void UI_Element::SetParent(UI_Element* _parent)
{
	parent = _parent;
	_parent->childs.PushBack(this);
	UpdateSprite();
}

#pragma endregion


// --------------- UI_ANIMATION --------------------------------------------------------

#pragma region UI__ANIMATION

UI_AnimatedImage::UI_AnimatedImage(int x, int y, int w, int h, char* path, SDL_Rect* _rect, uint nFrames, float speed, SDL_Rect _collider) : UI_Image(x, y, w, h, _rect[0], path, _collider)
{
	for (uint n = 0; n < nFrames && _rect; n++, _rect++)
	{
		animation.frames.PushBack(*_rect);
	}
	animation.speed = speed;
	animation.loop = true;
}

UI_AnimatedImage::UI_AnimatedImage(int x, int y, int w, int h, SDL_Rect* _rect, uint nFrames, float speed, SDL_Rect _collider) : UI_Image(x, y, w, h, App->gui->GetAtlas(), _rect[0], _collider)
{
	for (uint n = 0; n < nFrames && _rect; n++, _rect++)
	{
		animation.frames.PushBack(*_rect);
	}
	animation.speed = speed;
	animation.loop = true;
}

bool UI_AnimatedImage::PersonalUpdate(float dt)
{
	SDL_Rect frame = animation.GetCurrentFrame(dt);
	rect.x = frame.x; rect.y = frame.y; rect.h = frame.h; rect.w = frame.w;

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


//--------------- UI__BUTTON --------------------------------------------------------

#pragma region UI__BUTTON

UI_Button::UI_Button(int x, int y, int w, int h, char* path, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect _collider) : UI_Element(x, y, w, h, _collider)
{
	sprite.texture = App->tex->Load(path);
	sprite.section = rect[0] = button;
	rect[1] = hover;
	rect[2] = clicked;
}

UI_Button::UI_Button(int x, int y, int w, int h, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect _collider) : UI_Element(x, y, w, h, _collider)
{
	sprite.texture = NULL;
	sprite.section = rect[0] = button;
	rect[0] = button;
	rect[1] = hover;
	rect[2] = clicked;
}

bool UI_Button::PersonalUpdate(float dt)
{
	if (!Draw())
	{
		LOG("No se pudo dibujar la textura.");
	}
	return true;
}

bool UI_Button::Draw()
{
	switch (lastEvent)
	{
	case UI_MOUSE_ENTER: {}
	case UI_KEYBOARD_FOCUSED: {}
	case UI_MOUSE_UP: {sprite.section = rect[1]; localPosition.w = rect[1].w; localPosition.h = rect[1].h; break; }
	case UI_KEYBOARD_CLICK: {}
	case UI_MOUSE_DOWN: { sprite.section = rect[2]; localPosition.w = rect[2].w; localPosition.h = rect[2].h; break; }
	default: { sprite.section = rect[0]; localPosition.w = rect[0].w; localPosition.h = rect[0].h; break; }
	}
	if (sprite.texture)
	{
		App->render->AddSprite(&sprite, GUI);
		return true;
	}
	return false;
}

#pragma endregion


//--------------- UI_BUTTON2 --------------------------------------------------------

#pragma region UI_BUTTON_2

UI_Button2::UI_Button2(int x, int y, int w, int h, char* path, const SDL_Rect& button, const SDL_Rect& clicked, const SDL_Rect _collider) : UI_Element(x, y, w, h, _collider)
{
	sprite.texture = App->tex->Load(path);
	rect[0] = button;
	rect[1] = clicked;

	avaliable = true;

	//order = NULL;
}

UI_Button2::UI_Button2(int x, int y, int w, int h, SDL_Texture* _buttons, const SDL_Rect& button, const  SDL_Rect& clicked, const SDL_Rect _collider) : UI_Element(x, y, w, h, _collider)
{
	sprite.texture = _buttons;
	rect[0] = button;
	rect[1] = clicked;

	avaliable = true;

	//order = NULL;
}

UI_Button2::~UI_Button2()
{

}

bool UI_Button2::PersonalUpdate(float dt)
{
	if (!Draw())
	{
		LOG("Could not draw button!");
		return false;
	}
	else
		return true;
}

bool UI_Button2::Draw()
{
	bool ret = true;

	//Set the rect to draw, then draw the back and then the UI image
	if (avaliable)
	{
		switch (lastEvent)
		{
		case UI_MOUSE_UP:
		{
			sprite.section = rect[0];
			localPosition.w = rect[0].w;
			localPosition.h = rect[0].h;
			break;
		}
		case UI_MOUSE_DOWN:
		{
			sprite.section = rect[1];
			localPosition.w = rect[1].w;
			localPosition.h = rect[1].h;
			break;
		}
		default: { sprite.section = rect[0]; localPosition.w = rect[0].w; localPosition.h = rect[0].h; break; }
		}
	}
	if (sprite.texture)
	{
		App->render->AddSprite(&sprite, GUI);
		ret = true;
	}
	else
		ret = false;
	return ret;
}
#pragma endregion


// --------------- UI_IMAGE --------------------------------------------------------

#pragma region UI__IMAGE

UI_Image::UI_Image(int x, int y, int w, int h, SDL_Rect _rect, char* path, SDL_Rect _collider) : UI_Element(x, y, w, h, _collider)
{
	sprite.texture = App->tex->Load(path);
	sprite.section = rect;
	sprite.position = GetWorldPosition();
}

UI_Image::UI_Image(int x, int y, int w, int h, char* path, SDL_Rect _collider) : UI_Element(x, y, w, h, _collider)
{
	sprite.texture = App->tex->Load(path);
	sprite.section = rect;
	sprite.position = GetWorldPosition();
}

UI_Image::UI_Image(int x, int y, int w, int h, SDL_Texture* _texture, SDL_Rect _rect, SDL_Rect _collider) : UI_Element(x, y, w, h, _collider)
{
	sprite.texture = _texture;
	sprite.section = _rect;
	sprite.position = GetWorldPosition();
}

UI_Image::UI_Image(int x, int y, int w, int h, SDL_Texture* _texture, SDL_Rect _collider) : UI_Element(x, y, w, h, _collider)
{
	sprite.texture = _texture;
	sprite.section = rect;
	sprite.position = GetWorldPosition();
}

UI_Image::~UI_Image(){};

bool UI_Image::PersonalUpdate(float dt)
{
	if (!Draw())
	{
		LOG("Could not draw image texture");
	}
	return true;
}

bool UI_Image::Draw()
{
	if (sprite.texture)
	{
		App->render->AddSprite(&sprite, GUI);
		return true;
	}
	return false;
}

#pragma endregion


// --------------- UI__RECTANGLE --------------------------------------------------------

#pragma region UI__RECTANGLE

UI_Rect::UI_Rect(int x, int y, int w, int h, uint r, uint g, uint b, uint a, SDL_Rect _collider) : UI_Element(x, y, w, h, _collider)
{
	R = r;
	G = g;
	B = b;
	A = a;
}

UI_Rect::~UI_Rect()
{
}


bool UI_Rect::PersonalUpdate(float dt)
{
	if (!Draw())
	{
		LOG("No se pudo dibujar la textura.");
	}
	return true;
}

bool UI_Rect::Draw()
{
	App->render->AddRect(GetWorldPosition(), sprite.useCamera, R, G, B, A);
	return true;
}

#pragma endregion



// --------------- UI_LABEL --------------------------------------------------------
#pragma region UI__LABEL

UI_Label::UI_Label(int x, int y, int w, int h, char* _text, _TTF_Font* _typo, SDL_Rect _collider) : UI_Element(x, y, w, h, _collider)
{
	text = _text;
	typo = _typo;
	if (typo == NULL)
	{
		typo = App->font->GetDefaultFont();
	}
	SetText(text);
	R = B = G = 255;
}

bool UI_Label::PersonalUpdate(float dt)
{
	if (!Draw())
	{
		LOG("Unable to draw texture");
	}
	return true;
}

bool UI_Label::Draw()
{
	if (sprite.texture)
	{
		App->render->AddSprite(&sprite, GUI);
		return true;
	}
	return false;
}

bool UI_Label::SetText(C_String _text, int _R, int _G, int _B)
{
	if (_R == -1) { _R = R; }
	if (_G == -1) { _G = G; }
	if (_B == -1) { _B = B; }
	text = _text;
	if (sprite.texture)
	{
		App->tex->UnLoad(sprite.texture);
	}
	if (text != "")
	{
		sprite.texture = App->font->Print(_text.GetString(), SDL_Color{ _R, _G, _B }, typo);
	}
	else
	{
		sprite.texture = App->font->Print(" ", SDL_Color{ _R, _G, _B }, typo);
	}
	if (sprite.texture)
	{
		return true;
	}
	return false;
}

void UI_Label::UpdateSprite()
{
	sprite.position = GetWorldPosition();
}

#pragma endregion


// --------------- UI_COLLAPSE --------------------------------------------------------

#pragma region UI__COLLAPSE

UI_Collapse::UI_Collapse(int x, int y, int w, int h, UI_Element* toColapse, SDL_Rect closed, SDL_Rect open, SDL_Rect _collider) : UI_Element(x, y, w, h, _collider)
{
	linkedElement = toColapse;
	images[0] = open;
	images[1] = closed;
}

bool UI_Collapse::PersonalUpdate(float dt)
{
	if (linkedElement->GetActive())
	{
		App->render->Blit(App->gui->GetAtlas(), &GetWorldPosition(), sprite.useCamera, &images[0]);
	}
	else
	{
		App->render->Blit(App->gui->GetAtlas(), &GetWorldPosition(), sprite.useCamera, &images[1]);
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

UI_ProgressBar::UI_ProgressBar(int x, int y, int w, int h, SDL_Texture* _texture, SDL_Rect _rect, int* _maxData, int* _currentData) : UI_Element(x, y, w, h), label(_rect.w / 2, _rect.h / 2 - 17, w, h, "0/0")
{
	texture = _texture;
	rect = _rect;
	maxData = _maxData;
	currentData = _currentData;
	label.SetParent(this);
}

UI_ProgressBar::UI_ProgressBar(int x, int y, int w, int h, char* path, SDL_Rect _rect, int* _maxData, int* _currentData) : UI_Element(x, y, w, h), label(_rect.w / 2, _rect.h / 2 - 17, w, h, "0/0")
{
	texture = App->tex->Load(path);
	rect = _rect;
	maxData = _maxData;
	currentData = _currentData;
	label.SetParent(this);
}

bool UI_ProgressBar::PersonalUpdate(float dt)
{
	char buf[46];
	sprintf_s(buf, sizeof(char) * 46, "%i  /  %i\0", *currentData, *maxData);

	label.SetText(buf);

	float ratio = ((float)*currentData / (float)*maxData);

	SDL_Rect toDraw = rect;
	toDraw.w *= ratio;


	SDL_Rect rect = GetWorldPosition();
	rect.w *= ratio;

	App->render->Blit(texture, &rect, sprite.useCamera, &toDraw);
	//label.Draw();


	return true;
}

SDL_Texture* UI_ProgressBar::GetTexture()
{
	return texture;
}

void UI_ProgressBar::SetTexture(SDL_Texture* text)
{
	texture = text;
}

void UI_ProgressBar::SetRect(SDL_Rect _rect)
{
	rect = _rect;
}
#pragma endregion


// --------------- UI_InputText --------------------------------------------------------

#pragma region UI_INPUT_Text

UI_InputText::UI_InputText(int x, int y, int w, int h, char* defText, SDL_Rect _collider, int offsetX, int offsetY) : UI_Element(x, y, w, h, _collider), text(offsetX, offsetY, w, h, defText)
{
	defaultText = defText;
	text.SetParent(this);
	currentChar = textList.end();
}

UI_InputText::~UI_InputText()
{
	textList.clear();
	delete defaultText;
}

void UI_InputText::UpdateCursorPosition()
{
	int x = 0, y = 0;
	if (cursorPosition > 0)
	{
		std::list<char>::iterator item = textList.begin();
		char* str = new char[cursorPosition + 1];
		for (int i = 0; i < cursorPosition && item != textList.end(); i++)
		{
			if (!hiddenText)
			{
				str[i] = (*item);
			}
			else
			{
				str[i] = '*';
			}
			item++;
		}
		str[cursorPosition] = '\0';

		TTF_SizeText(text.GetFont(), str, &x, &y);

		delete[] str;
	}

	cursorStart.x = x;
	cursorStart.y = text.localPosition.y + y;

	cursorNeedUpdate = false;
}

void UI_InputText::RenderCursor()
{
	int timeSinceStart = (int)(App->GetTimeSinceStart() * cursorBlinkSpeed);
	int rest = timeSinceStart % 2;
	if (rest == 0)
	{
		if (cursorNeedUpdate)
			UpdateCursorPosition();
		int x = 0, y = 0;
		SDL_QueryTexture(text.GetTexture(), NULL, NULL, &x, &y);
		int x1 = cursorStart.x + text.GetWorldPosition().x;
		int x2 = x1;
		int y1 = text.GetWorldPosition().y;
		int y2 = y1 + y;

		App->render->DrawLine(x1, y1, x2, y2, sprite.useCamera, 255, 255, 255);
	}
}

void UI_InputText::GetNewInput(char* text)
{
	bool end = false;
	if (textList.size() == 0)
	{
		for (uint i = 0; !end && textList.size() <= (uint)maxCharacters; i++)
		{
			if (text[i] == '\0')
			{
				end = true;
			}
			else
			{
				textList.push_back(text[i]);
				textChanged = true;
				cursorPosition++;
				cursorNeedUpdate = true;
			}
		}
		currentChar = textList.end();
	}
	else if (textList.size() < (uint)maxCharacters)
	{
		for (uint i = 0; !end && textList.size() <= (uint)maxCharacters; i++)
		{
			if (text[i] == '\0')
			{
				end = true;
			}
			else
			{
				currentChar = textList.insert(currentChar, text[i]);
				currentChar++;
				textChanged = true;
				cursorPosition++;
				cursorNeedUpdate = true;
			}
		}
	}
}

void UI_InputText::DeleteCharacterOnCursor()
{
	if (textList.size() > 0)
	{
		if (currentChar != textList.begin())
		{
			cursorPosition--;
			std::list<char>::iterator toErase = currentChar;
			toErase--;
			textList.erase(toErase);
			textChanged = true;
		}
	}
}

void UI_InputText::DeleteNextCharacterToCursor()
{
	if (textList.size() > 0)
	{
		if (currentChar != textList.end())
		{
			std::list<char>::iterator toDelete;
			toDelete = currentChar;
			currentChar++;
			textList.erase(toDelete);
		}
		textChanged = true;
		cursorNeedUpdate = true;
	}
}

void UI_InputText::UpdateTextTexture()
{
	if (textList.size() > 0)
	{
		char* str = new char[textList.size() + 1];
		if (!hiddenText)
		{
			//Building the string from the list
			std::list<char>::iterator item = textList.begin();
			for (uint i = 0; i < textList.size() && item != textList.end(); i++)
			{
				str[i] = (*item);
				item++;
			}
			str[textList.size()] = '\0';
		}
		else
		{
			for (uint i = 0; i < textList.size(); i++)
			{
				str[i] = '*';
			}
			str[textList.size()] = '\0';
		}
		//Printing the string into the texture
		text.SetText(str);

		delete[] str;
	}
	else
	{
		text.SetText(" ");
	}
}

bool UI_InputText::PersonalUpdate(float dt)
{
	if (textChanged)
	{
		UpdateTextTexture();
		textChanged = false;
	}

	if (App->gui->focus == this)
	{
		if (textList.size() == 0 && defaultOn)
		{
			text.SetText("");
			defaultOn = false;
		}

		ManageTextInput();
		RenderCursor();
	}
	else if (!defaultOn && textList.size() == 0)
	{
		text.SetText(defaultText);
		defaultOn = true;
	}

	text.Draw();
	return true;
}

void UI_InputText::ManageTextInput()
{
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		if ((uint)cursorPosition < textList.size())
		{
			cursorPosition++;
			cursorNeedUpdate = true;
			if (cursorPosition == 1)
			{
				currentChar = textList.begin();
			}
			else
				currentChar++;
		}

	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		if (cursorPosition > 0)
		{
			cursorPosition--;
			cursorNeedUpdate = true;
			currentChar--;
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN)
	{
		DeleteCharacterOnCursor();
		cursorNeedUpdate = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{
		DeleteNextCharacterToCursor();
	}
}

C_String UI_InputText::GetString() const
{
	if (textList.size() == 0)
	{
		LOG("this has no string");
		return NULL;
	}

	else
	{

		char* str = new char[textList.size() + 1];
		std::list<char>::const_iterator item = textList.begin();
		for (int i = 0; item != textList.end(); i++)
		{
			str[i] = (*item);
			item++;
		}

		str[textList.size()] = '\0';

		C_String ret = str;
		delete[] str;
		return ret;
	}

}

void UI_InputText::DeleteText()
{
	textList.clear();
	UpdateTextTexture();
	cursorPosition = 0;
	UpdateCursorPosition();
}

void UI_InputText::OnEvent(GUI_EVENTS event)
{
	switch (event)
	{
	case UI_MOUSE_ENTER:
	{
		//App->input->DisableCursorImage();
		App->gui->inputEnabled = true;
		break;
	}
	case UI_MOUSE_EXIT:
	{
		App->gui->inputEnabled = false;
		//App->input->EnableCursorImage();
		break;
	}
	case UI_MOUSE_DOWN:
	{
		if (defaultText)
		{
			if (defaultOn)
			{
				defaultOn = false;
				textChanged = true;
			}
		}

	}
	case UI_GET_FOCUS:
	{
		App->input->FreezeInput();
		SDL_StartTextInput();
		break;
	}
	case UI_LOST_FOCUS:
	{
		SDL_StopTextInput();
		App->input->UnFreezeInput();
	}
	}
}
#pragma endregion