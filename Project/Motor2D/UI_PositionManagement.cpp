#include "j1Gui.h"
#include "UIElements.h"

void UIElement::SetCollider(SDL_Rect rect)
{
	collider = rect;
}

void UIElement::SetCollider(int x, int y, int w, int h)
{
	SetLocalPosition(x, y);
	collider.w = w;	 collider.h = h;
}

void UIElement::SetLocalPosition(int x, int y)
{
	collider.x = x;
	collider.y = y;
}

void UIElement::SetGlobalPosition(int x, int y)
{
	//(GetWorldRect - collider) is the world position of the parent
	collider.x = x - (GetWorldRect().x - collider.x);
	collider.y = y - (GetWorldRect().y - collider.y);
}

void UIElement::Center(UIElement* element)
{
	SDL_Rect elementRect = element->GetWorldRect();
	int centerX = elementRect.x + elementRect.w / 2;
	int centerY = elementRect.y + elementRect.h / 2;

	SetGlobalPosition(centerX - collider.w / 2, centerY - collider.h / 2);
}
void UIElement::Center(iPoint point)
{
	SetGlobalPosition(point.x - collider.w / 2, point.y - collider.h / 2);
}

void UIElement::Center_x(UIElement* element)
{
	int centerX = element->GetWorldRect().x + element->collider.w / 2;
	int currentY = GetWorldRect().y;
	SetGlobalPosition(centerX - collider.w / 2, currentY);
}
void UIElement::Center_x(int x)
{
	SDL_Rect rect = GetWorldRect();
	SetGlobalPosition(x - collider.w / 2, rect.y);
}

void UIElement::Center_y(UIElement* element)
{
	int centerY = element->GetWorldRect().y + element->collider.h / 2;
	int currentX = GetWorldRect().x;
	SetGlobalPosition(currentX, centerY - collider.h / 2);
}
void UIElement::Center_y(int y)
{
	SDL_Rect rect = GetWorldRect();
	SetGlobalPosition(rect.x, y - collider.h);
}

void UIElement::Align(UIElement* element)
{
	SDL_Rect elementRect = element->GetWorldRect();
	SetGlobalPosition(elementRect.x, elementRect.y);
}

void UIElement::Align_x(UIElement* element)
{
	SDL_Rect elementRect = element->GetWorldRect();
	int currentY = GetWorldRect().y;
	SetGlobalPosition(elementRect.x, currentY);

}
void UIElement::Align_y(UIElement* element)
{
	SDL_Rect elementRect = element->GetWorldRect();
	int currentX = GetWorldRect().x;
	SetGlobalPosition(currentX, elementRect.y);
}

iPoint UIElement::GetLocalPosition() const
{
	iPoint ret{ collider.x, collider.y };
	return ret;
}

SDL_Rect UIElement::GetWorldRect() const
{
	SDL_Rect rect = collider;
	UIElement* item = (UIElement*)parent;
	for (UIElement* item = parent; item; item = item->parent)
	{
		rect.x += item->collider.x;
		rect.y += item->collider.y;
	}

	return rect;
}