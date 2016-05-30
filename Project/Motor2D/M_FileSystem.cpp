#include "M_FileSystem.h"

#include "j1App.h"

#include "PhysFS/include/physfs.h"
#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

M_FileSystem::M_FileSystem(bool start_enabled) : j1Module(start_enabled)
{
	name.create("file_system");

	// need to be created before Awake so other modules can use it
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);
	SDL_free(base_path);

	// By default we include executable's own directory
	// without this we won't be able to find config.xml :-(
//	AddPath(".");

	AddPath("./../Game");
}

// Destructor
M_FileSystem::~M_FileSystem()
{
	PHYSFS_deinit();
}

// Called before render is available
bool M_FileSystem::Awake(pugi::xml_node& config)
{
	LOG("Loading File System");
	bool ret = true;

	// Add all paths in configuration in order
	for(pugi::xml_node path = config.child("path"); path; path = path.next_sibling("path"))
	{
		AddPath(path.child_value());
	}

	// Ask SDL for a write dir
	char* write_path = SDL_GetPrefPath(App->GetOrganization(), App->GetTitle());

	if(PHYSFS_setWriteDir(write_path) == 0)
		LOG("File System error while creating write dir: %s", PHYSFS_getLastError());
	else
	{
		// We add the writing directory as a reading directory too with speacial mount point
		LOG("Writing directory is %s", write_path);
		AddPath(write_path, GetSaveDirectory());
	}

	SDL_free(write_path);
	PHYSFS_addToSearchPath(GetSaveDirectory(), 0);

	return ret;
}

// Called before quitting
bool M_FileSystem::CleanUp()
{
	//LOG("Freeing File System subsystem");
	return true;
}

// Add a new zip file or folder
bool M_FileSystem::AddPath(const char* path_or_zip, const char* mount_point)
{
	bool ret = false;

	if(PHYSFS_mount(path_or_zip, mount_point, 1) == 0)
		LOG("File System error while adding a path or zip(%s): %s", path_or_zip, PHYSFS_getLastError());
	else
		ret = true;

	return ret;
}

// Check if a file exists
bool M_FileSystem::Exists(const char* file) const
{
	return PHYSFS_exists(file) != 0;
}

// Check if a file is a directory
bool M_FileSystem::IsDirectory(const char* file) const
{
	return PHYSFS_isDirectory(file) != 0;
}

std::vector<std::string> M_FileSystem::GetSaveFiles()
{
	std::vector<std::string> ret;
	char **rc = PHYSFS_enumerateFiles("save");
	char **i;
	

	for (i = rc; *i != NULL; i++)
	{
			C_String tmp = *i;
			tmp.Cut(tmp.Length() - 4);
			if (tmp != "CVars")
			{
				ret.push_back(tmp.GetString());
				LOG(" * We've got [%s].", *i);
			}
	}
	PHYSFS_freeList(rc);

	if (ret.size() > 1)
	{
		//Ordering save files
		bool ordered = false;
		while (ordered == false)
		{
			int n = 0;
			while (n < ret.size() - 1)
			{
				ordered = true;
				C_String tmp = ret[n].c_str();
				int day1 = atoi(tmp.GetString());
				tmp.Cut(0, 2);
				int month1 = atoi(tmp.GetString());
				tmp.Cut(0, 3);
				int hour1 = atoi(tmp.GetString());
				tmp.Cut(0, 2);
				int minute1 = atoi(tmp.GetString());

				C_String tmp2 = ret[n + 1].c_str();
				int day2 = atoi(tmp2.GetString());
				tmp2.Cut(0, 2);
				int month2 = atoi(tmp2.GetString());
				tmp2.Cut(0, 3);
				int hour2 = atoi(tmp2.GetString());
				tmp2.Cut(0, 2);
				int minute2 = atoi(tmp2.GetString());

				if (month1 == month2)
				{
					if (day1 == day2)
					{
						if (hour1 == hour2)
						{
							if (minute1 > minute2)
							{
								C_String p = ret[n].c_str();
								ret[n] = ret[n + 1];
								ret[n + 1] = p.GetString();

								ordered = false;
							}
							else
							{
								n++;
							}
						}
						else if (hour1 > hour2)
						{
							C_String p = ret[n].c_str();
							ret[n] = ret[n + 1];
							ret[n + 1] = p.GetString();

							ordered = false;
						}
						else
						{
							n++;
						}
					}
					else if (day1 > day2)
					{
						C_String p = ret[n].c_str();
						ret[n] = ret[n + 1];
						ret[n + 1] = p.GetString();

						ordered = false;
					}
					else
					{
						n++;
					}
				}
				else if (month1 > month2)
				{
					C_String p = ret[n].c_str();
					ret[n] = ret[n + 1];
					ret[n + 1] = p.GetString();

					ordered = false;
				}
				else
				{
					n++;
				}
			}
		}
	}
	return ret;
}

bool M_FileSystem::EraseFile(const char* file)
{
	if (true || Exists(file))
	{
		if (PHYSFS_delete(file) != 0)
		{
			return true;
		}
		LOG("Tried to erase %s, error: %s", file, PHYSFS_getLastError());
		return false;
	}
	LOG("Tried to erase %s, which could not be found", file);
	return false;
}

// Read a whole file and put it in a new buffer
unsigned int M_FileSystem::Load(const char* file, char** buffer) const
{
	unsigned int ret = 0;

	PHYSFS_file* fs_file = PHYSFS_openRead(file);

	if(fs_file != NULL)
	{
		PHYSFS_sint64 size = PHYSFS_fileLength(fs_file);

		if(size > 0)
		{
			*buffer = new char[(uint)size];
			PHYSFS_sint64 readed = PHYSFS_read(fs_file, *buffer, 1, (PHYSFS_sint32)size);
			if(readed != size)
			{
				LOG("File System error while reading from file %s: %s", file, PHYSFS_getLastError());
				RELEASE(buffer);
			}
			else
				ret = (uint)readed;
		}

		if(PHYSFS_close(fs_file) == 0)
			LOG("File System error while closing file %s: %s", file, PHYSFS_getLastError());
	}
	else
		LOG("File System error while opening file %s: %s", file, PHYSFS_getLastError());

	return ret;
}

// Read a whole file and put it in a new buffer
SDL_RWops* M_FileSystem::Load(const char* file) const
{
	char* buffer;
	int size = Load(file, &buffer);

	if(size > 0)
	{
		SDL_RWops* r = SDL_RWFromConstMem(buffer, size);
		if(r != NULL)
			r->close = close_sdl_rwops;

		return r;
	}
	else
		return NULL;
}

int close_sdl_rwops(SDL_RWops *rw)
{
	RELEASE_ARRAY(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;
}

// Save a whole buffer to disk
unsigned int M_FileSystem::Save(const char* file, const char* buffer, unsigned int size) const
{
	unsigned int ret = 0;

	PHYSFS_file* fs_file = PHYSFS_openWrite(file);

	if(fs_file != NULL)
	{
		PHYSFS_sint64 written = PHYSFS_write(fs_file, (const void*)buffer, 1, size);
		if(written != size)
			LOG("File System error while writing to file %s: %s", file, PHYSFS_getLastError());
		else
			ret = (uint) written;

		if(PHYSFS_close(fs_file) == 0)
			LOG("File System error while closing file %s: %s", file, PHYSFS_getLastError());
	}
	else
		LOG("File System error while opening file %s: %s", file, PHYSFS_getLastError());

	return ret;
}