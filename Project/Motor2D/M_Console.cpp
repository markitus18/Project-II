#include "M_Console.h"
#include "M_Render.h"
#include "M_Input.h"
#include "j1App.h"
#include "M_GUI.h"
#include "M_Fonts.h"
#include "UIElements.h"

void Command::function(const C_DynArray<C_String>* arg){ LOG("Executing command function"); }

M_Console::M_Console(bool start_enabled) : j1Module(start_enabled)
{
	name.create("console");
}

// Destructor
M_Console::~M_Console()
{}

// Called before render is available
bool M_Console::Awake(pugi::xml_node& config)
{
	bool ret = true;

	AddCommand(&c_commandList);
	AddCommand(&c_tagList);
	AddCommand(&c_closeConsole);
	AddCommand(&c_clearConsole);
	AddCommand(&c_Quit);

	return ret;
}

// Called before the first frame
bool M_Console::Start()
{
	_TTF_Font* inputFont = App->font->Load("fonts/open_sans/OpenSans-Regular.ttf", 16);

	// -----------------------------------
	consoleRect_D = App->gui->CreateUI_Rect({ 0, 0, App->render->camera.w, 350 }, 0, 0, 0, 200);
	consoleRect_D->layer = GUI_MAX_LAYERS;
	consoleRect_D->AddListener(this);

	inputRect_D = App->gui->CreateUI_Rect({ 0, 350, App->render->camera.w, 40 }, 130, 130, 130);
	inputRect_D->layer = GUI_MAX_LAYERS;

	inputText_D = App->gui->CreateUI_InputText(0, 350, "Command", { 0, 0, App->render->camera.w, 40 }, 10, 10);
	inputText_D->layer = GUI_MAX_LAYERS;
	inputText_D->AddListener(this);

	//Moving Miscellaneous tag to the last tag in the list
	bool found = false;
	for (uint i = 0; i < tags.Count(); i++)
	{
		if (tags[i] == "Miscellaneous" && i != tags.Count() - 1)
		{
			found = true;
		}
		if (found && i != tags.Count() - 1)
		{
			tags[i] = tags[i + 1];
		}
	}
	if (found)
		tags[tags.Count() - 1] = "Miscellaneous";

	Close();
	return true;
	
}

