#ifndef __j1CONSOLE_H__
#define __j1CONSOLE_H__

#include "j1Module.h"

#define LINE_SPACING 13
#define MAX_OUTPUT_LINES 49


class UI_InputText;
class UI_Rect;
class UI_Label;

class M_Console : public j1Module
{
public:

	M_Console(bool);

	// Destructor
	virtual ~M_Console();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called every frame
	bool Update(float dt);

	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	void OnGUI(GUI_EVENTS, UI_Element*);

	void AddCommand(Command*);
	uint AddCVar(const char* _name, float* reference, j1Module* listener = NULL, bool serialize = false);
	uint AddCVar(const char* _name, int* reference, j1Module* listener = NULL, bool serialize = false);
	uint AddCVar(const char* _name, char* reference, j1Module* listener = NULL, bool serialize = false);
	uint AddCVar(const char* _name, bool* reference, j1Module* listener = NULL, bool serialize = false);

	void GetNewInput(const char* src);


	void Output(char* str);

	void Open();
	void Close();
	void Clear();

	void DisplayCommands(C_String str) const;
	void DisplayAllCommands() const;
	void DisplayTags() const;

	bool isActive() const;

	bool SaveCVars(pugi::xml_node& data) const;
	bool LoadCVars(pugi::xml_node&);

private:
	void CutString(const char* src, C_DynArray<C_String>* dst);
	Command* FindCommand(const char* str, uint nArgs) const;
	CVar* FindCVar(const char* str);
	void SetCVar(const char* value);

private:
	std::list<Command*> commandList;
	std::list<CVar*> CVarList;
	C_DynArray<C_String> tags;

	std::vector<UI_Label*> output;
	std::vector<UI_Label*>::iterator nextOutput;

	UI_InputText* inputText_D;
	UI_Rect* consoleRect_D;

	UI_Rect* inputRect_D;

	bool active = false;
	bool dragText = false;

	int textStart = 0;
	int outputHeight = 0;

	bool closeGame = false;

	
#pragma region Commands
	struct C_commandList : public Command
	{
		C_commandList() : Command("list", "Display command list", 1, NULL, "Console"){}
		void function(const C_DynArray<C_String>* arg);	
	};
	C_commandList c_commandList;

	struct C_tagList : public Command
	{
		C_tagList() : Command("tags", "Display tag list", 0, NULL, "Console"){}
		void function(const C_DynArray<C_String>* arg);
	};
	C_tagList c_tagList;

	struct C_closeConsole : public Command
	{
		C_closeConsole() : Command("close", "Close console", 0, NULL, "Console"){}
		void function(const C_DynArray<C_String>* arg);
	};
	C_closeConsole c_closeConsole;

	struct C_clearConsole : public Command
	{
		C_clearConsole() : Command("cls", "Clear console output", 0, NULL, "Console"){}
		void function(const C_DynArray<C_String>* arg);
	};
	C_clearConsole c_clearConsole;

	struct C_Quit : public Command
	{
		C_Quit() : Command("quit", "Quit the application", 0, NULL, "Console"){}
		void function(const C_DynArray<C_String>* arg);
	};
	C_Quit c_Quit;
#pragma endregion

};

#endif // __j1CONSOLE_H__