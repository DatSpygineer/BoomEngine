#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "defs.h"

BOOM_HEADER_BEGIN

typedef struct LaunchOptions {
	const char* title;
	const char* root_dir;
	int window_width, window_height, target_monitor, fullscreen;

	void (*setup_callback)(void);
	void (*shutdown_callback)(void);

	void (*update_callback)(double dt);
	void (*render_callback)(double dt);

	char launch_commands[8][128];
} LaunchOptions;

BOOM_API bool InitGameEngine(const LaunchOptions* options);
BOOM_API bool UpdateGameEngine();
BOOM_API void ShutdownGameEngine();

BOOM_HEADER_END