bool M_Console::PostUpdate(float dt)
{
	if (closeGame)
		return false;
	return true;
}
bool M_Console::Update(float dt)
{
	if (dragText)
	{
		int x, y;
		App->input->GetMouseMotion(x, y);
		if ((y > 0 && textStart + y <=0) || (y < 0 && textStart + outputHeight + y > 350 - 15))
		{
			int minY = 0;
			int maxY = inputText_D->GetWorldPosition().y - 20;

			textStart += y;
			for (uint n = 0; n < output.Count(); n++)
			{
				output[n]->SetActive(true);
				output[n]->localPosition.y += y;

				iPoint newPos = {output[n]->localPosition.x, output[n]->localPosition.y};

				if (newPos.y >= maxY || newPos.y < 0)
				{
					output[n]->SetActive(false);
				}

			}
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		if (App->console->isActive())
		{
			App->console->Close();
		}
		else
		{
			App->console->Open();
		}
	}

	return true;
}
// Called before quitting
bool M_Console::CleanUp()
{
	C_List_item<CVar*>* item = CVarList.start;
	while (item)
	{
		RELEASE(item->data);
		item = item->next;
	}
	return true;
}

void M_Console::OnGUI(GUI_EVENTS event, UI_Element* element)
{
	if (element == inputText_D)
	{
		if (event == UI_KEYBOARD_CLICK)
		{
			UI_InputText* input = (UI_InputText*)element;
			GetNewInput(input->GetString().GetString());
			input->DeleteText();
		}
	}

	if (element == consoleRect_D)
	{
		if (event == UI_MOUSE_DOWN)
		{
			dragText = true;
		}
		if (event == UI_MOUSE_UP || event == UI_MOUSE_EXIT)
		{
			dragText = false;
		}
	}

}
void M_Console::AddCommand(Command* command)
{
	commandList.add(command);

	bool found = false;
	bool std = false;
	for (uint i = 0; i < tags.Count(); i++)
	{
		if (command->tag == tags[i])
		{
			found = true;
		}
	}
	if (!found)
	{
		tags.PushBack(command->tag);
	}
}

uint M_Console::AddCVar(const char* newName, float* reference, j1Module* listener, bool serialize)
{
	CVar* newCVar = new CVar(newName, reference, serialize);
	newCVar->SetListener(listener);
	CVarList.add(newCVar);
	return CVarList.count() - 1;
}
uint M_Console::AddCVar(const char* newName, int* reference, j1Module* listener, bool serialize)
{
	CVar* newCVar = new CVar(newName, reference, serialize);
	newCVar->SetListener(listener);
	CVarList.add(newCVar);
	return CVarList.count() - 1;
}
uint M_Console::AddCVar(const char* newName, char* reference, j1Module* listener, bool serialize)
{
	CVar* newCVar = new CVar(newName, reference, serialize);
	newCVar->SetListener(listener);
	CVarList.add(newCVar);
	return CVarList.count() - 1;
}
uint M_Console::AddCVar(const char* newName, bool* reference, j1Module* listener, bool serialize)
{
	CVar* newCVar = new CVar(newName, reference, serialize);
	newCVar->SetListener(listener);
	CVarList.add(newCVar);
	return CVarList.count() - 1;
}

void M_Console::GetNewInput(const char* src)
{
	C_DynArray<C_String> input;
	CutString(src, &input);

	Command* command = FindCommand(input[0].GetString(), input.Count() - 1);
	if (command)
	{
		command->function(&input);
	}
	else
	{
		CVar* cVar = FindCVar(input.At(0)->GetString());
		if (cVar)
		{
			if (input.Count() == 1)
			{
				cVar->Display();
			}
			else if (input.Count() == 2 || input.Count() == 3)
			{
				if (input.Count() == 3)
				{
					cVar->serialize = std::stoi(input.At(2)->GetString());
				}
				C_String* str = input.At(1);
				cVar->Set(str);
			}
			else
			{
				LOG("Sent more than 1 argument for a variable");
			}
		}
		else
		{
			LOG("'%s' is not an intern command", input.At(0)->GetString());
		}
	}
	input.Clear();
}

void M_Console::CutString(const char* src, C_DynArray<C_String>* dst)
{
	C_String str(src);
	C_String tmp;

	bool onComas = false;

	for (uint i = 0; i < str.Length(); i++)
	{
		if (!onComas && str.GetString()[i] == ' ')
		{
			tmp = str;
			tmp.Cut(i);
			dst->PushBack(tmp);
			str.Cut(0, i);
			i = 0;
		}

		if (str.GetString()[i] == '"')
		{
			onComas = !onComas;

			if (onComas)
			{
				str.Cut(0, 0);
				i--;
			}
			else
			{
				str.Cut(i, i);
				i--;
			}
		}
	}
	dst->PushBack(str);

}

void M_Console::Output(char* str)
{
	int y = output.Count() * LINE_SPACING;

	UI_Label* newOutput = App->gui->CreateUI_Label({ 10, textStart + y, 0, 0 }, str);
	newOutput->SetActive(active);
	newOutput->SetParent(consoleRect_D);

	outputHeight = 10 + y;
	newOutput->layer = 1;
	output.PushBack(newOutput);

	int minY = 0;
	int maxY = inputText_D->GetWorldPosition().y - 20;

	int offset = (350 - 15) - (textStart + y + LINE_SPACING);
	if (offset < 0)
	{
		textStart += offset;
		for (uint n = 0; n < output.Count(); n++)
		{
			output[n]->localPosition.y += offset;
			iPoint newPos = {output[n]->localPosition.x, output[n]->localPosition.y};

			if (newPos.y >= maxY || newPos.y < 0)
			{
				output[n]->SetActive(false);
			}

		}
	}
}
// Find a command by  a string
Command* M_Console::FindCommand(const char* str, uint nArgs) const
{
	C_List_item<Command*>* item;
	Command* ret = NULL;
	for (item = commandList.start; item && !ret; item = item->next)
	{
		if (item->data->command == str || item->data->abreviation == str)
		{
			ret = item->data;
		}
	}
	if (ret)
	{
		if (nArgs > ret->nArgs)
		{
			LOG("Command '%s' should recieve %i arguments, sent %i.", str, ret->nArgs, nArgs);
			ret = NULL;
		}
	}

	return ret;

}

CVar* M_Console::FindCVar(const char* str)
{
	CVar* ret = NULL;
	//Looking for the command to execute
	C_List_item<CVar*>* Vars = CVarList.start;
	while (Vars)
	{
		if (Vars->data->GetName() == str)
		{
			ret = Vars->data;
			break;
		}
		Vars = Vars->next;
	}
	return ret;
}

void M_Console::Open()
{
	consoleRect_D->SetActive(true);
	inputRect_D->SetActive(true);
	inputText_D->SetActive(true);

	inputText_D->text.SetText(inputText_D->defaultText);
	App->gui->focus = inputText_D;

	int minY = 0;
	int maxY = inputText_D->GetWorldPosition().y ;

	textStart = output.Count() * (-LINE_SPACING) + maxY;
	if (textStart > 0)
		textStart = 0;

	for (uint n = 0; n < output.Count(); n++)
	{
		output[n]->SetActive(true);
		output[n]->localPosition.y = textStart + LINE_SPACING * n;

		iPoint newPos = { output[n]->localPosition.x, output[n]->localPosition.y };
		if (newPos.y >= maxY || newPos.y < 0)
		{
			output[n]->SetActive(false);
		}
	}

	active = true;
}

void M_Console::Close()
{
	inputText_D->DeleteText();
	consoleRect_D->SetActive(false);
	inputRect_D->SetActive(false);
	inputText_D->SetActive(false);
	active = false;
}

void M_Console::Clear()
{
	for (uint i = 0; i < output.Count(); i++)
	{
		output[i]->SetActive(false);
//		App->gui->DeleteElement(output[i]);
		//We should erase those texts, not just deactivate them
	}
	textStart = 0;
	output.Clear();
}

void M_Console::DisplayCommands(C_String str) const
{
	if (str == "")
	{
		DisplayAllCommands();
	}
	else
	{
		C_List_item<Command*>* item;
		bool found = false;
		for (uint i = 0; i < tags.Count() && !found; i++)
		{
			C_String str2 = tags[i];
			if (tags[i] == str)
			{
				LOG("%s:", str.GetString());
				for (item = commandList.start; item; item = item->next)
				{
					if (item->data->tag == tags[i])
					{
						str.Clear();
						str += "    ";
						str += item->data->command.GetString();

						if (item->data->abreviation != "")
						{
							str += " (";
							str += item->data->abreviation.GetString();
							str += ")";
						}

						str += " -- ";
						str += item->data->desc.GetString();
						LOG("%s", str.GetString());
					}
				}
				found = true;
			}
		}
		if (!found)
		{
			LOG("There is no tag '%s' in the command list", str.GetString());
		}
	}
}
void M_Console::DisplayAllCommands() const
{
	C_List_item<Command*>* item;
	C_String str;
	LOG("   ");
	LOG("Command List:");
	for (uint i = 0; i < tags.Count(); i++)
	{
		LOG("    %s:", tags[i].GetString());
		for (item = commandList.start; item; item = item->next)
		{
			if (item->data->tag == tags[i])
			{
				str.Clear();
				str += "        ";
				str += item->data->command.GetString();

				if (item->data->abreviation != "")
				{
					str += " (";
					str += item->data->abreviation.GetString();
					str += ")";
				}

				str += " -- ";
				str += item->data->desc.GetString();
				LOG("%s", str.GetString());
			}	
		}


		LOG(" ");
		C_List_item<CVar*>* citem;
		for (citem = CVarList.start; citem; citem = citem->next)
		{
			if (item)
			{
				if (item->data->tag == tags[i])
				{
					str.Clear();
					str += "  ";
					if (citem->data->GetListener())
						str += citem->data->GetListener()->name.GetString();
					str += " -- ";
					str += citem->data->GetName().GetString();

					LOG("%s", str.GetString());
				}
			}
		}
	}
}

void M_Console::DisplayTags() const
{
	LOG("Tag List:");
	for (uint i = 0; i < tags.Count(); i++)
	{
		LOG("    %s", tags[i].GetString());
	}
}

bool M_Console::isActive() const
{
	return active;
}


bool M_Console::SaveCVars(pugi::xml_node& Vars) const
{
	pugi::xml_node tmp;
	C_List_item<CVar*>* item = CVarList.start;

	while (item)
	{
		switch (item->data->GetType())
		{
		case c_float:
		{
			float toAppend = 0.0f;
			item->data->Read(&toAppend);
			Vars.append_child(item->data->GetName().GetString()).append_attribute("value") = toAppend;
			break;
		}
		case c_int:
		{
			int toAppend = 0;
			item->data->Read(&toAppend);
			Vars.append_child(item->data->GetName().GetString()).append_attribute("value") = toAppend;
			break;
		}
		case c_string:
		{
			char* toAppend = "";
			item->data->Read(toAppend);
			Vars.append_child(item->data->GetName().GetString()).append_attribute("value") = toAppend;
			break;
		}
		case c_bool:
		{
			bool toAppend = true;
			item->data->Read(&toAppend);
			Vars.append_child(item->data->GetName().GetString()).append_attribute("value") = toAppend;
			break;
		}
		}
		item = item->next;
	}

	return true;
}

bool M_Console::LoadCVars(pugi::xml_node& conf)
{
	pugi::xml_node vars = conf.first_child();
	while (vars)
	{
		char* name = (char*)vars.name();
		C_String toSend(name);
		CVar* cVarToSet = FindCVar(toSend.GetString());
		if (cVarToSet)
		{
			char* name2 = (char*)vars.attribute("value").as_string();
			C_String toSend2(name2);
			cVarToSet->Set(&toSend2);
		}

		vars = vars.next_sibling();
	}
	return true;
}

#pragma region Commands
void M_Console::C_commandList::function(const C_DynArray<C_String>* arg)
{
	C_String str("");
	if (arg->Count() > 1)
	{
		str = arg->At(1)->GetString();
	}
	App->console->DisplayCommands(str);
}

void M_Console::C_tagList::function(const C_DynArray<C_String>* arg)
{
	App->console->DisplayTags();
}

void M_Console::C_closeConsole::function(const C_DynArray<C_String>* arg)
{
    App->console->Close();
}

void M_Console:: C_clearConsole::function(const C_DynArray<C_String>* arg)
{
	App->console->Clear();
}

void M_Console::C_Quit::function(const C_DynArray<C_String>* arg)
{
	App->console->closeGame = true;
}
#pragma endregion

