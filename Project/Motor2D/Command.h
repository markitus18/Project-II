#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "C_DynArray.h"
#include "C_String.h"

enum CVarTypes
{
	c_float = 0,
	c_int,
	c_string,
	c_bool,
};

class j1Module;

struct Command
{
public:
	Command(){}
	Command(char* str, char* dsc, uint n, char* abr = NULL, char* newTag = "Miscellaneous"){ command = str; desc = dsc, nArgs = n; abreviation = abr; tag = newTag; }

public:
	C_String desc;
	C_String command;
	C_String abreviation;
	C_String tag;

	uint nArgs;
	virtual void function(const C_DynArray<C_String>* arg);
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
	C_String desc;
	C_String name;
	C_String tag;

private:
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
	void Set(C_String* data);

	void SetListener(j1Module* module);

	CVarTypes GetType();
	C_String GetName();
	const j1Module* GetListener() const;

	void Read(void* ret, CVarTypes expectedData);
	bool Read(float* output);
	bool Read(int* output);
	bool Read(char* output);
	bool Read(bool* output);
	void* ForceRead();

	void Display();
};

#endif