/*
UIElement::UIElement()
{}
UIElement::~UIElement()
{}
void UIElement::Deactivate()
{
	for (uint i = 0; i < childs.Count(); i++)
	{
		childs[i]->Deactivate();
	}
	if (this == App->gui->GetFocus())
	{
		App->gui->LooseFocus();
	}
	active = false;
}

void UIElement::Activate()
{
	for (uint i = 0; i < childs.Count(); i++)
	{
		childs[i]->Activate();
	}

	active = true;
}

void UIElement::SetCamera(bool set)
{
	for (uint i = 0; i < childs.Count(); i++)
	{
		childs[i]->useCamera = set;
	}
	useCamera = set;
}

//Checking Button
UICheckingButton::UICheckingButton()
{}
UICheckingButton::UICheckingButton(char* newName, iPoint newPosition, UIImage* newImage1, UIImage* newImage2)
{
	name = newName;
	SetCollider(newPosition.x, newPosition.y, newImage1->GetWorldRect().w, newImage1->GetWorldRect().h);
	defImage = newImage1;
	checkedImage = newImage2;
}
UICheckingButton::~UICheckingButton()
{}
void UICheckingButton::OnMouseDown()
{}
void UICheckingButton::OnMouseUp()
{
	checked = !checked;
	if (checked)
	{
		defImage->Deactivate();
		checkedImage->Activate();
	}
	else
	{
		checkedImage->Deactivate();
		defImage->Activate();
	}
}
*/