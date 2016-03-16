// ----------------------------------------------------
// j1Module.h
// Interface for all engine modules
// ----------------------------------------------------

#ifndef __j1MODULE_H__
#define __j1MODULE_H__

#include "C_String.h"
#include "C_DynArray.h"
#include "C_List.h"
#include "C_Point.h"
#include "Log.h"

#include <list>
#include <map>

#include "SDL/include/SDL.h"
#include "PugiXml\src\pugixml.hpp"

class UI_Element;
class j1App;

class j1Module
{
public:

	j1Module(bool start_enabled = true) : enabled(start_enabled)
	{}

	virtual ~j1Module()
	{}

	bool IsEnabled() const
	{
		return enabled;
	}

	bool Enable()
	{
		bool ret = true;
		if (enabled == false)
		{
			enabled = true;
			ret = Start();
		}
		return ret;
	}

	bool Disable()
	{
		bool ret = true;
		if (enabled)
		{
			enabled = false;
			ret = CleanUp();
		}
		return ret;
	}
	void Init()
	{
		enabled = true;
	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&)
	{
		return true;
	}

	//We will load all textures in PreStart
	virtual bool PreStart(pugi::xml_node&)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate(float dt)
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	virtual void ManageInput()
	{

	}

	virtual void OnGUI(GUI_EVENTS, UI_Element*)
	{

	}

	virtual bool Load(pugi::xml_node&)
	{
		return true;
	}

	virtual bool Save(pugi::xml_node&) const
	{
		return true;
	}

public:

	C_String	name;
	bool		enabled;

};

#endif // __j1MODULE_H__