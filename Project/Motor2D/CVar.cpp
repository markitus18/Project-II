#include "M_Console.h"
#include "Defs.h"
#include "Log.h"

CVar::CVar(const char* newName, float* newReference, bool newSerialize) : name(newName), type(c_float), serialize(newSerialize)
{
	reference.floatRef = newReference;
	value.floatVar = *newReference;
}
CVar::CVar(const char* newName, int* newReference, bool newSerialize) : name(newName), type(c_int), serialize(newSerialize)
{
	reference.intRef = newReference;
	value.intVar = *newReference;
}
CVar::CVar(const char* newName, char* newReference, bool newSerialize) : name(newName), type(c_string), serialize(newSerialize)
{
	reference.stringRef = newReference;
	value.stringVar = newReference;
}
CVar::CVar(const char* newName, bool* newReference, bool newSerialize) : name(newName), type(c_bool), serialize(newSerialize)
{
	reference.boolRef = newReference;
	value.boolVar = *newReference;
}


void CVar::LinkCommand(Command* toLink)
{
	command = toLink;
}


void CVar::Set(float newValue)
{
	if (type == c_float)
	{
		*reference.floatRef = newValue;

		LOG("Set CVar %s to %f.", name.GetString(), newValue);
		if (serialize)
		{
			value.floatVar = newValue;
			LOG("Will be serialized");
		}
		if (command)
		{
			command->function(NULL);
		}
	}
}
void CVar::Set(int newValue)
{
	if (type == c_int)
	{
		*reference.intRef = newValue;

		LOG("Set CVar %s to %i.", name.GetString(), newValue);
		if (serialize)
		{
			LOG("Will be serialized");
			value.intVar = newValue;
		}
		if (command)
		{
			command->function(NULL);
		}
	}
}
void CVar::Set(char* newValue)
{
	if (type == c_string)
	{
		reference.stringRef = newValue;

		LOG("Set CVar %s to %s.", name.GetString(), newValue);
		if (serialize)
		{
			LOG("Will be serialized");
			value.stringVar = newValue;
		}
		if (command)
		{
			command->function(NULL);
		}
	}
}
void CVar::Set(bool newValue)
{
	if (type == c_bool)
	{

		*reference.boolRef = newValue;

		LOG("Set CVar %s to %b.", name.GetString(), newValue);
		if (serialize)
		{
			LOG("Will be serialized");
			value.boolVar = newValue;
		}
		if (command)
		{
			command->function(NULL);
		}
	}
}
void CVar::Set(C_String* data)
{
	switch (type)
	{
	case c_float:
	{
		Set(std::stof(data->GetString()));
		break;
	}
	case c_int:
	{
		Set(std::stoi(data->GetString()));
		break;
	}
	case c_string:
	{
		Set(data->GetString());
		break;
	}
	case c_bool:
	{
		bool toSend = std::stoi(data->GetString());
		Set(toSend);
		break;
	}
	}
}

void CVar::SetListener(j1Module* module)
{
	listener = module;
}

CVarTypes CVar::GetType() { return type; }
C_String CVar::GetName() { return name; }

const j1Module* CVar::GetListener() const
{
	return listener;
}

void CVar::Read(void* ret, CVarTypes expectedData)
{
	if (expectedData == type)
	{
		ret = &value;
	}
	else
	{
		//LOG("Incorrect data type request to %s", name->GetString());
	}
}
bool CVar::Read(float* output) { if (type == c_float){ *output = value.floatVar; return true; } return false; }
bool CVar::Read(int* output) { if (type == c_int){ *output = value.intVar; return true; } return false; }
bool CVar::Read(char* output) { if (type == c_string){ output = value.stringVar; return true; } return false; }
bool CVar::Read(bool* output) { if (type == c_bool){ *output = value.boolVar; return true; } return false; }
void* CVar::ForceRead() { return &value; }

void CVar::Display()
{
	switch (type)
	{
	case c_float:
		LOG("Float value: %f", value.floatVar);
		break;
	case c_int:
		LOG("Int value: %i", value.intVar);
		break;
	case c_string:
		LOG("String value: %s", value.stringVar);
		break;
	case c_bool:
		LOG("Bool value: %b", value.boolVar);
		break;
	default:
		break;
	}
}