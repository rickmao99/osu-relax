#pragma once

//#define TIME_SIGNATURE "\xDB\x5D\xF4\x8B\x45\xF4\xA3" // Stable (Fallback)
#define TIME_SIGNATURE "\xDB\x5D\xE8\x8B\x45\xE8\xA3" // Stable (Latest), Beta, Cutting Edge (Experimental)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern DWORD time_address;
extern DWORD game_process_id;
extern HANDLE game_process;

#include "beatmap.h"
#include "findpattern.h"
#include "functions.h"