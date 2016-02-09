#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "UIElements.h"

j1Gui::j1Gui(bool start_enabled) : j1Module(start_enabled)
{
	name.create("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");
	cursorInput_file_name = conf.child("cursorInput").attribute("file").as_string("");

	App->console->AddCommand(&c_UIDebug);
	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	cursorInput_rect = SDL_Rect{ 0, 0, 20, 20 };
	cursorInput = App->tex->Load(cursorInput_file_name.GetString());
	atlas = App->tex->Load(atlas_file_name.GetString());

	screen_button = CreateScreenButton();
	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	return true;
}

bool j1Gui::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_UP)
	{
		focus = GetNextFocus();
	}
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		ClickFocus();
	}
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_UP)
	{
		UnclickFocus();
	}


	return true;
}
// Called after all Updates
bool j1Gui::PostUpdate(float dt)
{
	inputRecieved = false;
	for (int lay = 0; lay <= GUI_MAX_LAYERS; lay++)
	{
		p2List_item<UIElement*>* item = uiElements.start;
		while (item)
		{
			if (item->data->active && item->data->layer == lay)
			{
				item->data->Update(dt);
			}
			item = item->next;
		}	
	}

	for (int lay = GUI_MAX_LAYERS; lay >= 0; lay--)
	{
		p2List_item<UIElement*>* item = uiElements.end;
		while (item && !inputRecieved)
		{
			if (item->data->active && item->data->layer == lay)
			{
				item->data->CheckInput();

			}
			item = item->prev;
		}
	}
	//DrawFocus();
	if (inputEnabled)
		DrawCursorInput();
	if (hoveringElement != NULL)
	{
		if (hoveringElement->hoveringTexture != NULL)
		{
			DrawHoveringImage();
		}
	}
	return true;
}

// Called before quiting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");
	
	p2List_item<UIElement*>* item = uiElements.start;
	while (item)
	{
		p2List_item<UIElement*>* nextItem = item->next;
		item = nextItem;
	}
	uiElements.clear();
	return true;
}

UIButton* j1Gui::CreateScreenButton()
{
	SDL_Rect screenRect = { 0, 0, App->render->camera.w, App->render->camera.h};
	UIButton* scr_button = new UIButton;
	scr_button->SetCollider(screenRect);
	scr_button->name = "Screen Button";
	scr_button->active = true;
	scr_button->listener = this;
	uiElements.add(scr_button);
		
	return scr_button;
}

UIImage* j1Gui::CreateImage(char* name, iPoint position, p2DynArray<SDL_Rect> rects, UIElement* parent, bool active, j1Module* listener, char* path, bool interactive)
{
	UIImage* image = new UIImage(name, position, atlas, rects);
	image->parent = parent;
	if (parent)
		parent->childs.PushBack(image);
	image->listener = listener;
	image->interactive = interactive;
	image->active = active;
	uiElements.add(image);

	return image;
}

UILabel* j1Gui::CreateText(char* name, iPoint position, char* text, UIElement* parent, bool active, j1Module* listener, TTF_Font* font, SDL_Color color, bool interactive)
{
	UILabel* label = new UILabel(name, position, text);
	
	label->SetNewTexture(text, color, font);
	label->SetParent(parent);

	label->listener = listener;
	label->interactive = interactive;
	label->active = active;

	uiElements.add(label);

	return label;
}

UIButton* j1Gui::CreateButton(char* name, iPoint position, UIImage* image, UIElement* parent, bool active, j1Module* listener)
{
	UIButton* button = new UIButton(name, position, image);

	button->SetParent(parent);
	image->SetParent(button);

	button->listener = listener;
	button->interactive = true;
	button->active = active;

	uiElements.add(button);

	return button;
}

UICheckingButton* j1Gui::CreateCheckingButton(char* name, iPoint position, UIImage* image, UIImage* image2, UIElement* parent, bool active, j1Module* listener)
{
	UICheckingButton* checkingButton = new UICheckingButton(name, position, image, image2);
	checkingButton->SetParent(parent);
	image->SetParent(checkingButton);
	image2->SetParent(checkingButton);

	checkingButton->listener = listener;
	checkingButton->interactive = true;
	checkingButton->active = active;

	uiElements.add(checkingButton);

	return checkingButton;
}

UIScrollBar* j1Gui::CreateScrollBar(char* name, iPoint position, UIElement* image, UIElement* thumb, UIElement* parent, ScrollBar_Types type, int offsetL, int offsetR, int offsetU, int offsetD, bool active, j1Module* listener)
{
	UIScrollBar* scrollBar = new UIScrollBar(name, position, image, thumb);

	scrollBar->SetParent(parent);
	if (image)
		image->SetParent(scrollBar);
	if (thumb)
		thumb->SetParent(scrollBar);

	scrollBar->offsetL = offsetL;
	scrollBar->offsetR = offsetR;
	scrollBar->offsetU = offsetU;
	scrollBar->offsetD = offsetD;

	scrollBar->type = type;

	scrollBar->active = active;
	scrollBar->listener = listener;

	scrollBar->thumb->SetLocalPosition(offsetL, offsetU);

	uiElements.add(scrollBar);

	return scrollBar;
}
UIInputText* j1Gui::CreateInputText(char* name, SDL_Rect rect, UIImage* image, UILabel* label, UIElement* parent, int offsetX, int offsetY, bool active, j1Module* listener)
{
	UIInputText* inputText = new UIInputText(name, rect, image, label, offsetX, offsetY);

	inputText->SetParent(parent);
	if (image)
		image->SetParent(inputText);
	if (label)
		label->SetParent(inputText);

	inputText->listener = listener;
	inputText->active = active;
	inputText->interactive = true;

	uiElements.add(inputText);

	return inputText;
}

