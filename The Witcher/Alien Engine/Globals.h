#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 ) // Warning that exceptions are disabled

#include <windows.h>
#include <stdio.h>
#include "glew/include/glew.h"

// DISCOMMENT TO START THE ENGINE IN PLAY MODE
//#define GAME_VERSION

#define LOG_ENGINE(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

__declspec(dllexport) void log(const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define HAVE_M_PI
#define ALIEN_ENGINE_VERSION 1.0

#define RELEASE( x )\
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x )\
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }

typedef unsigned int uint;
typedef unsigned char uchar;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_SIZE 1
#define VSYNC true

#ifndef GAME_VERSION
#define WINDOW_ICON_WIDTH 457
#define WINDOW_ICON_HEIGHT 300
#define WINDOW_ICON_PATH "Configuration/Engine Icons/Logo_Name.bmp"
#define WINDOW_ICON_BAR_POSY 280
#else
#define WINDOW_ICON_WIDTH 457
#define WINDOW_ICON_HEIGHT 300
#define WINDOW_ICON_PATH "Configuration/Engine Icons/Logo_Name_madewith.bmp"
#define WINDOW_ICON_BAR_POSY 280
#endif

#define BAR_BEGIN_POS 6
#define BAR_END_POS WINDOW_ICON_WIDTH - BAR_BEGIN_POS

#define DEGTORAD 0.0174532925199432957f // 1degree x (pi rads / 180 degrees) = 0.017 rads
#define RADTODEG 57.295779513082320876f // 1rad x (180 degrees / pi rads) = 57 degrees