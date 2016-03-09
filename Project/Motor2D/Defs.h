#ifndef __P2DEFS_H__
#define __P2DEFS_H__

#include <stdio.h>

//  NULL just in case ----------------------

#ifdef NULL
#undef NULL
#endif
#define NULL  0

// Deletes a buffer
#define RELEASE( x ) \
    {                \
    if( x != NULL )  \
	    {            \
      delete x;      \
	  x = NULL;      \
	    }            \
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x ) \
    {                      \
    if( x != NULL )        \
	    {                  \
      delete[] x;          \
	  x = NULL;            \
	    }                  \
                           \
    }

#define IN_RANGE( value, min, max ) ( ((value) >= (min) && (value) <= (max)) ? 1 : 0 )
#define MIN( a, b ) ( ((a) < (b)) ? (a) : (b) )
#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define TO_BOOL( a )  ( (a != 0) ? true : false )

#define RADTODEG( a ) (a * 180 / 3.1416 )
#define DEGTORAD( a ) (a / 180 * 3.1416 )
#define CAP(x, m, M)	\
		{				\
	if (x < m)			\
		x = m;			\
	if (x > M)			\
		x = M;			\
		}

typedef unsigned int uint;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
typedef unsigned char uchar;

template <class VALUE_TYPE> void SWAP(VALUE_TYPE& a, VALUE_TYPE& b)
{
	VALUE_TYPE tmp = a;
	a = b;
	b = tmp;
}

// Standard string size
#define SHORT_STR	32
#define MID_STR		255
#define HUGE_STR	8192

// Joins a path and file
inline const char* const PATH(const char* folder, const char* file)
{
	static char path[MID_STR];
	sprintf_s(path, MID_STR, "%s/%s", folder, file);
	return path;
}

// Performance macros
#define PERF_START(timer) timer.Start()
#define PERF_PEEK(timer) LOG("%s took %f ms", __FUNCTION__, timer.ReadMs())

#define CURSOR_WIDTH 1
#define GUI_MAX_LAYERS 1

enum GUI_EVENTS
{
	UI_NONE,
	UI_MOUSE_ENTER,
	UI_MOUSE_EXIT,
	UI_MOUSE_DOWN,
	UI_MOUSE_UP,
	UI_ANIMATION_END,
	UI_KEYBOARD_FOCUSED,
	UI_KEYBOARD_CLICK,
	UI_TEXT_CHANGED,
	UIEACTIVATED,
	UI_ACTIVATED,
	UI_SLIDER_VALUE_CHANGED,
	UI_GET_FOCUS,
	UI_LOST_FOCUS
};

#endif