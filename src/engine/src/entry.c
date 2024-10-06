#include "boom/entry.h"
#include "boom/debug.h"
#include "boom/str.h"
#include "boom/fsys.h"
#include "boom/console.h"
#include "boom/inputs.h"

#define STB_IMAGE_IMPLEMENTATION
#include <boom/texture.h>

#include "stb_image.h"

CVAR(vid_width, CVAR_FLAG_CLIENT_SIDE | CVAR_FLAG_USER_DATA);
CVAR(vid_height, CVAR_FLAG_CLIENT_SIDE | CVAR_FLAG_USER_DATA);
CVAR(vid_fullscreen, CVAR_FLAG_CLIENT_SIDE | CVAR_FLAG_USER_DATA);
CVAR(vid_monitor, CVAR_FLAG_CLIENT_SIDE | CVAR_FLAG_USER_DATA);

char g_BaseDir[BOOM_PATH_MAX] = { 0 };
char g_CfgPath[BOOM_PATH_MAX] = { 0 };
char g_SavePath[BOOM_PATH_MAX] = { 0 };

static SDL_Window* g_pWindow = NULL;
static SDL_GLContext g_pContext = NULL;

static GameUpdateFunction g_pGameUpdate = NULL;
static GameFixedUpdateFunction g_pGameFixedUpdate = NULL;
static GameRenderFunction g_pGameRender = NULL;
static GameShutdownFunction g_pShutdownFunction = NULL;

static double g_dFixedUpdateInterval = 1.0 / 60.0;

static bool g_bRunning = true;


LaunchOptions BoomParseLauncherOptions(int argc, TChar** argv) {
	LaunchOptions opt = { 0 };
	opt.width = opt.height = opt.fullscreen = opt.monitor = -1;
	memset(opt.initCommand, 0, 2049);

	int i = 1;
	while (i < argc) {
		if (TStrEquals(argv[i], TStr("--width"))) {
			if (++i < argc) {
				TStrToIntRadix(argv[i], &opt.width, 10);
			}
		} else if (TStrEquals(argv[i], TStr("--height"))) {
			if (++i < argc) {
				TStrToIntRadix(argv[i], &opt.height, 10);
			}
		} else if (TStrEquals(argv[i], TStr("--monitor"))) {
			if (++i < argc) {
				TStrToIntRadix(argv[i], &opt.monitor, 10);
			}
		} else if (TStrEquals(argv[i], TStr("--fullscreen"))) {
			if (++i < argc) {
				opt.fullscreen = 1;
			}
		} else if (TStrEquals(argv[i], TStr("--windowed"))) {
			if (++i < argc) {
				opt.fullscreen = 0;
			}
		} else if (TStrEquals(argv[i], TStr("--game"))) {
			if (++i < argc) {
#ifdef _WIN32
				PathDecl(temp);
				wcstombs(temp, argv[i], BOOM_PATH_MAX);
				temp[WStrLen(argv[i])] = '\0';
				opt.basedir = PathAbsolute(g_BaseDir, temp);
#else
				opt.basedir = PathAbsolute(g_BaseDir, argv[i]);
#endif
			}
#ifndef NDEBUG
		} else if (TStrEquals(argv[i], TStr("--ut"))) {
			if (++i < argc) {
#ifdef _WIN32
				wcstombs(opt.unittest, argv[i], 128);
#else
				StrCpy(opt.unittest, 128, argv[i]);
#endif
			}
#endif
		} else if (TStrStartWithChar(argv[i], '+')) {
			TChar commandLine[2048] = { 0 };
			TStrAppend(commandLine, 2048, argv[i] + 1);
			if (++i < argc) {
				while (i < argc && argv[i][0] != '-') {
					if (TStrAppendChar(commandLine, 2048, ' ') == NULL || TStrAppend(commandLine, 2048, argv[i]) == NULL) {
						i++;
						break;
					}
					i++;
				}
				TStrAppendChar(commandLine, 2048, ';');
			}
#ifdef _WIN32
			wcstombs(opt.initCommand, commandLine, 2048);
			opt.initCommand[wcslen(commandLine)] = '\0';
#else
			StrCpy(opt.initCommand, 2048, commandLine);
#endif
		}
	}

	if (opt.basedir == NULL) {
#ifdef _WIN32
		PathDecl(temp);
		wcstombs(temp, argv[0], BOOM_PATH_MAX);
		temp[wcslen(argv[0])] = '\0';
#else
		const char* temp = argv[0];
#endif
		PathGetParent(g_BaseDir, temp);
		PathParent(g_BaseDir);
		opt.basedir = g_BaseDir;
	}

	return opt;
}

