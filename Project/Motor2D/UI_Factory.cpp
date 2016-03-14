#include "M_GUI.h"
#include "UI_Element.h"

#include "j1App.h"
#include "M_Fonts.h"

//----------------  FACTORY   -------------------------------


UI_Image* M_GUI::CreateUI_Image(SDL_Rect position, char* path, SDL_Rect rect, SDL_Rect collider)
{
	int id = UI_Elements.count();
	UI_Image* image = new UI_Image(position.x, position.y, position.w, position.h, rect, path, collider);
	UI_Elements.add(image);
	if (image->localPosition.w == 0 || image->localPosition.h == 0)
	{
		SDL_QueryTexture(image->GetTexture(), NULL, NULL, &image->localPosition.w, &image->localPosition.h);
		if (collider.w == 0 || collider.h == 0)
		{
			image->collider.w = image->localPosition.w;
			image->collider.h = image->localPosition.h;
		}
	}
	return image;
}

UI_Image* M_GUI::CreateUI_Image(SDL_Rect position, char* path, SDL_Rect collider)
{
	int id = UI_Elements.count();
	UI_Image* image = new UI_Image(position.x, position.y, position.w, position.h, path, collider);
	UI_Elements.add(image);
	if (image->localPosition.w == 0 || image->localPosition.h == 0)
	{
		SDL_QueryTexture(image->GetTexture(), NULL, NULL, &image->localPosition.w, &image->localPosition.h);
		if (collider.w == 0 || collider.h == 0)
		{
			image->collider.w = image->localPosition.w;
			image->collider.h = image->localPosition.h;
		}
	}
	return image;
}

UI_Image* M_GUI::CreateUI_Image(SDL_Rect position, SDL_Texture* texture, SDL_Rect rect, SDL_Rect collider)
{
	int id = UI_Elements.count();
	if (position.w == 0 || position.h == 0)
	{
		position.w = rect.w; position.h = rect.h;
	}

	UI_Image* image = new UI_Image(position.x, position.y, position.w, position.h, texture, rect, collider);
	UI_Elements.add(image);
	return image;
}


UI_Rect* M_GUI::CreateUI_Rect(SDL_Rect position, uint r, uint g, uint b, uint a, SDL_Rect collider)
{
	int id = UI_Elements.count();

	UI_Rect* rect = new UI_Rect(position.x, position.y, position.w, position.h, r, g ,b, a, collider);
	UI_Elements.add(rect);
	return rect;
}


UI_Button* M_GUI::CreateUI_Button(SDL_Rect position, char* path, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect collider)
{
	int id = UI_Elements.count();
	SDL_Rect pos = position;
	UI_Button* Button = new UI_Button(position.x, position.y, position.w, position.h, path, button, hover, clicked, collider);
	UI_Elements.add(Button);
	Button->localPosition.w = button.w; Button->localPosition.h = button.h;
	if (Button->collider.w == 0 || Button->collider.h == 0)
	{
		Button->collider.w = button.w; Button->collider.h = button.h;
	}
	return Button;
}

UI_Button* M_GUI::CreateUI_Button(SDL_Rect position, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect collider)
{
	int id = UI_Elements.count();
	UI_Button* Button = new UI_Button(position.x, position.y, position.w, position.h, button, hover, clicked, collider);
	UI_Elements.add(Button);
	Button->localPosition.w = button.w; Button->localPosition.h = button.h;
	if (Button->collider.w == 0 || Button->collider.h == 0)
	{
		Button->collider.w = button.w; Button->collider.h = button.h;
	}
	return Button;
}

// UI Button 2

UIButton2*  M_GUI::CreateUIButton2(const SDL_Rect& position, char* path, const SDL_Rect& _button, const SDL_Rect& _clicked, bool _toRender, SDL_Rect collider)
{
	UIButton2* generated = new UIButton2(position.x, position.y, position.w, position.h, path, _button, _clicked, _toRender, collider);

	UI_Elements.add(generated);

	generated->localPosition.w = _button.w; generated->localPosition.h = _button.h;


	if (generated->collider.w == 0 || generated->collider.h == 0)
	{
		generated->collider.w = _button.w; generated->collider.h = _button.h;
	}

	return generated;
}

