#ifndef __C_String_H__
#define __C_String_H__

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "Defs.h"

#define TMP_STRING_SIZE	4096

#include <string>

class C_String
{
private:

	std::string str;

public:

	// Constructors
	C_String() : str()
	{
		str.clear();
	}

	C_String(const C_String& string) : str(string.GetString())
	{
	}

	C_String(const char *format, ...)
	{
		if(format != NULL)
		{
			static char tmp[TMP_STRING_SIZE];
			static va_list  ap;

			// Construct the string from variable arguments
			va_start(ap, format);
			int res = vsprintf_s(tmp, TMP_STRING_SIZE, format, ap);
			va_end(ap);

			if(res > 0)
			{
				str = tmp;
			}
		}
	}
	
	// Destructor
	virtual ~C_String()
	{
	}

	const C_String& create(const char *format, ...)
	{
		if(format != NULL)
		{
			static char tmp[TMP_STRING_SIZE];
			static va_list  ap;

			// Construct the string from variable arguments
			va_start(ap, format);
			int res = vsprintf_s(tmp, TMP_STRING_SIZE, format, ap);
			va_end(ap);

			if(res > 0)
			{
				str = tmp;
			}
		}

		return *this;
	}

	// Operators
	bool operator== (const C_String& string) const
	{
		if (string.str == str)
		{
			return true;
		}
		return false;
	}

	bool operator== (const char* string) const
	{
		if (string != NULL)
		{
			if (str == string)
			{
				return true;
			}
		}
		return false;
	}

	bool operator!= (const C_String& string) const
	{
		if (string.str == str)
		{
			return false;
		}
		return true;
	}

	bool operator!= (const char* string) const
	{
		if (string != NULL)
		{
			if (str == string)
			{
				return false;
			}
		}
		return true;
	}
	
	const C_String& operator= (const C_String& string)
	{
		str = string.str;
		return(*this);
	}

	const C_String& operator= (const char* string)
	{
		if (string != NULL)
		{
			str = string;
		}
		else
		{
			str.clear();
		}

		return *this;
	}
	
	const C_String& operator+= (const C_String& string)
	{
		str += string.str;
		return(*this);
	}

	const C_String& operator+= (const char* string)
	{
		if (string != NULL)
		{
			str += string;
		}

		return(*this);
	}

	// Utils
	unsigned int Length() const
	{
		return str.length();
	}

	void Clear()
	{
		str.clear();
	}

	const char* GetString() const
	{
		return str.data();
	}

	unsigned int GetCapacity() const
	{
		return str.capacity();
	}

	bool Cut(unsigned int begin, unsigned int end = 0)
	{
		uint len = Length();

		if (end >= len || end == 0)
		{
			end = len - 1;
		}

		if (begin > len || end <= begin)
		{
			return false;
		}

		/*char* p1 = str.data() + begin;
		char* p2 = str.data() + end + 1;

		while(*p1++ = *p2++);*/

		if (end != 0)
		{
			str.erase(begin, (end - begin + 1));
		}
		else
		{
			str.erase(begin);
		}

		return true;
	}

	uint Find(const char* string) const
	{
		uint ret = str.find(string);
		return ret;
	}

	

};

#endif // __C_String_H__
