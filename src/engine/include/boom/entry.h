#pragma once

#include "defs.h"
#include "str.h"
#include "cvar.h"

BOOM_HEADER_START

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

typedef void (*GameStartupFunction)();
typedef void (*GameUpdateFunction)(double dt);
typedef void (*GameFixedUpdateFunction)(double dt);
typedef void (*GameRenderFunction)(double dt);
typedef void (*GameShutdownFunction)();

typedef struct LaunchOptions {
	const char* name;
	const char* basedir;

	GameStartupFunction fnCVarReg;
	GameStartupFunction fnStartup;
	GameUpdateFunction fnUpdate;
	GameFixedUpdateFunction fnFixedUpdate;
	GameRenderFunction fnRender;
	GameShutdownFunction fnShutdown;

	int width, height, fullscreen, monitor;
	char initCommand[2049];
#ifndef NDEBUG
	char unittest[128];
#endif
} LaunchOptions;

BOOM_API LaunchOptions BoomParseLauncherOptions(int argc, TChar** argv);

BOOM_API bool BoomInitialize(int argc, TChar** argv);
BOOM_API bool BoomInitializeCustom(LaunchOptions opt);
BOOM_API void BoomStartMainLoop();
BOOM_API void BoomShutdown();

BOOM_API const char* BoomBaseDirectory();
BOOM_API const char* BoomCfgDirectory();
BOOM_API const char* BoomSaveDirectory();

BOOM_API void BoomSetFixedUpdateInterval(double interval);
BOOM_API double BoomGetFixedUpdateInterval();

BOOM_HEADER_END
