#include "UI_Element.h"

#include "M_InputManager.h"

#include "M_GUI.h"
#include "M_Render.h"
#include "j1App.h"
#include "Orders Factory.h"
#include "M_Input.h"

#include "SDL_ttf\include\SDL_ttf.h"
//No time for clean solutions
#include "Building.h"



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
	sprite.useCamera = false;
}

bool UI_Element::Update(float dt)
{
	bool ret = true;
	if (active == true)
	{
		sprite.position = GetWorldPosition();
		PersonalUpdate(dt);

#pragma region debug
		if (App->gui->debug == true)
		{
			if (App->gui->focus == this)
			{
				App->render->AddDebugRect(GetColliderWorldPosition(), sprite.useCamera, 50, 255, 50, 50);
			}
			else
			{
				App->render->AddDebugRect(GetColliderWorldPosition(), sprite.useCamera, 255, 50, 50, 50);
			}
			SDL_Rect pos = GetWorldPosition();
			App->render->AddDebugRect(pos, sprite.useCamera, 100, 100, 255, 100);
		}
#pragma endregion
	}
	return ret;
}

void UI_Element::InputManager()
{
	if (active == true)
	{
		GUI_EVENTS currentEvent = UI_NONE;
		int mouseX = App->events->GetMouseOnScreen().x;
		int mouseY = App->events->GetMouseOnScreen().y;
		int motionX = App->events->GetMouseMotion().x;
		int motionY = App->events->GetMouseMotion().y;
		SDL_Rect worldPos = GetColliderWorldPosition();

		if (mouseX > worldPos.x && mouseX < worldPos.x + worldPos.w && mouseY > worldPos.y && mouseY < worldPos.y + worldPos.h)
		{
			if ((lastEvent == UI_MOUSE_EXIT || lastEvent == UI_KEYBOARD_FOCUSED))
			{
				currentEvent = UI_MOUSE_ENTER;
			}
		}
		else if (lastEvent != UI_KEYBOARD_FOCUSED || motionX != 0 || motionY != 0)
		{
			currentEvent = UI_MOUSE_EXIT;
		}

		if (lastEvent != UI_MOUSE_EXIT && currentEvent != UI_MOUSE_EXIT && (App->events->GetEvent(E_LEFT_CLICK) == EVENT_DOWN))
		{
			currentEvent = UI_MOUSE_DOWN;
		}
		else if (lastEvent == UI_MOUSE_DOWN && (App->events->GetEvent(E_LEFT_CLICK) == EVENT_UP))
		{
			currentEvent = UI_MOUSE_UP;
		}

		if (lastEvent != UI_MOUSE_EXIT && currentEvent != UI_MOUSE_EXIT && (App->events->GetEvent(E_RIGHT_CLICK) == EVENT_DOWN))
		{
			SendEvent(UI_RIGHT_MOUSE_DOWN);
			App->events->EraseEvent(E_RIGHT_CLICK);
		}

		if (App->gui->focus == this && App->events->GetEvent(E_PRESSED_ENTER) == EVENT_DOWN)
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

		if (lastEvent == UI_MOUSE_DOWN || currentEvent == UI_MOUSE_DOWN)
		{
				App->events->clickedUI = true;
		}
		if (lastEvent != UI_MOUSE_EXIT && currentEvent != UI_MOUSE_EXIT)
		{
			App->events->hoveringUI = true;
			App->entityManager->FreezeInput();
		}

		if (lastEvent != currentEvent && currentEvent != UI_NONE)
		{
			if (listeners.empty() == false)
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

		if (App->gui->focus == this && lastEvent == UI_MOUSE_EXIT && (App->events->GetEvent(E_LEFT_CLICK) != EVENT_REPEAT))
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

SDL_Rect UI_Element::GetWorldPosition() const
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

SDL_Rect UI_Element::GetColliderWorldPosition() const
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

void UI_Element::SetAsFocus()
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

void UI_Element::UpdateSprite()
{
	sprite.position = GetWorldPosition();
}

GUI_EVENTS UI_Element::GetLastEvent() const
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
		std::list<UI_Element*>::iterator it = childs.begin();
		while(it != childs.end())
		{
			(*it)->SetActive(_active);
			it++;
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

void UI_Element::SetLayer(uint layer)
{
	if (layer >= 0 && layer <= GUI_MAX_LAYERS)
	{
		sprite.layer = layer;
	}
}

const bool UI_Element::IsActive() const
{
	return active;
}

void UI_Element::SetParent(UI_Element* _parent)
{
	parent = _parent;
	_parent->childs.push_back(this);
	UpdateSprite();
}

#pragma endregion

//--------------- UI__BUTTON --------------------------------------------------------

#pragma region UI__BUTTON

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

UI_Button2::UI_Button2(int x, int y, int w, int h, SDL_Texture* _buttons, const SDL_Rect& button, const  SDL_Rect& clicked, const SDL_Rect _collider) : UI_Element(x, y, w, h, _collider)
{
	sprite.texture = _buttons;
	rect[0] = button;
	rect[1] = clicked;

	sprite.section = rect[0]; localPosition.w = rect[0].w; localPosition.h = rect[0].h;

	required_build = ZERG_SAMPLE;
}

UI_Button2::~UI_Button2()
{
	if (son != NULL)
	{
		App->gui->DeleteUIElement(son);
	}
	if (requiresImage != NULL)
	{
		App->gui->DeleteUIElement(requiresImage);
	}
	if (hoverImage != NULL)
	{
		App->gui->DeleteUIElement(hoverImage);
	}
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

void UI_Button2::SetActive(bool _active)
{
	if (active != _active)
	{
		active = _active;
		std::list<UI_Element*>::iterator it = childs.begin();
		while (it != childs.end())
		{
			(*it)->SetActive(_active);
			it++;
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
	if (_active)
	{
		if (hoverImage)
			hoverImage->SetActive(false);
		if (requiresImage)
			requiresImage->SetActive(false);
	}
}

bool UI_Button2::Draw()
{
	bool ret = true;

	//Set the rect to draw, then draw the back and then the UI image
	//Needs optimization! We set the rect at every frame!
	if (enabled)
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
		default:{ sprite.section = rect[0]; localPosition.w = rect[0].w; localPosition.h = rect[0].h; break; }
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

void UI_Button2::OnEvent(GUI_EVENTS event)
{
	if (event == UI_MOUSE_DOWN)
		if (order != NULL)
			order->Function();

	if (event == UI_MOUSE_ENTER)
	{
		if (hoverImage)
		{
			hoverImage->SetActive(true);
		}
	}
	if (event == UI_MOUSE_EXIT)
	{
		if (hoverImage)
		{
			hoverImage->SetActive(false);
		}

	}
};

void UI_Button2::SetHoverImage(UI_Image* image, bool setAsParent)
{
	if (setAsParent)
	{
		image->SetParent(this);
	}
	hoverImage = image;
}

void UI_Button2::SetRequiresImage(UI_Image* image)
{
	image->SetParent(this);
	requiresImage = image;
}

void UI_Button2::SetEnabled(bool state)
{
	SDL_Color col = { 255, 255, 255, 255 };

	if (!state)
		col = { 115, 115, 115, 255 };

	enabled = state;
	if (son)
	{
		son->sprite.tint = col;
	}
}

void UI_Button2::InitRequiredBuilding(Building_Type _type, bool state)
{
	required_build = _type;
	SetEnabled(state);
}

void UI_Button2::changeStateOnBuilding(Building_Type _type, bool state)
{
	if (required_build == _type)
	{
		SetEnabled(state);
	}
}
// --------------- UI_IMAGE --------------------------------------------------------

#pragma region UI__IMAGE

UI_Image::UI_Image(int x, int y, int w, int h, SDL_Texture* _texture, SDL_Rect _rect, SDL_Rect _collider) : UI_Element(x, y, w, h, _collider)
{
	sprite.texture = _texture;
	sprite.section = _rect;
	sprite.position = GetWorldPosition();
}

UI_Image::UI_Image(int x, int y, int w, int h, SDL_Texture* _texture, SDL_Rect _collider) : UI_Element(x, y, w, h, _collider)
{
	sprite.texture = _texture;
	sprite.section = { 0, 0, 0, 0 };
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

void
UI_Image::SetRect(const SDL_Rect& _rect)
{
	sprite.section.x = _rect.x; sprite.section.y = _rect.y; sprite.section.w = _rect.w; sprite.section.h = _rect.h;
//rect.x = _rect.x; rect.y = _rect.y; rect.w = _rect.w; rect.h = _rect.h;
}

SDL_Rect UI_Image::getRect()
{
	return sprite.section;
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
	text = C_String(_text);
	typo = _typo;
	if (typo == NULL)
	{
		typo = App->font->GetDefaultFont();
	}
	SetText(text);
	R = B = G = 255;
}

UI_Label::~UI_Label()
{	
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
	text = C_String(_text);
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
		SDL_QueryTexture(sprite.texture, NULL, NULL, &localPosition.w, &localPosition.h);
		sprite.position.w = localPosition.w;
		sprite.position.h = localPosition.h;
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
	if (linkedElement->IsActive())
	{
		App->render->Blit(App->gui->GetAtlas(), &GetWorldPosition(), sprite.useCamera, &images[0]);
	}
	else
	{
		App->render->Blit(App->gui->GetAtlas(), &GetWorldPosition(), sprite.useCamera, &images[1]);
	}
	if (lastEvent == UI_MOUSE_DOWN && changed == false)
	{
		linkedElement->SetActive(!linkedElement->IsActive());
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

UI_ProgressBar::UI_ProgressBar(int x, int y, int w, int h, SDL_Texture* _texture, SDL_Rect _rect, int* _maxData, int* _currentData) : UI_Element(x, y, w, h)
{
	sprite.texture = _texture;
	sprite.section = rect = _rect;
	maxData = _maxData;
	currentData = _currentData;
}

bool UI_ProgressBar::PersonalUpdate(float dt)
{
	if (currentData == NULL || maxData == NULL)
		return false;

	float ratio = ((float)*currentData / (float)*maxData);
	CAP(ratio, 0, 1);

	if (ratio > 0.01)
	{
		sprite.section = rect;
		sprite.position = GetWorldPosition();
		sprite.section.w *= ratio;
		sprite.position.w *= ratio;
		App->render->AddSprite(&sprite, GUI);
	}

	return true;
}

SDL_Texture* UI_ProgressBar::GetTexture()
{
	return sprite.texture;
}

void UI_ProgressBar::SetTexture(SDL_Texture* text)
{
	sprite.texture = text;
}

void UI_ProgressBar::SetRect(SDL_Rect _rect)
{
	sprite.section = rect = _rect;
}

UI_ProgressBar_F::UI_ProgressBar_F(int x, int y, int w, int h, SDL_Texture* _texture, SDL_Rect _rect, float* _maxData, float* _currentData) : UI_Element(x, y, w, h)
{
	sprite.texture = _texture;
	sprite.section = rect = _rect;
	maxData = _maxData;
	currentData = _currentData;
}

bool UI_ProgressBar_F::PersonalUpdate(float dt)
{
	if (currentData == NULL || maxData == NULL)
		return false;

	float ratio = ((float)*currentData / (float)*maxData);
	CAP(ratio, 0, 1);

	if (ratio > 0.01)
	{
		sprite.section = rect;
		sprite.position = GetWorldPosition();
		sprite.section.w *= ratio;
		sprite.position.w *= ratio;
		App->render->AddSprite(&sprite, GUI);
	}

	return true;
}

SDL_Texture* UI_ProgressBar_F::GetTexture()
{
	return sprite.texture;
}

void UI_ProgressBar_F::SetTexture(SDL_Texture* text)
{
	sprite.texture = text;
}

void UI_ProgressBar_F::SetRect(SDL_Rect _rect)
{
	sprite.section = rect = _rect;
}


UI_HPBar::UI_HPBar(int x, int y, int w, int h, SDL_Texture* hp_tex, SDL_Texture* shield_tex, SDL_Texture* back_tex, int* _currHP, int* _maxHP, int* _currShield, int* _maxShield) : UI_Element(x, y, 0, 0)
{
	sprite.section = shield.section = back.section = rect = { 0, 0, w, h };
	sprite.texture = hp_tex;
	shield.texture = shield_tex;
	back.texture = back_tex;

	currHP = _currHP;
	maxHP = _maxHP;
	currShield = _currShield;
	maxShield = _maxShield;
}

bool UI_HPBar::PersonalUpdate(float dt)
{
	float hp_ratio = ((float)*currHP / (float)*maxHP);
	float shield_ratio = ((float)*currShield / (float)*maxShield);
	CAP(hp_ratio, 0, 1);
	CAP(shield_ratio, 0, 1);

	sprite.section = shield.section = back.section = rect;
	sprite.position = shield.position = back.position = GetWorldPosition();

	if (hp_ratio > 0.01)
	{
		sprite.position = GetWorldPosition();
		sprite.y_ref = sprite.position.y;
		sprite.section.w *= hp_ratio;
		sprite.position.w *= hp_ratio;

		if (hp_ratio < (0.33))
		{
			sprite.section.y = 14;
		}
		else if (hp_ratio < (0.66))
		{
			sprite.section.y = 7;
		}
		else
		{
			sprite.section.y = 0;
		}
		CAP(sprite.section.w, 1, INT_MAX);
		App->render->AddSprite(&sprite, SCENE);
	}

	if (shield_ratio > 0.01)
	{
		shield.position = sprite.position;
		shield.y_ref = sprite.position.y;
		shield.section.w *= shield_ratio;
		shield.position.w *= shield_ratio;
		CAP(shield.section.w, 1, INT_MAX);
		App->render->AddSprite(&shield, SCENE);
	}

	back.y_ref = sprite.y_ref;
	App->render->AddSprite(&back, SCENE);

	return true;
}


#pragma endregion


// --------------- UI_InputText --------------------------------------------------------

#pragma region UI_INPUT_Text

UI_InputText::UI_InputText(int x, int y, int w, int h, char* defText, SDL_Rect _collider, int offsetX, int offsetY) : UI_Element(x, y, w, h, _collider), text(offsetX, offsetY, w, h, defText)
{
	defaultText = C_String(defText);
	text.SetParent(this);
	currentChar = textList.end();
}

UI_InputText::~UI_InputText()
{
	textList.clear();
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
				if (text[i] != '%')
				{
					textList.push_back(text[i]);
					textChanged = true;
					cursorPosition++;
					cursorNeedUpdate = true;
				}
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
				if (text[i] != '%')
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
		if (defaultText.GetString())
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
		App->events->FreezeInput();
		SDL_StartTextInput();
		break;
	}
	case UI_LOST_FOCUS:
	{
		SDL_StopTextInput();
		App->events->UnfreezeInput();
	}
	}
}
#pragma endregion