bool BoomInitialize(int argc, TChar** argv) {
	return BoomInitializeCustom(BoomParseLauncherOptions(argc, argv));
}
bool BoomInitializeCustom(LaunchOptions opt) {
	if (opt.basedir != g_BaseDir) {
		StrCpy(g_BaseDir, BOOM_PATH_MAX, opt.basedir);
	}

	StrCpy(g_CfgPath, BOOM_PATH_MAX, g_BaseDir);
	PathAppend(g_CfgPath, "cfg");
	StrCpy(g_SavePath, BOOM_PATH_MAX, g_BaseDir);
	PathAppend(g_SavePath, "save");

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		DebugLog(LogError, "Failed to initialize SDL: %s", SDL_GetError());
		return false;
	}

	CVarRegister(&vid_width, "1360");
	CVarRegister(&vid_height, "768");
	CVarRegister(&vid_fullscreen, "0");
	CVarRegister(&vid_monitor, "0");

	if (opt.fnCVarReg != NULL) opt.fnCVarReg();

	ConsoleInitialize();
	ConsoleExec(opt.initCommand);

	int w = CVarGetInt("vid_width");
	int h = CVarGetInt("vid_height");
	int m = CVarGetInt("vid_monitor");
	bool fs = CVarGetBool("vid_fullscreen");

	if (opt.fullscreen >= 0) {
		fs = opt.fullscreen != 0;
	}
	if (opt.width > 0) {
		w = opt.width;
	}
	if (opt.height > 0) {
		h = opt.height;
	}
	if (opt.monitor >= 0) {
		m = opt.monitor;
	}

	int x = fs ? SDL_WINDOWPOS_UNDEFINED_DISPLAY(m) : SDL_WINDOWPOS_CENTERED_DISPLAY(m);
	int y = fs ? SDL_WINDOWPOS_UNDEFINED_DISPLAY(m) : SDL_WINDOWPOS_CENTERED_DISPLAY(m);

	if (fs) {
		SDL_DisplayMode mode;
		SDL_GetDesktopDisplayMode(m, &mode);
		w = mode.w;
		h = mode.h;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	g_pWindow = SDL_CreateWindow(opt.name == NULL ? "BOOM Engine" : opt.name, x, y, w, h, SDL_WINDOW_OPENGL);
	if (g_pWindow == NULL) {
		DebugLog(LogError, "Failed to create window: %s", SDL_GetError());
		BoomShutdown();
		return false;
	}

	g_pContext = SDL_GL_CreateContext(g_pWindow);
	if (g_pContext == NULL || SDL_GL_MakeCurrent(g_pWindow, g_pContext) != 0) {
		DebugLog(LogError, "Failed to create OpenGL context: %s", SDL_GetError());
		BoomShutdown();
		return false;
	}

	PathDecl(iconPath);
	StrCpy(iconPath, BOOM_PATH_MAX, g_BaseDir);
	PathAppend(iconPath, "res");
	PathAppend(iconPath, "icon.png");
	if (PathExists(iconPath)) {
		int iconW, iconH;
		uint8_t* data = stbi_load(iconPath, &iconW, &iconH, NULL, STBI_rgb_alpha);
		if (data != NULL) {
			SDL_Surface* pSurf = SDL_CreateRGBSurfaceFrom(data, iconW, iconH, 32, iconW * 4,
														  0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
			if (pSurf != NULL) {
				SDL_SetWindowIcon(g_pWindow, pSurf);
				SDL_FreeSurface(pSurf);
			}
			stbi_image_free(data);
		}
	}

	glewExperimental = GL_TRUE;
	GLenum glewErr;
	if ((glewErr = glewInit()) != GLEW_OK) {
		DebugLog(LogError, "Failed to initialize OpenGL: %s", (const char*)glewGetErrorString(glewErr));
		BoomShutdown();
		return false;
	}

	InputInit();

	if (opt.fnStartup != NULL) {
		opt.fnStartup();
	}

	g_pGameUpdate = opt.fnUpdate;
	g_pGameFixedUpdate = opt.fnFixedUpdate;
	g_pGameRender = opt.fnRender;
	g_pShutdownFunction = opt.fnShutdown;

	return true;
}

void BoomStartMainLoop() {
	SDL_Event event;
	double lastTime = (double)SDL_GetTicks64() / 1000.0;
	double nextFixedTime = lastTime + g_dFixedUpdateInterval;
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	while (g_bRunning) {
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				g_bRunning = false;
				break;
			}
			InputPoll(&event);
		}

		double time = (double)SDL_GetTicks64() / 1000.0;
		if (g_pGameUpdate != NULL) {
			g_pGameUpdate(lastTime - time);
		}

		if (nextFixedTime <= time) {
			if (g_pGameFixedUpdate != NULL) g_pGameFixedUpdate(g_dFixedUpdateInterval);
			nextFixedTime = time + g_dFixedUpdateInterval;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (g_pGameRender != NULL) {
			g_pGameRender(lastTime - time);
		}

		SDL_GL_SwapWindow(g_pWindow);
	}
	BoomShutdown();
}

void BoomShutdown() {
	if (g_pShutdownFunction != NULL) {
		g_pShutdownFunction();
	}

	ConsoleShutdown();
	TextureUnloadAll();
	DebugCloseLogFile();

	if (g_pContext != NULL) {
		SDL_GL_DeleteContext(g_pContext);
		g_pContext = NULL;
	}
	if (g_pWindow != NULL) {
		SDL_DestroyWindow(g_pWindow);
		g_pWindow = NULL;
	}

	SDL_Quit();
}

const char* BoomBaseDirectory() {
	return g_BaseDir;
}
const char* BoomCfgDirectory() {
	return g_CfgPath;
}
const char* BoomSaveDirectory() {
	return g_SavePath;
}

void BoomSetFixedUpdateInterval(double interval) {
	g_dFixedUpdateInterval = interval;
}
double BoomGetFixedUpdateInterval() {
	return g_dFixedUpdateInterval;
}