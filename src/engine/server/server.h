#pragma once

#include "puli.h"
#include "defs.h"

BOOM_HEADER_BEGIN

typedef struct LaunchOptions {
	const char* title;
	const char* root_dir;

	void (*setup_callback)(void);
	void (*shutdown_callback)(void);

	void (*update_callback)(double dt);
	void (*render_callback)(double dt);

	char launch_commands[8][128];
} LaunchOptions;

void InitGameServer();
void ShutdownGameServer();

BOOM_HEADER_END