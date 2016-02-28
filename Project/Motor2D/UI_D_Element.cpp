#include "UI_D_Element.h"

#include "j1Input.h"

#include "j1Gui_D.h"

#include "j1Console.h"

//////////////////////////////////////////////////////////
//														//
//					Ctrl+M+O							//
//			or suffer the consequences					//
//														//
//////////////////////////////////////////////////////////



// --------------- UI_D__ELEMENT --------------------------------------------------------

#pragma region UI_D__ELEMENT

UI_D_Element::UI_D_Element(UIElementType _type, int _id, int posX, int posY, int width, int heigth, SDL_Rect _collider, bool _active, uint _layer) : type(_type), active(_active), id(_id), movable(false)
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

UIElementType UI_D_Element::GetType()
{
	return type;
}

int UI_D_Element::GetId()
{
	return id;
}

void UI_D_Element::SetParent(UI_D_Element* _parent)
{
	parent = _parent;
	_parent->childs.PushBack(this);
}

#pragma endregion


// --------------- UI_D__ANIMATION --------------------------------------------------------

#pragma region UI_D__ANIMATION

UI_D_AnimatedImage::UI_D_AnimatedImage(int _id, int x, int y, int w, int h, char* path, SDL_Rect* _rect, uint nFrames, float speed, SDL_Rect _collider) : UI_D_Image(_id, x, y, w, h, _rect[0], path, _collider)
{
	for (uint n = 0; n < nFrames && _rect; n++, _rect++)
	{
		animation.frames.PushBack(*_rect);
	}
	animation.speed = speed;
	animation.loop = true;
}

UI_D_AnimatedImage::UI_D_AnimatedImage(int _id, int x, int y, int w, int h, SDL_Rect* _rect, uint nFrames, float speed, SDL_Rect _collider) : UI_D_Image(_id, x, y, w, h, _rect[0], _collider)
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

UI_D_Button::UI_D_Button(int _id, int x, int y, int w, int h, char* path, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect _collider) : UI_D_Element(UI_Button, _id, x, y, w, h, _collider)
{
	texture = App->tex->Load(path);
	rect[0] = button;
	rect[1] = hover;
	rect[2] = clicked;
}

UI_D_Button::UI_D_Button(int _id, int x, int y, int w, int h, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect _collider) : UI_D_Element(UI_Button, _id, x, y, w, h, _collider)
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
		App->render->Blit(texture, &GetWorldPosition(), &toDraw);
		return true;
	}
	else if (App->render->Blit(App->gui_D->GetAtlas(), &GetWorldPosition(), &toDraw))
	{
		return true;
	}
	return false;
}

#pragma endregion


// --------------- UI_D__IMAGE --------------------------------------------------------

#pragma region UI_D__IMAGE

UI_D_Image::UI_D_Image(int _id, int x, int y, int w, int h, SDL_Rect _rect, char* path, SDL_Rect _collider) : UI_D_Element(UI_Image, _id, x, y, w, h, _collider)
{
	texture = App->tex->Load(path);
	rect = new SDL_Rect(_rect);
}

UI_D_Image::UI_D_Image(int _id, int x, int y, int w, int h, char* path, SDL_Rect _collider) : UI_D_Element(UI_Image, _id, x, y, w, h, _collider)
{
	texture = App->tex->Load(path);
	rect = NULL;
}

UI_D_Image::UI_D_Image(int _id, int x, int y, int w, int h, SDL_Rect _rect, SDL_Rect _collider) : UI_D_Element(UI_Image, _id, x, y, w, h, _collider)
{
	texture = NULL;
	rect = new SDL_Rect(_rect);
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
		App->render->Blit(texture, &GetWorldPosition(), rect);
		return true;
	}
	else if (App->render->Blit(App->gui_D->GetAtlas(), &GetWorldPosition(), rect))
	{
		return true;
	}
	return false;
}

#pragma endregion


// --------------- UI_D__RECTANGLE --------------------------------------------------------

#pragma region UI_D__RECTANGLE

UI_D_Rect::UI_D_Rect(int _id, int x, int y, int w, int h, uint r, uint g, uint b, uint a, SDL_Rect _collider) : UI_D_Element(UI_Rect, _id, x, y, w, h, _collider)
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



// --------------- UI_D__LABEL --------------------------------------------------------

#pragma region UI_D__LABEL

UI_D_Label::UI_D_Label(int _id, int x, int y, int w, int h, char* _text, UI_LabelAlineation _alineation, _TTF_Font* _typo, SDL_Rect _collider) : UI_D_Element(UI_Label, _id, x, y, w, h, _collider)
{
	text = _text;
	typo = _typo;
	SetText(text);
	alineation = _alineation;
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
		App->render->Blit(texture, &GetWorldPosition(), false);
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

void UI_D_Label::SetAlineation(UI_LabelAlineation _alineation)
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


// --------------- UI_D__COLLAPSE --------------------------------------------------------

#pragma region UI_D__COLLAPSE

UI_D_Collapse::UI_D_Collapse(int _id, int x, int y, int w, int h, UI_D_Element* toColapse, SDL_Rect closed, SDL_Rect open, SDL_Rect _collider) : UI_D_Element(UI_Collapse, _id, x, y, w, h, _collider)
{
	linkedElement = toColapse;
	images[0] = open;
	images[1] = closed;
}

bool UI_D_Collapse::PersonalUpdate(float dt)
{
	if (linkedElement->GetActive())
	{
		App->render->Blit(App->gui_D->GetAtlas(), &GetWorldPosition(), &images[0]);
	}
	else
	{
		App->render->Blit(App->gui_D->GetAtlas(), &GetWorldPosition(), &images[1]);
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


// --------------- UI_D__PROGRESS_BAR --------------------------------------------------------

#pragma region UI_D__PROGRESS_BAR

UI_D_ProgressBar::UI_D_ProgressBar(int _id, int x, int y, int w, int h, SDL_Rect _image, int* _maxData, int* _currentData) : UI_D_Element(UI_ProgressBar, _id, x, y, w, h), label(_id, _image.w / 2, _image.h / 2 - 17, w, h, "0/0", UI_AlignCenter)
{
	image = _image;
	maxData = _maxData;
	currentData = _currentData;
	label.SetParent(this);
}

bool UI_D_ProgressBar::PersonalUpdate(float dt)
{
	char buf[46];
	sprintf_s(buf, sizeof(char) * 46, "%i  /  %i\0", *currentData, *maxData);



	label.SetText(buf);

	SDL_Rect toDraw = image;
	toDraw.w *= ((float)*currentData / (float)*maxData);
	
	App->render->Blit(App->gui_D->GetAtlas(), GetWorldPosition().x, GetWorldPosition().y, &toDraw);
	label.Draw();


	return true;
}

#pragma endregion