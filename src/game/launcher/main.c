#include <stdio.h>

#define SDL_MAIN_HANDLED

#include "boom/str.h"
#include "boom/fsys.h"
#include "boom/debug.h"
#include "boom/entry.h"

#ifdef _WIN32
	#define LIBNAME "game.dll"
#else
	#define LIBNAME "libgame.so"
#endif

#ifdef _WIN32
	#include <Windows.h>
	#include <wchar.h>
	#include <wctype.h>

	#define DllOpen(__path) ((void*)LoadLibraryA(__path))
	#define DllSym(__dll, __name) GetProcAddress((HMODULE)(__dll), __name)
	#define DllClose(__dll) FreeLibrary((HMODULE)(__dll))
#else
	#include <dlfcn.h>

	#define DllOpen(__path) dlopen(__path, RTLD_LAZY)
	#define DllSym(__dll, __name) dlsym(__dll, __name)
	#define DllClose(__dll) dlclose(__dll)
#endif

#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	int argc;
	wchar_t** argv = CommandLineToArgvW(pCmdLine, &argc);
#ifndef NDEBUG
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
#endif
#else
int main(int argc, char** argv) {
#endif
	SDL_SetMainReady();

	int exit_code = 0;
	LaunchOptions opt = BoomParseLauncherOptions(argc, argv);

	PathDecl(dllPath);
	StrCpy(dllPath, BOOM_PATH_MAX, opt.basedir);
	dllPath[strlen(opt.basedir)] = '\0';
	PathAppend(dllPath, "bin");
	PathAppend(dllPath, LIBNAME);

	void* dll = DllOpen(dllPath);
	if (dll == NULL) {
		DebugLog(LogError, "Could not open game library \"%s\"!", dllPath);
		exit_code = 1;
		goto FINALIZE;
	}

	const char* (*nameFn)() = (const char*(*)())DllSym(dll, "GameTitle");
	if (nameFn == NULL) {
		opt.name = "BOOM Engine";
	} else {
		opt.name = nameFn();
	}

	opt.fnStartup = (GameStartupFunction)DllSym(dll, "GameStartup");
	opt.fnCVarReg = (GameStartupFunction)DllSym(dll, "GameRegisterCVars");
	opt.fnShutdown = (GameShutdownFunction)DllSym(dll, "GameShutdown");
	opt.fnUpdate = (GameUpdateFunction)DllSym(dll, "GameUpdate");
	opt.fnFixedUpdate = (GameFixedUpdateFunction)DllSym(dll, "GameFixedUpdate");
	opt.fnRender = (GameRenderFunction)DllSym(dll, "GameRender");

	if (!BoomInitializeCustom(opt)) {
		exit_code = 1;
		goto FINALIZE;
	}

	BoomStartMainLoop();

FINALIZE:
	return exit_code;
}