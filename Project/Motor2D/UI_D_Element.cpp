#include "UI_D_Element.h"

#include "j1Input.h"

#include "j1Gui_D.h"
#include "j1Render.h"

#include "SDL_ttf\include\SDL_ttf.h"



//////////////////////////////////////////////////////////
//														//
//					Ctrl+M+O							//
//			or suffer the consequences					//
//														//
//////////////////////////////////////////////////////////



// --------------- UI_D__ELEMENT --------------------------------------------------------

#pragma region UI_D__ELEMENT

UI_D_Element::UI_D_Element(int posX, int posY, int width, int heigth, SDL_Rect _collider, bool _active, uint _layer) : active(_active), movable(false)
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

bool UI_D_Element::Update(float dt)
{
	bool ret = true;
	if (active == true)
	{
		PersonalUpdate(dt);

		if (App->gui_D->debug == true)
		{
			if (App->gui_D->focus == this)
			{
				App->render->DrawQuad(GetColliderWorldPosition(), useCamera, 50, 255, 50, 50);
			}
			else
			{
				App->render->DrawQuad(GetColliderWorldPosition(), useCamera, 255, 50, 50, 50);
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

void UI_D_Element::InputManager()
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
				App->gui_D->focus = NULL;
			}
		}

		if (lastEvent != currentEvent && currentEvent != UI_NONE)
		{
			if (listeners.start != NULL)
			{
				SendEvent(currentEvent);
				if (currentEvent == UI_MOUSE_DOWN)
				{
					App->gui_D->focus = this;
				}
			}
			lastEvent = currentEvent;
		}
	}
}

void UI_D_Element::SendEvent(GUI_EVENTS event)
{
	p2List_item<j1Module*>* item = listeners.start;
	while (item)
	{
		//item->data->OnGUI(event, this);
		item = item->next;
	}
}

SDL_Rect UI_D_Element::GetWorldPosition()
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

SDL_Rect UI_D_Element::GetColliderWorldPosition()

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

void UI_D_Element::ForceLastEvent(GUI_EVENTS _event)
{
	if (lastEvent != _event) { SendEvent(_event);  lastEvent = _event; }
}

GUI_EVENTS UI_D_Element::GetLastEvent()
{
	return lastEvent;
}

void UI_D_Element::AddListener(j1Module* toAdd)
{
	listeners.add(toAdd);
}

bool UI_D_Element::HasListeners()
{
	if (listeners.start) { return true; } return false;
}

void UI_D_Element::SetActive(bool _active)
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

const bool UI_D_Element::GetActive()
{
	return active;
}

void UI_D_Element::SetParent(UI_D_Element* _parent)
{
	parent = _parent;
	_parent->childs.PushBack(this);
}

#pragma endregion


// --------------- UI_D_ANIMATION --------------------------------------------------------

#pragma region UI_D__ANIMATION

UI_D_AnimatedImage::UI_D_AnimatedImage(int x, int y, int w, int h, char* path, SDL_Rect* _rect, uint nFrames, float speed, SDL_Rect _collider) : UI_D_Image(x, y, w, h, _rect[0], path, _collider)
{
	for (uint n = 0; n < nFrames && _rect; n++, _rect++)
	{
		animation.frames.PushBack(*_rect);
	}
	animation.speed = speed;
	animation.loop = true;
}

UI_D_AnimatedImage::UI_D_AnimatedImage(int x, int y, int w, int h, SDL_Rect* _rect, uint nFrames, float speed, SDL_Rect _collider) : UI_D_Image(x, y, w, h, App->gui_D->GetAtlas(), _rect[0], _collider)
{
	for (uint n = 0; n < nFrames && _rect; n++, _rect++)
	{
		animation.frames.PushBack(*_rect);
	}
	animation.speed = speed;
	animation.loop = true;
}

bool UI_D_AnimatedImage::PersonalUpdate(float dt)
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


//--------------- UI_D__BUTTON --------------------------------------------------------

#pragma region UI_D__BUTTON

UI_D_Button::UI_D_Button(int x, int y, int w, int h, char* path, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect _collider) : UI_D_Element(x, y, w, h, _collider)
{
	texture = App->tex->Load(path);
	rect[0] = button;
	rect[1] = hover;
	rect[2] = clicked;
}