UIButton2*  M_GUI::CreateUIButton2(const SDL_Rect& position, SDL_Texture* tex, const SDL_Rect& _button, const SDL_Rect& _clicked, bool _toRender, SDL_Rect collider)
{
	UIButton2* generated = new UIButton2(position.x, position.y, position.w, position.h, tex, _button, _clicked, _toRender, collider);

	UI_Elements.add(generated);

	generated->localPosition.w = _button.w; generated->localPosition.h = _button.h;

	if (generated->collider.w == 0 || generated->collider.h == 0)
	{
		generated->collider.w = _button.w; generated->collider.h = _button.h;
	}

	return generated;
}

UI_AnimatedImage* M_GUI::CreateUI_AnimatedImage(SDL_Rect position, char* path, SDL_Rect _rect[], uint nFrames, float _speed = 25.0f, SDL_Rect collider)
{
	int id = UI_Elements.count();
	UI_AnimatedImage* anim = new UI_AnimatedImage(position.x, position.y, position.w, position.h, path, _rect, nFrames, _speed, collider);
	UI_Elements.add(anim);
	return anim;
}

UI_AnimatedImage* M_GUI::CreateUI_AnimatedImage(SDL_Rect position, SDL_Rect _rect[], uint nFrames, float _speed = 25.0f, SDL_Rect collider)
{
	int id = UI_Elements.count();
	UI_AnimatedImage* anim = new UI_AnimatedImage(position.x, position.y, position.w, position.h, _rect, nFrames, _speed, collider);
	UI_Elements.add(anim);
	return anim;
}


UI_Label* M_GUI::CreateUI_Label(SDL_Rect position, char* text, char* fontPath, int fontSize, SDL_Rect collider)
{
	int id = UI_Elements.count();

	_TTF_Font* typo;
	//LoadFont
	if (fontPath != "")
	{
		typo = App->font->Load(fontPath, fontSize);
	}
	else
	{
		typo = NULL;
	}

	return CreateUI_Label(position, text, typo, collider);
}

UI_Label* M_GUI::CreateUI_Label(SDL_Rect position, char* text, _TTF_Font* typo, SDL_Rect collider)
{
	int id = UI_Elements.count();

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
	UI_Elements.add(label);
	return label;
}

UI_Collapse* M_GUI::CreateUI_Collapse(SDL_Rect position, UI_Element* toLink, SDL_Rect collapsed, SDL_Rect opened, SDL_Rect collider)
{
	int id = UI_Elements.count();
	SDL_Rect pos = position;
	UI_Collapse* Button = new UI_Collapse(position.x, position.y, position.w, position.h, toLink, collapsed, opened, collider);
	UI_Elements.add(Button);
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
	UI_ProgressBar* Bar = new UI_ProgressBar(position.x, position.y, position.w, position.h, texture, rect, maxData, currentData);

	if (rect.w == 0 || rect.h == 0)
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}
	Bar->SetRect(rect);

	UI_Elements.add(Bar);
	return Bar;
}

UI_ProgressBar* M_GUI::CreateUI_ProgressBar(SDL_Rect position, char* path, int* maxData, int* currentData, SDL_Rect rect)
{
	SDL_Rect pos = position;
	UI_ProgressBar* Bar = new UI_ProgressBar(position.x, position.y, position.w, position.h, path, rect, maxData, currentData);

	if (rect.w == 0 || rect.h == 0)
	{
		SDL_QueryTexture(Bar->GetTexture(), NULL, NULL, &rect.w, &rect.h);
	}
	Bar->SetRect(rect);

	UI_Elements.add(Bar);
	return Bar;
}

UI_InputText* M_GUI::CreateUI_InputText(int x, int y, char* _defaultText, SDL_Rect collider, int offsetX, int offsetY)
{
	UI_InputText* inp = new UI_InputText(x, y, 0, 0, _defaultText, collider, offsetX, offsetY);
	inp->AddListener(this);

	UI_Elements.add(inp);
	return inp;
}