#include "p2Defs.h"
#include "p2Log.h"

#include "j1FileSystem.h"
#include "j1Console.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1App.h"
#include "j1Gui.h"
#include "j1Fonts.h"
#include "UIElements.h"

#include "SDL/include/SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

void Command::function(const p2DynArray<p2SString>* arg){ LOG("Executing command function"); }

j1Console::j1Console(bool start_enabled) : j1Module(start_enabled)
{
	name.create("console");
}

// Destructor
j1Console::~j1Console()
{}

// Called before render is available
bool j1Console::Awake(pugi::xml_node& config)
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
bool j1Console::Start()
{
	TTF_Font* inputFont = App->font->Load("fonts/open_sans/OpenSans-Regular.ttf", 16);


	//Console rect
	consoleRect = App->gui->CreateRect("Console rect", { 0, 0, App->render->camera.w, 350 }, 0, 0, 0, 200);
	consoleRect->SetLayer(GUI_MAX_LAYERS);
	consoleRect->interactive = true;
	consoleRect->listener = this;

	//Console input rect
	inputRect = App->gui->CreateRect("Console Input rect", { 0, 350, App->render->camera.w, 40 }, 130, 130, 130);
	inputRect->SetLayer(GUI_MAX_LAYERS);

	//Console input label
	UILabel* console_defLabel;
	console_defLabel = App->gui->CreateText("Console default label", { 10, 8 }, "Command", NULL, true, NULL, inputFont, SDL_Color{ 0, 0, 0 });
	console_defLabel->SetLayer(GUI_MAX_LAYERS);

	//Console input
	inputText = App->gui->CreateInputText("Console Input", { 0, 350, App->render->camera.w, 40 }, NULL, console_defLabel, App->gui->GetScreen(), 10, 10, true, this);
	inputText->Center_x(App->gui->GetScreen());
	inputText->maxCharacters = 67;
	inputText->SetLayer(GUI_MAX_LAYERS);

	//Console scroll bar
	scrollbar_rect = App->gui->CreateRect("Console Scroll Bar Rect", { 0, 0, 15, 350 }, 200, 200, 200);
	scrollbar_rect->SetLayer(GUI_MAX_LAYERS);

	scrollbar_thumb = App->gui->CreateRect("Console Scroll Bar Thumb", { App->render->camera.w - 13, 155, 11, 20 }, 0, 0, 0);
	scrollbar_thumb->SetLayer(GUI_MAX_LAYERS);

	scrollbar = App->gui->CreateScrollBar("Console Scroll Bar", { App->render->camera.w - 15, 0 }, scrollbar_rect, scrollbar_thumb, App->gui->GetScreen(), VERTICAL, 2, 0, 20, 20, true, this);
	scrollbar->SetLayer(GUI_MAX_LAYERS);

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

bool j1Console::PostUpdate(float dt)
{
	if (closeGame)
		return false;
	return true;
}
bool j1Console::Update(float dt)
{
	if (dragText)
	{
		int x, y;
		App->input->GetMouseMotion(x, y);
		if ((y > 0 && textStart + y <=0) || (y < 0 && textStart + outputHeight + y > 350 - 15))
		{
			int minY = 0;
			int maxY = inputText->GetWorldRect().y - 20;

			textStart += y;
			for (uint n = 0; n < output.Count(); n++)
			{
				output[n]->active = true;
				iPoint pos = output[n]->GetLocalPosition();
				output[n]->SetLocalPosition(pos.x, pos.y + y);
				iPoint newPos = output[n]->GetLocalPosition();

				if (newPos.y >= maxY || newPos.y < 0)
				{
					output[n]->active = false;
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
bool j1Console::CleanUp()
{
	return true;
}

void j1Console::OnGUI(UI_Event _event, UIElement* _element)
{
	if (_element == inputText)
	{
		if (_event == RETURN_DOWN)
		{
			UIInputText* input = (UIInputText*)_element;
			GetNewInput(input->GetString());
			input->DeleteText();
		}
	}
	if (_element == consoleRect)
	{
		if (_event == MOUSE_DOWN)
		{
			dragText = true;
		}
		if (_event == MOUSE_UP || _event == MOUSE_EXIT)
		{
			dragText = false;
		}
	}
	if (_element == scrollbar)
	{
		if (_event == SCROLL_CHANGE)
		{
			float v = scrollbar->GetValue();
			int maxY = inputText->GetWorldRect().y - 20;
			int minY = output.Count() * (-LINE_SPACING) + maxY;

			textStart = v * minY;

			for (uint n = 0; n < output.Count(); n++)
			{
				output[n]->active = true;
				iPoint pos = output[n]->GetLocalPosition();
				output[n]->SetLocalPosition(pos.x, textStart + n * LINE_SPACING);
				iPoint newPos = output[n]->GetLocalPosition();

				if (newPos.y >= maxY || newPos.y < 0)
				{
					output[n]->active = false;
				}

			}
			
		}
	}
}
void j1Console::AddCommand(Command* command)
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

uint j1Console::AddCVar(const char* newName, float* reference, j1Module* listener, bool serialize)
{
	CVar* newCVar = new CVar(newName, reference, serialize);
	newCVar->SetListener(listener);
	CVarList.add(newCVar);
	return CVarList.count() - 1;
}
uint j1Console::AddCVar(const char* newName, int* reference, j1Module* listener, bool serialize)
{
	CVar* newCVar = new CVar(newName, reference, serialize);
	newCVar->SetListener(listener);
	CVarList.add(newCVar);
	return CVarList.count() - 1;
}
uint j1Console::AddCVar(const char* newName, char* reference, j1Module* listener, bool serialize)
{
	CVar* newCVar = new CVar(newName, reference, serialize);
	newCVar->SetListener(listener);
	CVarList.add(newCVar);
	return CVarList.count() - 1;
}
uint j1Console::AddCVar(const char* newName, bool* reference, j1Module* listener, bool serialize)
{
	CVar* newCVar = new CVar(newName, reference, serialize);
	newCVar->SetListener(listener);
	CVarList.add(newCVar);
	return CVarList.count() - 1;
}

void j1Console::GetNewInput(char* src)
{
	p2DynArray<p2SString> input;
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
				p2SString* str = input.At(1);
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

void j1Console::CutString(char* src, p2DynArray<p2SString>* dst)
{
	p2SString str(src);
	p2SString tmp;

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

void j1Console::Output(char* str)
{
	int y = output.Count() * LINE_SPACING;

	UILabel* newOutput = App->gui->CreateText("outputLine", { 10, textStart + y }, str, consoleRect, active, NULL, NULL);
	outputHeight = 10 + y;
	newOutput->layer = 1;
	output.PushBack(newOutput);

	int minY = 0;
	int maxY = inputText->GetWorldRect().y - 20;

	int offset = (350 - 15) - (textStart + y + LINE_SPACING);
	if (offset < 0)
	{
		textStart += offset;
		for (uint n = 0; n < output.Count(); n++)
		{
			output[n]->active = active ? true : false;

			iPoint pos = output[n]->GetLocalPosition();
			output[n]->SetLocalPosition(pos.x, pos.y + offset);
			iPoint newPos = output[n]->GetLocalPosition();

			if (newPos.y >= maxY || newPos.y < 0)
			{
				output[n]->active = false;
			}

		}
	}
	scrollbar->SetValue(1.0f);
}
// Find a command by  a string
Command* j1Console::FindCommand(const char* str, uint nArgs) const
{
	p2List_item<Command*>* item;
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

CVar* j1Console::FindCVar(const char* str)
{
	CVar* ret = NULL;
	//Looking for the command to execute
	p2List_item<CVar*>* Vars = CVarList.start;
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

void j1Console::Open()
{
	consoleRect->Activate();
	inputRect->Activate(); 
	inputText->Activate();
	scrollbar->Activate();
	App->gui->SetFocus(inputText);

	int minY = 0;
	int maxY = inputText->GetWorldRect().y ;

	textStart = output.Count() * (-LINE_SPACING) + maxY;
	if (textStart > 0)
		textStart = 0;

	for (uint n = 0; n < output.Count(); n++)
	{
		output[n]->active = true;
		iPoint pos = output[n]->GetLocalPosition();
		output[n]->SetLocalPosition(pos.x, textStart + LINE_SPACING * n);

		iPoint newPos = output[n]->GetLocalPosition();
		if (newPos.y >= maxY || newPos.y < 0)
		{
			output[n]->active = false;
		}
	}

	active = true;
}

void j1Console::Close()
{
	inputText->DeleteText();
	consoleRect->Deactivate();
	inputRect->Deactivate();
	inputText->Deactivate();
	scrollbar->Deactivate();

	active = false;
}

void j1Console::Clear()
{
	for (uint i = 0; i < output.Count(); i++)
	{
		output[i]->active = false;
//		App->gui->DeleteElement(output[i]);
		//We should erase those texts, not just deactivate them
	}
	textStart = 0;
	output.Clear();
}

void j1Console::DisplayCommands(p2SString str) const
{
	if (str == "")
	{
		DisplayAllCommands();
	}
	else
	{
		p2List_item<Command*>* item;
		bool found = false;
		for (uint i = 0; i < tags.Count() && !found; i++)
		{
			p2SString str2 = tags[i];
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
void j1Console::DisplayAllCommands() const
{
	p2List_item<Command*>* item;
	p2SString str;
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
		p2List_item<CVar*>* citem;
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

void j1Console::DisplayTags() const
{
	LOG("Tag List:");
	for (uint i = 0; i < tags.Count(); i++)
	{
		LOG("    %s", tags[i].GetString());
	}
}

bool j1Console::isActive() const
{
	return active;
}


bool j1Console::SaveCVars(pugi::xml_node& Vars) const
{
	pugi::xml_node tmp;
	p2List_item<CVar*>* item = CVarList.start;

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

bool j1Console::LoadCVars(pugi::xml_node& conf)
{
	pugi::xml_node vars = conf.first_child();
	while (vars)
	{
		char* name = (char*)vars.name();
		p2SString toSend(name);
		CVar* cVarToSet = FindCVar(toSend.GetString());
		if (cVarToSet)
		{
			char* name2 = (char*)vars.attribute("value").as_string();
			p2SString toSend2(name2);
			cVarToSet->Set(&toSend2);
		}

		vars = vars.next_sibling();
	}
	return true;
}

#pragma region Commands
void j1Console::C_commandList::function(const p2DynArray<p2SString>* arg)
{
	p2SString str("");
	if (arg->Count() > 1)
	{
		str = arg->At(1)->GetString();
	}
	App->console->DisplayCommands(str);
}

void j1Console::C_tagList::function(const p2DynArray<p2SString>* arg)
{
	App->console->DisplayTags();
}

void j1Console::C_closeConsole::function(const p2DynArray<p2SString>* arg)
{
    App->console->Close();
}

void j1Console:: C_clearConsole::function(const p2DynArray<p2SString>* arg)
{
	App->console->Clear();
}

void j1Console::C_Quit::function(const p2DynArray<p2SString>* arg)
{
	App->console->closeGame = true;
}
#pragma endregion

