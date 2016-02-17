#include "j1Gui.h"
#include "UI_Element.h"



//----------------  FACTORY   -------------------------------


UIImage* j1Gui::CreateUIImage(SDL_Rect position, char* path, SDL_Rect rect, SDL_Rect collider)
{
	int id = UIElements.count();
	UIImage* image = new UIImage(id, position.x, position.y, position.w, position.h, rect, path, collider);
	UIElements.add(image);
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

UIImage* j1Gui::CreateUIImage(SDL_Rect position, char* path, SDL_Rect collider)
{
	int id = UIElements.count();
	UIImage* image = new UIImage(id, position.x, position.y, position.w, position.h, path, collider);
	UIElements.add(image);
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

UIImage* j1Gui::CreateUIImage(SDL_Rect position, SDL_Rect rect, SDL_Rect collider)
{
	int id = UIElements.count();
	if (position.w == 0 || position.h == 0)
	{
		position.w = rect.w; position.h = rect.h;
	}

	UIImage* image = new UIImage(id, position.x, position.y, position.w, position.h, rect, collider);
	UIElements.add(image);
	return image;
}


UIRect* j1Gui::CreateUIRect(SDL_Rect position, uint r, uint g, uint b, uint a, SDL_Rect collider)
{
	int id = UIElements.count();

	UIRect* rect = new UIRect(id, position.x, position.y, position.w, position.h, r, g ,b, a, collider);
	UIElements.add(rect);
	return rect;
}


UIButton* j1Gui::CreateUIButton(SDL_Rect position, char* path, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect collider)
{
	int id = UIElements.count();
	SDL_Rect pos = position;
	UIButton* Button = new UIButton(id, position.x, position.y, position.w, position.h, path, button, hover, clicked, collider);
	UIElements.add(Button);
	Button->localPosition.w = button.w; Button->localPosition.h = button.h;
	if (Button->collider.w == 0 || Button->collider.h == 0)
	{
		Button->collider.w = button.w; Button->collider.h = button.h;
	}
	return Button;
}

UIButton* j1Gui::CreateUIButton(SDL_Rect position, SDL_Rect button, SDL_Rect hover, SDL_Rect clicked, SDL_Rect collider)
{
	int id = UIElements.count();
	UIButton* Button = new UIButton(id, position.x, position.y, position.w, position.h, button, hover, clicked, collider);
	UIElements.add(Button);
	Button->localPosition.w = button.w; Button->localPosition.h = button.h;
	if (Button->collider.w == 0 || Button->collider.h == 0)
	{
		Button->collider.w = button.w; Button->collider.h = button.h;
	}
	return Button;
}


UIAnimatedImage* j1Gui::CreateUIAnimatedImage(SDL_Rect position, char* path, SDL_Rect _rect[], uint nFrames, float _speed = 25.0f, SDL_Rect collider)
{
	int id = UIElements.count();
	UIAnimatedImage* anim = new UIAnimatedImage(id, position.x, position.y, position.w, position.h, path, _rect, nFrames, _speed, collider);
	UIElements.add(anim);
	return anim;
}

UIAnimatedImage* j1Gui::CreateUIAnimatedImage(SDL_Rect position, SDL_Rect _rect[], uint nFrames, float _speed = 25.0f, SDL_Rect collider)
{
	int id = UIElements.count();
	UIAnimatedImage* anim = new UIAnimatedImage(id, position.x, position.y, position.w, position.h, _rect, nFrames, _speed, collider);
	UIElements.add(anim);
	return anim;
}


UILabel* j1Gui::CreateUILabel(SDL_Rect position, char* text, char* fontPath, int fontSize, UI_LabelAlineation _alineation, SDL_Rect collider)
{
	int id = UIElements.count();

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

	return CreateUILabel(position, text, _alineation, typo, collider);
}

UILabel* j1Gui::CreateUILabel(SDL_Rect position, char* text, UI_LabelAlineation _alineation, _TTF_Font* typo, SDL_Rect collider)
{
	int id = UIElements.count();

	UILabel* label = new UILabel(id, position.x, position.y, position.w, position.h, text, _alineation, typo, collider);
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
	UIElements.add(label);
	return label;
}


UIInputBox* j1Gui::CreateUIInputBox(SDL_Rect position, SDL_Rect button, SDL_Rect focused, char* text, int margin, UI_LabelAlineation _alineation, _TTF_Font* typo, SDL_Rect collider)
{
	int id = UIElements.count();
	SDL_Rect pos = position;
	UIInputBox* Box = new UIInputBox(id, position.x, position.y, position.w, position.h, button, focused, text, margin, _alineation, typo, collider);
	UIElements.add(Box);
	Box->localPosition.w = button.w; Box->localPosition.h = button.h;
	if (Box->collider.w == 0 || Box->collider.h == 0)
	{
		Box->collider.w = Box->localPosition.w;
		Box->collider.h = Box->localPosition.h;
	}
	return Box;
}


UICollapse* j1Gui::CreateUICollapse(SDL_Rect position, UIElement* toLink, SDL_Rect collapsed, SDL_Rect opened, SDL_Rect collider)
{
	int id = UIElements.count();
	SDL_Rect pos = position;
	UICollapse* Button = new UICollapse(id, position.x, position.y, position.w, position.h, toLink, collapsed, opened, collider);
	UIElements.add(Button);
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

UIProgressBar* j1Gui::CreateUIProgressBar(SDL_Rect position, SDL_Rect image, int* maxData, int* currentData)
{
	int id = UIElements.count();
	SDL_Rect pos = position;
	UIProgressBar* Bar = new UIProgressBar(id, position.x, position.y, position.w, position.h, image, maxData, currentData);
	UIElements.add(Bar);
	return Bar;
}


UISlider* j1Gui::CreateUISlider(SDL_Rect position, j1Module* listener, SDL_Rect bar, SDL_Rect thumb, int _leftOffest, int _rightOfset, int _thumbOffset, SDL_Rect _barCollider, SDL_Rect _thumbCollider)
{
	if (listener)
	{
		UIImage* _thumb = CreateUIImage({ _leftOffest, _thumbOffset, 0, 0 }, thumb, _thumbCollider);

		int id = UIElements.count();
		SDL_Rect pos = position;
		UISlider* Slider = new UISlider(id, position.x, position.y, position.w, position.h, bar, _thumb, _leftOffest, _rightOfset, _barCollider);
		UIElements.InsertAfter(_thumb->GetId() - 1, Slider);
		_thumb->AddListener(listener);
		Slider->AddListener(listener);

		UIImage* _bar = Slider->GetBar();
		if (_bar->localPosition.w == 0 || _bar->localPosition.h == 0)
		{
			_bar->localPosition.w = Slider->localPosition.w = bar.w;
			_bar->localPosition.h = Slider->localPosition.h = bar.h;
		}
		return Slider;
	}
	return NULL;
}