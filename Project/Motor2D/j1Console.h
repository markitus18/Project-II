#ifndef __j1CONSOLE_H__
#define __j1CONSOLE_H__

#include "j1Module.h"

#define LINE_SPACING 16

enum CVarTypes
{
	c_float = 0,
	c_int,
	c_string,
	c_bool,
};

struct Command
{
public:
	Command(){}
	Command(char* str, char* dsc, uint n, char* abr = NULL, char* newTag = "Miscellaneous"){ command = str; desc = dsc, nArgs = n; abreviation = abr; tag = newTag; }

public:
	p2SString desc;
	p2SString command;
	p2SString abreviation;
	p2SString tag;

	uint nArgs;
	virtual void function(const p2DynArray<p2SString>* arg);
};

class CVar
{
public:
	CVar(const char* newName, float* newRference, bool newSerialize = false);
	CVar(const char* newName, int* newReference, bool newSerialize = false);
	CVar(const char* newName, char* newReference, bool newSerialize = false);
	CVar(const char* newName, bool* newReference, bool newSerialize = false);

public:
	bool serialize;

private:
	p2SString desc;
	p2SString name;
	p2SString tag;
	CVarTypes type;
	Command* command = NULL;
	j1Module* listener = NULL;

	union {
		float* floatRef;
		int* intRef;
		char* stringRef;
		bool* boolRef;
	} reference;

	union {
		float floatVar;
		int intVar;
		char* stringVar;
		bool boolVar;
	} value;

public:
	void LinkCommand(Command* toLink);

	void Set(float newValue);
	void Set(int newValue);
	void Set(char* newValue);
	void Set(bool newValue);
	void Set(p2SString* data);

	void SetListener(j1Module* module);

	CVarTypes GetType();
	p2SString GetName();
	const j1Module* GetListener() const;

	void Read(void* ret, CVarTypes expectedData);
	bool Read(float* output);
	bool Read(int* output);
	bool Read(char* output);
	bool Read(bool* output);
	void* ForceRead();

	void Display();
};


class UIInputText;
class UIRect;
class UILabel;
class UIScrollBar;

class j1Console : public j1Module
{
public:

	j1Console(bool);

	// Destructor
	virtual ~j1Console();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called every frame
	bool Update(float dt);

	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	void OnGUI(UI_Event _event, UIElement* _element);

	void AddCommand(Command*);
	uint AddCVar(const char* _name, float* reference, j1Module* listener = NULL, bool serialize = false);
	uint AddCVar(const char* _name, int* reference, j1Module* listener = NULL, bool serialize = false);
	uint AddCVar(const char* _name, char* reference, j1Module* listener = NULL, bool serialize = false);
	uint AddCVar(const char* _name, bool* reference, j1Module* listener = NULL, bool serialize = false);

	void GetNewInput(char* src);


	void Output(char* str);

	void Open();
	void Close();
	void Clear();

	void DisplayCommands(p2SString str) const;
	void DisplayAllCommands() const;
	void DisplayTags() const;

	bool isActive() const;

	bool SaveCVars(pugi::xml_node& data) const;
	bool LoadCVars(pugi::xml_node&);

private:
	void CutString(char* src, p2DynArray<p2SString>* dst);
	Command* FindCommand(const char* str, uint nArgs) const;
	CVar* FindCVar(const char* str);
	void SetCVar(const char* value);

private:
	p2List<Command*> commandList;
	p2List<CVar*> CVarList;
	p2DynArray<p2SString> tags;

	UIInputText* inputText;
	UIRect* consoleRect;

	UIRect* inputRect;
	UILabel* console_defLabel;

	UIScrollBar* scrollbar;
	UIRect* scrollbar_rect;
	UIRect* scrollbar_thumb;

	p2DynArray<UILabel*> output;

	bool active = false;
	bool dragText = false;

	int textStart = 0;
	int outputHeight = 0;

	bool closeGame = false;

	
#pragma region Commands
	struct C_commandList : public Command
	{
		C_commandList() : Command("list", "Display command list", 1, NULL, "Console"){}
		void function(const p2DynArray<p2SString>* arg);	
	};
	C_commandList c_commandList;

	struct C_tagList : public Command
	{
		C_tagList() : Command("tags", "Display tag list", 0, NULL, "Console"){}
		void function(const p2DynArray<p2SString>* arg);
	};
	C_tagList c_tagList;

	struct C_closeConsole : public Command
	{
		C_closeConsole() : Command("close", "Close console", 0, NULL, "Console"){}
		void function(const p2DynArray<p2SString>* arg);
	};
	C_closeConsole c_closeConsole;

	struct C_clearConsole : public Command
	{
		C_clearConsole() : Command("cls", "Clear console output", 0, NULL, "Console"){}
		void function(const p2DynArray<p2SString>* arg);
	};
	C_clearConsole c_clearConsole;

	struct C_Quit : public Command
	{
		C_Quit() : Command("quit", "Quit the application", 0, NULL, "Console"){}
		void function(const p2DynArray<p2SString>* arg);
	};
	C_Quit c_Quit;
#pragma endregion

};

#endif // __j1CONSOLE_H__