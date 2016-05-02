#include "M_GUI.h"
#include "UI_Element.h"

#include "j1App.h"
#include "M_Fonts.h"

//----------------  FACTORY   -------------------------------

UI_Image* M_GUI::CreateUI_Image(SDL_Rect position, SDL_Texture* texture, SDL_Rect rect, SDL_Rect collider)
{
	if (position.w == 0 || position.h == 0)
	{
		position.w = rect.w; position.h = rect.h;
	}

	UI_Image* image = new UI_Image(position.x, position.y, position.w, position.h, texture, rect, collider);
	UI_Elements.push_back(image);
	return image;
}


UI_Rect* M_GUI::CreateUI_Rect(SDL_Rect position, uint r, uint g, uint b, uint a, SDL_Rect collider)
{
	UI_Rect* rect = new UI_Rect(position.x, position.y, position.w, position.h, r, g ,b, a, collider);
	UI_Elements.push_back(rect);
	return rect;
}

UI_Button* M_GUI::CreateUI_Button(SDL_Rect position, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect collider)
{
	UI_Button* Button = new UI_Button(position.x, position.y, position.w, position.h, button, hover, clicked, collider);
	UI_Elements.push_back(Button);
	Button->localPosition.w = button.w; Button->localPosition.h = button.h;
	if (Button->collider.w == 0 || Button->collider.h == 0)
	{
		Button->collider.w = button.w; Button->collider.h = button.h;
	}
	return Button;
}

// UI Button 2

UI_Button2*  M_GUI::CreateUI_Button2(const SDL_Rect& position, SDL_Texture* tex, const SDL_Rect& _button, const SDL_Rect& _clicked, bool _toRender, SDL_Rect collider)
{
	UI_Button2* generated = new UI_Button2(position.x, position.y, position.w, position.h, tex, _button, _clicked, collider);

	UI_Elements.push_back(generated);



	generated->localPosition.w = _button.w; generated->localPosition.h = _button.h;

	if (generated->collider.w == 0 || generated->collider.h == 0)
	{
		generated->collider.w = _button.w; generated->collider.h = _button.h;
	}

	generated->SetActive(_toRender);
	return generated;
}

UI_Label* M_GUI::CreateUI_Label(SDL_Rect position, char* text, _TTF_Font* typo, SDL_Rect collider)
{
	UI_Label* label = new UI_Label(position.x, position.y, position.w, position.h, text, typo, collider);
	if (label->localPosition.w == 0 || label->localPosition.h == 0)
	{
		SDL_QueryTexture(label->GetTexture(), NULL, NULL, &label->localPosition.w, &label->localPosition.h);
	}

	if (label->collider.w == 0 || label->collider.h == 0)
	{
		label->collider.w = label->localPosition.w;
		label->collider.h = label->localPosition.h;
	}
	UI_Elements.push_back(label);
	return label;
}

UI_Collapse* M_GUI::CreateUI_Collapse(SDL_Rect position, UI_Element* toLink, SDL_Rect collapsed, SDL_Rect opened, SDL_Rect collider)
{
	SDL_Rect pos = position;
	UI_Collapse* Button = new UI_Collapse(position.x, position.y, position.w, position.h, toLink, collapsed, opened, collider);
	UI_Elements.push_back(Button);
	if (Button->localPosition.w == 0 || Button->localPosition.h == 0)
	{
		Button->localPosition.w = collapsed.w;
		Button->localPosition.h = collapsed.h;
	}
	if (Button->collider.w == 0|| Button->collider.h == 0)
	{
		Button->collider.w = Button->localPosition.w;
		Button->collider.h = Button->localPosition.h;
	}
	toLink->SetParent(Button);

	return Button;
}

UI_ProgressBar* M_GUI::CreateUI_ProgressBar(SDL_Rect position, SDL_Texture* texture,  int* maxData, int* currentData, SDL_Rect rect)
{
	SDL_Rect pos = position;
	UI_ProgressBar* bar = new UI_ProgressBar(position.x, position.y, position.w, position.h, texture, rect, maxData, currentData);

	if (rect.w == 0 || rect.h == 0)
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}
	bar->SetRect(rect);

	UI_Elements.push_back(bar);
	return bar;
}

UI_HPBar* M_GUI::CreateUI_HPBar(SDL_Rect position, SDL_Texture* hp_tex, SDL_Texture* shield_tex, SDL_Texture* back_tex, int* maxData, int* currentData, int* maxShield, int* currShield)
{
	SDL_Rect pos = position;
	UI_HPBar* bar = new UI_HPBar(position.x, position.y, position.w, position.h, hp_tex, shield_tex, back_tex, currentData, maxData, currShield, maxShield);

	UI_Elements.push_back(bar);
	return bar;
}
UI_InputText* M_GUI::CreateUI_InputText(int x, int y, char* _defaultText, SDL_Rect collider, int offsetX, int offsetY)
{
	UI_InputText* inp = new UI_InputText(x, y, 0, 0, _defaultText, collider, offsetX, offsetY);
	inp->AddListener(this);

	UI_Elements.push_back(inp);
	return inp;
}