UI_D_Button::UI_D_Button(int x, int y, int w, int h, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect _collider) : UI_D_Element(x, y, w, h, _collider)
{
	texture = NULL;
	rect[0] = button;
	rect[1] = hover;
	rect[2] = clicked;
}

bool UI_D_Button::PersonalUpdate(float dt)
{
	if (!Draw())
	{
		LOG("No se pudo dibujar la textura.");
	}
	return true;
}

bool UI_D_Button::Draw()
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
		App->render->Blit(texture, &GetWorldPosition(), useCamera, &toDraw);
		return true;
	}
	else if (App->render->Blit(App->gui_D->GetAtlas(), &GetWorldPosition(), useCamera, &toDraw))
	{
		return true;
	}
	return false;
}

#pragma endregion


// --------------- UI_D_IMAGE --------------------------------------------------------

#pragma region UI_D__IMAGE

UI_D_Image::UI_D_Image(int x, int y, int w, int h, SDL_Rect _rect, char* path, SDL_Rect _collider) : UI_D_Element(x, y, w, h, _collider)
{
	texture = App->tex->Load(path);
	rect = new SDL_Rect(_rect);
}

UI_D_Image::UI_D_Image(int x, int y, int w, int h, char* path, SDL_Rect _collider) : UI_D_Element(x, y, w, h, _collider)
{
	texture = App->tex->Load(path);
	rect = NULL;
}

UI_D_Image::UI_D_Image(int x, int y, int w, int h, SDL_Texture* _texture, SDL_Rect _rect, SDL_Rect _collider) : UI_D_Element(x, y, w, h, _collider)
{
	texture = _texture;
	rect = new SDL_Rect(_rect);
}

UI_D_Image::UI_D_Image(int x, int y, int w, int h, SDL_Texture* _texture, SDL_Rect _collider) : UI_D_Element(x, y, w, h, _collider)
{
	texture = _texture;
	rect = NULL;
}

UI_D_Image::~UI_D_Image()
{
	RELEASE(rect);
}


bool UI_D_Image::PersonalUpdate(float dt)
{
	if (!Draw())
	{
		LOG("No se pudo dibujar la textura.");
	}
	return true;
}

bool UI_D_Image::Draw()
{
	if (texture)
	{
		App->render->Blit(texture, &GetWorldPosition(), useCamera, rect);
		return true;
	}
	else if (App->render->Blit(App->gui_D->GetAtlas(), &GetWorldPosition(), useCamera, rect))
	{
		return true;
	}
	return false;
}

#pragma endregion


// --------------- UI_D__RECTANGLE --------------------------------------------------------

#pragma region UI_D__RECTANGLE

UI_D_Rect::UI_D_Rect(int x, int y, int w, int h, uint r, uint g, uint b, uint a, SDL_Rect _collider) : UI_D_Element(x, y, w, h, _collider)
{
	R = r;
	G = g;
	B = b;
	A = a;
}

UI_D_Rect::~UI_D_Rect()
{
}


bool UI_D_Rect::PersonalUpdate(float dt)
{
	if (!Draw())
	{
		LOG("No se pudo dibujar la textura.");
	}
	return true;
}

bool UI_D_Rect::Draw()
{
	App->render->DrawQuad(GetWorldPosition(), R, G, B, A);
	return true;
}

#pragma endregion



// --------------- UI_D_LABEL --------------------------------------------------------

#pragma region UI_D__LABEL

UI_D_Label::UI_D_Label(int x, int y, int w, int h, char* _text, _TTF_Font* _typo, SDL_Rect _collider) : UI_D_Element(x, y, w, h, _collider)
{
	text = _text;
	typo = _typo;
	SetText(text);
	R = B = G = 255;
}

bool UI_D_Label::PersonalUpdate(float dt)
{
	if (!Draw())
	{
		LOG("No se pudo dibujar la textura.");
	}
	return true;
}

bool UI_D_Label::Draw()
{
	if (texture)
	{
		App->render->Blit(texture, &GetWorldPosition(), useCamera);
		return true;
	}
	return false;
}

