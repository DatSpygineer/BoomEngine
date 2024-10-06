#pragma once

#include "cvar.h"
#include "console.h"
#include "entry.h"
#include "debug.h"
#include "inputs.h"
#include "fsys.h"
#include "str.h"

#ifdef __cplusplus
	#ifdef _WIN32
		#define GAME_DLL extern "C" __declspec(dllexport)
	#else
		#define GAME_DLL extern "C" __attribute__((visibility("default")))
	#endif
#else
	#ifdef _WIN32
		#define GAME_DLL __declspec(dllexport)
	#else
		#define GAME_DLL __attribute__((visibility("default")))
	#endif
#endif