UIRect* j1Gui::CreateRect(char* name, SDL_Rect rect, int newR, int newG, int newB, int newA, bool newFilled, UIElement* parent, bool active, j1Module* listener)
{
	UIRect* newRect = new UIRect(name, rect, newR, newG, newB, newA, newFilled);
	newRect->SetParent(parent);
	newRect->active = active;
	newRect->listener = listener;
	newRect->interactive = false;

	uiElements.add(newRect);

	return newRect;
}

UIBar* j1Gui::CreateBar(char* newName, UIElement* background, UIElement* fillImage, int* maxValue, int* currValue)
{
	UIBar* bar = new UIBar(newName, background, fillImage, maxValue, currValue);
	if (background)
	{
		bar->SetCollider(background->GetWorldRect());
		background->SetParent(bar);
	}
	if (fillImage)
	{
		fillImage->SetParent(bar);
	}
	bar->interactive = false;

	uiElements.add(bar);

	return bar;
}

void j1Gui::DeleteElement(UIElement* element)
{
	p2List_item<UIElement*> item = *uiElements.At(uiElements.find(element));
	uiElements.del(&item);
}

const UIElement* j1Gui::GetMouseHover() const
{
	/*
	iPoint mouse;
	App->input->GetMousePosition(mouse.x, mouse.y);
	SDL_Rect r;
	for (p2List_item<UIElement*>* item = uiElements.end; item; item = item->prev)
	{
		if (item->data->interactive == true)
		{
		//	r = item->data->Get
		}
	}
	*/
	return NULL;

}

// const getter for atlas
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

void j1Gui::SetHovering(UIElement* element)
{
	hoveringElement = element;
}

void j1Gui::SetFocus(UIElement* newFocus)
{
	if (focus)
	{
		focus->OnLooseFocus();
	}
	focus = newFocus;
	if (newFocus)
	{
	focus->OnGetFocus();
	}
}

UIElement* j1Gui::GetFocus() const
{
	return focus;
}

//Compress this function
UIElement* j1Gui::GetNextFocus()
{
	p2List_item<UIElement*>* item = NULL;
	UIElement* ret = NULL;
	UIElement* firstFocus = NULL;

	item = uiElements.start;
	bool end = false;
	bool afterFocus = false;

	while (item && !end)
	{
		if (focus == NULL)
		{
			afterFocus = true;
		}
		if (!firstFocus)
		{
			if (item->data->active && item->data->interactive && item->data != screen_button)
			{
				firstFocus = item->data;
			}
		}
		if (item->prev && item->prev->data == focus)
		{
			afterFocus = true;
		}

		if (item->data->active && item->data->interactive && afterFocus && item->data != screen_button)
		{
			LOG("Focus: %s", item->data->name.GetString());
			ret = item->data;
			end = true;
		}

		item = item->next;
	}
	if (!end)
	{
		ret = firstFocus;
	}
	if (focus)
	{
		focus->OnLooseFocus();
	}
	if (ret)
	{
		ret->OnGetFocus();
	}
	return ret;
}
/*
UIElement* j1Gui::GetHovering() const
{
	return hoveringElement;
}*/
void j1Gui::DrawFocus() const
{
	if (focus)
	{
		SDL_Rect rect = focus->GetWorldRect();
		rect.x = focus->GetWorldRect().x - App->render->camera.x;
		rect.y = focus->GetWorldRect().y - App->render->camera.y;
		App->render->DrawQuad(rect, 255, 0, 0, 50, false);
	}
}

void j1Gui::ClickFocus()
{
	if (focus)
	{
		focus->listener->OnGUI(MOUSE_DOWN, focus);
		focus->OnMouseDown();
	}
}

void j1Gui::UnclickFocus()
{
	if (focus)
	{
		focus->listener->OnGUI(MOUSE_UP, focus);
		if (focus)
			focus->OnMouseUp();
	}
}

void j1Gui::LooseFocus()
{
	if (focus)
	{
		focus->OnLooseFocus();
	}
	focus = NULL;
}

void j1Gui::SendNewInput(char* text)
{
	UIInputText* inputText = (UIInputText*)focus;
	inputText->GetNewInput(text);
}

void j1Gui::DrawCursorInput()
{
	int mouseX, mouseY;
	App->input->GetMousePosition(mouseX, mouseY);
	App->render->Blit(cursorInput, mouseX - 10 - App->render->camera.x, mouseY - 10 - App->render->camera.y, &cursorInput_rect);
}

void j1Gui::DrawHoveringImage()
{
	int x, y, w, h, offsetX = 7;
	App->input->GetMousePosition(x, y);
	SDL_QueryTexture(hoveringElement->hoveringTexture, NULL, NULL, &w, &h);
	if (x + offsetX + w > App->render->camera.w)
	{
		offsetX = -w - 20;
	}
	App->render->Blit(hoveringElement->hoveringTexture, x + offsetX - App->render->camera.x, y + 18 - App->render->camera.y);
}

UIElement* j1Gui::GetScreen() const
{
	return screen_button;
}
// class Gui ---------------------------------------------------

#pragma region Commands
void j1Gui::C_UIDebug::function(const p2DynArray<p2SString>* arg)
{
	if (arg->Count() > 1)
	{
		p2SString str = arg->At(1)->GetString();
		if (str == "enable")
		{
			App->gui->debugMode = true;
			LOG("-- GUI: Debug mode enabled --");
		}
		else if (str == "disable")
		{
			App->gui->debugMode = false;
			LOG("-- GUI: Debug mode disabled --");
		}
		else
			LOG("gui_debug: unexpected command '%s', expecting enable / disable", arg->At(1)->GetString());
	}
	else
		LOG("'%s': not enough arguments, expecting enable / disable", arg->At(0)->GetString());

}

#pragma endregion