bool UI_D_Label::SetText(char* _text, int _R, int _G, int _B)
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

bool UI_D_Label::SetText(p2SString _text, int _R, int _G, int _B)
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

#pragma endregion


// --------------- UI_D_COLLAPSE --------------------------------------------------------

#pragma region UI_D__COLLAPSE

UI_D_Collapse::UI_D_Collapse(int x, int y, int w, int h, UI_D_Element* toColapse, SDL_Rect closed, SDL_Rect open, SDL_Rect _collider) : UI_D_Element(x, y, w, h, _collider)
{
	linkedElement = toColapse;
	images[0] = open;
	images[1] = closed;
}

bool UI_D_Collapse::PersonalUpdate(float dt)
{
	if (linkedElement->GetActive())
	{
		App->render->Blit(App->gui_D->GetAtlas(), &GetWorldPosition(), useCamera, &images[0]);
	}
	else
	{
		App->render->Blit(App->gui_D->GetAtlas(), &GetWorldPosition(), useCamera, &images[1]);
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


// --------------- UI_D_PROGRESS_BAR --------------------------------------------------------

#pragma region UI_D_PROGRESS_BAR

UI_D_ProgressBar::UI_D_ProgressBar(int x, int y, int w, int h, SDL_Texture* _texture, SDL_Rect _rect, int* _maxData, int* _currentData) : UI_D_Element(x, y, w, h), label(_rect.w / 2, _rect.h / 2 - 17, w, h, "0/0")
{
	texture = _texture;
	rect = _rect;
	maxData = _maxData;
	currentData = _currentData;
	label.SetParent(this);
}

UI_D_ProgressBar::UI_D_ProgressBar(int x, int y, int w, int h, char* path, SDL_Rect _rect, int* _maxData, int* _currentData) : UI_D_Element(x, y, w, h), label(_rect.w / 2, _rect.h / 2 - 17, w, h, "0/0")
{
	texture = App->tex->Load(path);
	rect = _rect;
	maxData = _maxData;
	currentData = _currentData;
	label.SetParent(this);
}

bool UI_D_ProgressBar::PersonalUpdate(float dt)
{
	char buf[46];
	sprintf_s(buf, sizeof(char) * 46, "%i  /  %i\0", *currentData, *maxData);

	label.SetText(buf);

	float ratio = ((float)*currentData / (float)*maxData);

	SDL_Rect toDraw = rect;
	toDraw.w *= ratio;


	SDL_Rect rect = GetWorldPosition();
	rect.w *= ratio;

	App->render->Blit(texture, &rect, useCamera, &toDraw);
	label.Draw();


	return true;
}

SDL_Texture* UI_D_ProgressBar::GetTexture()
{
	return texture;
}

void UI_D_ProgressBar::SetRect(SDL_Rect _rect)
{
	rect = _rect;
}
#pragma endregion


// --------------- UI_D_InputText --------------------------------------------------------

#pragma region UI_D_INPUT_Text

UI_D_InputText::UI_D_InputText(int x, int y, int w, int h, char* defText, SDL_Rect _collider, int offsetX, int offsetY) : UI_D_Element(x, y, w, h, _collider), text(offsetX, offsetY, w, h, defText)
{
	defaultText = defText;
	text.SetParent(this);
}

UI_D_InputText::~UI_D_InputText()
{}

void UI_D_InputText::UpdateCursorPosition()
{
	int x = 0, y = 0;
	if (cursorPosition > 0)
	{
		p2List_item<char>* item = textList.start;
		char* str = new char[cursorPosition + 1];
		for (int i = 0; i < cursorPosition && item; i++)
		{
			if (!hiddenText)
			{
				str[i] = item->data;
			}
			else
			{
				str[i] = '*';
			}
			item = item->next;
		}
		str[cursorPosition] = '\0';

		TTF_SizeText(text.GetFont(), str, &x, &y);

		delete[] str;
	}

	cursorStart.x = text.localPosition.x + x;
	cursorStart.y = text.localPosition.y + y;

	cursorNeedUpdate = false;
}

void UI_D_InputText::RenderCursor()
{
	int timeSinceStart = (int)(App->GetTimeSinceStart() * cursorBlinkSpeed);
	int rest = timeSinceStart % 2;
	if (rest == 0)
	{
		if (cursorNeedUpdate)
			UpdateCursorPosition();
		int x1 = cursorStart.x + text.GetWorldPosition().x;
		int x2 = x1;
		int y1 = text.GetWorldPosition().y;
		int y2 = y1 + collider.h - 2 *(text.localPosition.y);
		App->render->DrawLine(x1, y1, x2, y2, useCamera, 255, 255, 255);
	}
}

void UI_D_InputText::GetNewInput(char* text)
{
	bool end = false;
	if (textList.count() < (uint)maxCharacters)
	{
		for (uint i = 0; !end && textList.count() <= (uint)maxCharacters; i++)
		{
			if (text[i] == '\0')
				end = true;
			else
			{
				p2List_item<char>* charItem = new p2List_item<char>(text[i]);
				textList.Insert(currentChar, charItem);
				textChanged = true;
				cursorPosition++;
				cursorNeedUpdate = true;
				currentChar = charItem;
			}
		}
	}
}

void UI_D_InputText::DeleteCharacterOnCursor()
{
	if (textList.count() > 0)
	{
		if (currentChar)
		{
			cursorPosition--;
			p2List_item<char>* nextCurrent = currentChar->prev;
			textList.del(currentChar);
			currentChar = nextCurrent;
			textChanged = true;
		}
	}
}

void UI_D_InputText::DeleteNextCharacterToCursor()
{
	if (textList.count() > 0)
	{
		if (currentChar)
		{
			textList.del(currentChar->next);;
		}
		else
		{
			textList.del(textList.start);
		}
		textChanged = true;
		cursorNeedUpdate = true;
	}
}

void UI_D_InputText::UpdateTextTexture()
{
	if (textList.count() > 0)
	{
		char* str = new char[textList.count() + 1];
		if (!hiddenText)
		{
			//Building the string from the list
			p2List_item<char>* item = textList.start;
			for (uint i = 0; i < textList.count() && item; i++)
			{
				str[i] = item->data;
				item = item->next;
			}
			str[textList.count()] = '\0';
		}
		else
		{
			for (uint i = 0; i < textList.count(); i++)
			{
				str[i] = '*';
			}
			str[textList.count()] = '\0';
		}
		//Printing the string into the texture
		text.SetText(str);

		delete[] str;
	}
}
bool UI_D_InputText::PersonalUpdate(float dt)
{
	if (textChanged)
	{
		UpdateTextTexture();
		textChanged = false;
	}

	if (App->gui_D->focus == this)
	{
		if (textList.count() == 0 && defaultOn)
		{
			text.SetText("");
			defaultOn = false;
		}

		ManageTextInput();
		RenderCursor();
	}
	else if (!defaultOn && textList.count() == 0)
	{
		text.SetText(defaultText);
		defaultOn = true;
	}

	text.Draw();
	return true;
}

void UI_D_InputText::ManageTextInput()
{
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		if ((uint)cursorPosition < textList.count())
		{
			cursorPosition++;
			cursorNeedUpdate = true;
			if (cursorPosition == 1)
			{
				currentChar = textList.start;
			}
			else
				currentChar = currentChar->next;
		}

	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		if (cursorPosition > 0)
		{
			cursorPosition--;
			cursorNeedUpdate = true;
			currentChar = currentChar->prev;
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

p2SString UI_D_InputText::GetString() const
{
	if (textList.count() == 0)
	{
		LOG("this has no string");
		return NULL;
	}

	else
	{

		char* str = new char[textList.count() + 1];
		p2List_item<char>* item = textList.start;
		for (int i = 0; item; i++)
		{
			str[i] = item->data;
			item = item->next;
		}

		str[textList.count()] = '\0';

		p2SString ret = str;
		delete[] str;
		return ret;
	}

}

void UI_D_InputText::DeleteText()
{
	textList.clear();
	UpdateTextTexture();
	cursorPosition = 0;
	UpdateCursorPosition();
}
#pragma endregion