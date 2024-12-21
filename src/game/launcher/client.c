#include <client.h>
#include <debug.h>
#include <dlfcn.h>

#include "puli.h"

typedef void (*clInitFunc)(void);
typedef void (*clUpdateFunc)(double);

#ifdef _WIN32
#include <windows.h>

static char** CommandLineToArgvA(int* argc) {
	wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), argc);
	char** argv = (char**)malloc(sizeof(char*) * *argc);
	for (int i = 0; i < *argc; i++) {
		const size_t len = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, NULL, 0, NULL, NULL);
		argv[i] = (char*)malloc(len + 1);
		WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, argv[i], len, NULL, NULL);
	}
	return argv;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hpInstance, LPSTR nCmdLine, int iCmdShow) {
	int argc;
	char** argv = CommandLineToArgvA(&argc);
#else
int main(int argc, char** argv) {
#endif
	P_PATH_BUFFER(basedir);
	pPathParent(basedir, argv[0]);

	P_PATH_BUFFER(dllpath);
	pStrCpy(dllpath, P_PATH_MAX, basedir);
	pPathAppend(dllpath, "bin");
#ifdef _WIN32
	SetDllDirectoryA(dllpath);
#endif

#ifdef _WIN32
	pPathAppend(dllpath, "client.dll");
#else
	pPathAppend(dllpath, "libclient.so");
#endif

	pSetupDialogs(argc, argv);

	int code = 0;

	LaunchOptions options;

	for (int i = 1; i < argc;) {
		if (strcmp(argv[i], "--width") == 0) {
			if (++i < argc) {
				pStrToInt(argv[i++], &options.window_width);
			}
		}
		if (strcmp(argv[i], "--height") == 0) {
			if (++i < argc) {
				pStrToInt(argv[i++], &options.window_height);
			}
		}
		if (strcmp(argv[i], "--fullscreen") == 0) {
			options.fullscreen = true;
			i++;
		}
		if (strcmp(argv[i], "--windowed") == 0) {
			options.fullscreen = false;
			i++;
		}
		if (strcmp(argv[i], "--game") == 0) {
			if (++i < argc) {
				const char* name = argv[i++];
				pPathAbsolute(basedir, name);
				memset(dllpath, 0, P_PATH_MAX);
				pStrCpy(dllpath, P_PATH_MAX, basedir);
				pPathAppend(dllpath, "bin");
#ifdef _WIN32
				pPathAppend(dllpath, "client.dll");
#else
				pPathAppend(dllpath, "libclient.so");
#endif
			}
		}
	}

	pPathSetCurrentDir(basedir);

	void* game_dll = dlopen(dllpath, RTLD_LAZY);
	if (game_dll == NULL) {
		DevErr("Failed to open game library \"%s\"", dllpath);
		code = -1;
		goto FINALIZE;
	}

	void* initFn = dlsym(game_dll, "client_main");
	void* closeFn = dlsym(game_dll, "client_exit");
	void* upFn = dlsym(game_dll, "client_update");
	void* renderFn = dlsym(game_dll, "client_render");

	if (initFn == NULL) {
		DevErr("Failed to get \"client_main\" function from game library!");
		code = -1;
		goto FINALIZE;
	}

	char title[64] = { 0 };
	pPathGetFilename(title, 64, basedir);

	options.root_dir = basedir;
	options.setup_callback = initFn;
	options.shutdown_callback = closeFn;
	options.update_callback = upFn;
	options.render_callback = renderFn;
	options.title = title;

	if (!InitGameEngine(&options)) {
		code = -1;
		goto FINALIZE;
	}

	while (UpdateGameEngine());

	ShutdownGameEngine();

FINALIZE:
	if (game_dll != NULL) {
		dlclose(game_dll);
	}

#ifdef _WIN32
	for (int i = 0; i < argc; i++) {
		free(argv[i]);
	}
	free(argv);
#endif

	pDisposeDialogs();

	return code;
}