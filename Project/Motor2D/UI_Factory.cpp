#include "j1Gui_D.h"
#include "UI_D_Element.h"



//----------------  FACTORY   -------------------------------


UI_D_Image* j1Gui_D::CreateUI_D_Image(SDL_Rect position, char* path, SDL_Rect rect, SDL_Rect collider)
{
	int id = UI_D_Elements.count();
	UI_D_Image* image = new UI_D_Image(position.x, position.y, position.w, position.h, rect, path, collider);
	UI_D_Elements.add(image);
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

UI_D_Image* j1Gui_D::CreateUI_D_Image(SDL_Rect position, char* path, SDL_Rect collider)
{
	int id = UI_D_Elements.count();
	UI_D_Image* image = new UI_D_Image(position.x, position.y, position.w, position.h, path, collider);
	UI_D_Elements.add(image);
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

UI_D_Image* j1Gui_D::CreateUI_D_Image(SDL_Rect position, SDL_Texture* texture, SDL_Rect rect, SDL_Rect collider)
{
	int id = UI_D_Elements.count();
	if (position.w == 0 || position.h == 0)
	{
		position.w = rect.w; position.h = rect.h;
	}

	UI_D_Image* image = new UI_D_Image(position.x, position.y, position.w, position.h, texture, rect, collider);
	UI_D_Elements.add(image);
	return image;
}


UI_D_Rect* j1Gui_D::CreateUI_D_Rect(SDL_Rect position, uint r, uint g, uint b, uint a, SDL_Rect collider)
{
	int id = UI_D_Elements.count();

	UI_D_Rect* rect = new UI_D_Rect(position.x, position.y, position.w, position.h, r, g ,b, a, collider);
	UI_D_Elements.add(rect);
	return rect;
}


UI_D_Button* j1Gui_D::CreateUI_D_Button(SDL_Rect position, char* path, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect collider)
{
	int id = UI_D_Elements.count();
	SDL_Rect pos = position;
	UI_D_Button* Button = new UI_D_Button(position.x, position.y, position.w, position.h, path, button, hover, clicked, collider);
	UI_D_Elements.add(Button);
	Button->localPosition.w = button.w; Button->localPosition.h = button.h;
	if (Button->collider.w == 0 || Button->collider.h == 0)
	{
		Button->collider.w = button.w; Button->collider.h = button.h;
	}
	return Button;
}

UI_D_Button* j1Gui_D::CreateUI_D_Button(SDL_Rect position, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect collider)
{
	int id = UI_D_Elements.count();
	UI_D_Button* Button = new UI_D_Button(position.x, position.y, position.w, position.h, button, hover, clicked, collider);
	UI_D_Elements.add(Button);
	Button->localPosition.w = button.w; Button->localPosition.h = button.h;
	if (Button->collider.w == 0 || Button->collider.h == 0)
	{
		Button->collider.w = button.w; Button->collider.h = button.h;
	}
	return Button;
}


UI_D_AnimatedImage* j1Gui_D::CreateUI_D_AnimatedImage(SDL_Rect position, char* path, SDL_Rect _rect[], uint nFrames, float _speed = 25.0f, SDL_Rect collider)
{
	int id = UI_D_Elements.count();
	UI_D_AnimatedImage* anim = new UI_D_AnimatedImage(position.x, position.y, position.w, position.h, path, _rect, nFrames, _speed, collider);
	UI_D_Elements.add(anim);
	return anim;
}

UI_D_AnimatedImage* j1Gui_D::CreateUI_D_AnimatedImage(SDL_Rect position, SDL_Rect _rect[], uint nFrames, float _speed = 25.0f, SDL_Rect collider)
{
	int id = UI_D_Elements.count();
	UI_D_AnimatedImage* anim = new UI_D_AnimatedImage(position.x, position.y, position.w, position.h, _rect, nFrames, _speed, collider);
	UI_D_Elements.add(anim);
	return anim;
}


UI_D_Label* j1Gui_D::CreateUI_D_Label(SDL_Rect position, char* text, char* fontPath, int fontSize, UI_LabelAlineation _alineation, SDL_Rect collider)
{
	int id = UI_D_Elements.count();

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

	return CreateUI_D_Label(position, text, _alineation, typo, collider);
}

UI_D_Label* j1Gui_D::CreateUI_D_Label(SDL_Rect position, char* text, UI_LabelAlineation _alineation, _TTF_Font* typo, SDL_Rect collider)
{
	int id = UI_D_Elements.count();

	UI_D_Label* label = new UI_D_Label(position.x, position.y, position.w, position.h, text, _alineation, typo, collider);
	if (label->localPosition.w == 0 || label->localPosition.h == 0)
	{
		SDL_QueryTexture(label->GetTexture(), NULL, NULL, &label->localPosition.w, &label->localPosition.h);
	}
	if (_alineation == UI_AlignCenter)
	{
		label->localPosition.x -= label->localPosition.w / 2;
	}
	if (_alineation == UI_AlignRight)
	{
		label->localPosition.x -= label->localPosition.w;
	}
	if (label->collider.w == 0 || label->collider.h == 0)
	{
		label->collider.w = label->localPosition.w;
		label->collider.h = label->localPosition.h;
	}
	UI_D_Elements.add(label);
	return label;
}

UI_D_Collapse* j1Gui_D::CreateUI_D_Collapse(SDL_Rect position, UI_D_Element* toLink, SDL_Rect collapsed, SDL_Rect opened, SDL_Rect collider)
{
	int id = UI_D_Elements.count();
	SDL_Rect pos = position;
	UI_D_Collapse* Button = new UI_D_Collapse(position.x, position.y, position.w, position.h, toLink, collapsed, opened, collider);
	UI_D_Elements.add(Button);
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

UI_D_ProgressBar* j1Gui_D::CreateUI_D_ProgressBar(SDL_Rect position, SDL_Texture* texture,  int* maxData, int* currentData, SDL_Rect rect)
{
	SDL_Rect pos = position;
	UI_D_ProgressBar* Bar = new UI_D_ProgressBar(position.x, position.y, position.w, position.h, texture, rect, maxData, currentData);

	if (rect.w == 0 || rect.h == 0)
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}
	Bar->SetRect(rect);

	UI_D_Elements.add(Bar);
	return Bar;
}

UI_D_ProgressBar* j1Gui_D::CreateUI_D_ProgressBar(SDL_Rect position, char* path, int* maxData, int* currentData, SDL_Rect rect)
{
	SDL_Rect pos = position;
	UI_D_ProgressBar* Bar = new UI_D_ProgressBar(position.x, position.y, position.w, position.h, path, rect, maxData, currentData);

	if (rect.w == 0 || rect.h == 0)
	{
		SDL_QueryTexture(Bar->GetTexture(), NULL, NULL, &rect.w, &rect.h);
	}
	Bar->SetRect(rect);

	UI_D_Elements.add(Bar);
	return Bar;
}