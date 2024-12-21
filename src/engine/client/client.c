#include "client.h"

#include <debug.h>

#include "cvar.h"

static GLFWwindow* s_Window;
static const LaunchOptions* s_LaunchOptions;

color_t g_BackgroundColor = { 0.25f, 0.5f, 1.0f, 1.0f };

static void CVar_rClearColorSet(cvar_t* self);

CVAR(vid_width, CVAR_FLAG_CLIENT_SIDE | CVAR_FLAG_USER_DATA);
CVAR(vid_height, CVAR_FLAG_CLIENT_SIDE | CVAR_FLAG_USER_DATA);
CVAR(vid_fullscreen, CVAR_FLAG_CLIENT_SIDE | CVAR_FLAG_USER_DATA);
CVAR(vid_monitor, CVAR_FLAG_CLIENT_SIDE | CVAR_FLAG_USER_DATA);
CVAR_WITH_CALLBACK(r_clear_color, CVAR_FLAG_CLIENT_SIDE, &CVar_rClearColorSet);

static void s_glfwErrorCallbackFn(int error, const char* description) {
	if (error != GLFW_NO_ERROR) {
		DevErr("GLFW Error: %s (%d)", description, error);
	}
}
static void s_glfwWindowSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	// TODO: Resize viewport matrix
}
static void CVar_rClearColorSet(cvar_t* self) {
	g_BackgroundColor = cvar_to_color(self);
	glClearColor(g_BackgroundColor.r, g_BackgroundColor.g, g_BackgroundColor.b, g_BackgroundColor.a);
}

bool InitGameEngine(const LaunchOptions* options) {
	if (options == NULL) {
		DevErr("Launch options must not be null!");
		return false;
	}

	s_LaunchOptions = options;

	if (glfwInit() != GLFW_TRUE) {
		const char* errMsg;
		int err = glfwGetError(&errMsg);
		DevErr("Failed to initialize GLFW: %s (%d)", errMsg, err);
		return false;
	}

	cvar_register(&vid_width,      "1360");
	cvar_register(&vid_height,     "768");
	cvar_register(&vid_fullscreen, "0");
	cvar_register(&vid_monitor,    "0");
	cvar_register(&r_clear_color,  "0.25 0.5 1.0 1.0");

	console_init();

	long width      = options->window_width <= 0 ? cvar_get_int("vid_width") : options->window_width;
	long height     = options->window_height <= 0 ? cvar_get_int("vid_height") : options->window_height;
	long fullscreen = options->fullscreen < 0 ? cvar_get_int("vid_fullscreen") : options->fullscreen;
	long monitor    = options->target_monitor < 0 ? cvar_get_int("vid_monitor") : options->target_monitor;

	glfwSetErrorCallback(&s_glfwErrorCallbackFn);

	if (width <= 0) {
		DevWarn("Window width is invalid, it must be more then 0. Using default value \"1360\".");
		width = 1360;
	}
	if (height <= 0) {
		DevWarn("Window height is invalid, it must be more then 0. Using default value \"768\".");
		height = 768;
	}
	if (monitor < 0) {
		monitor = 0;
	}
	if (fullscreen < 0) {
		fullscreen = 0;
	}

	GLFWmonitor* targetMonitor = NULL;
	if (fullscreen != 0) {
		int monitorCount = 0;
		GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
		if (monitor >= monitorCount) {
			targetMonitor = glfwGetPrimaryMonitor();
			monitor = 0;
		} else {
			targetMonitor = monitors[monitor];
		}
	}

	cvar_set_int("vid_width", width);
	cvar_set_int("vid_height", height);
	cvar_set_int("vid_fullscreen", fullscreen);
	cvar_set_int("vid_monitor", monitor);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	s_Window = glfwCreateWindow((int)width, (int)height, options->title, targetMonitor, NULL);
	if (s_Window == NULL) {
		return false;
	}

	if (fullscreen == 0) {
		GLFWmonitor* curMonitor = glfwGetWindowMonitor(s_Window);
		if (curMonitor != NULL) {
			int mWidth = 0, mHeight = 0;
			glfwGetMonitorWorkarea(curMonitor, NULL, NULL, &mWidth, &mHeight);
			glfwSetWindowPos(s_Window, (int)(mWidth / 2 - width / 2), (int)(mHeight / 2 - height / 2));
		}
	}

	glfwMakeContextCurrent(s_Window);

#ifdef __APPLE__
	glewExperimental = GL_TRUE;
#endif
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		DevErr("Failed to initialize OpenGL: %s", (const char*)glewGetErrorString(glewError));
		return false;
	}

	if (options->setup_callback != NULL) options->setup_callback();

	glViewport(0, 0, (GLint)width, (GLint)height);

	glfwSetWindowSizeCallback(s_Window, &s_glfwWindowSizeCallback);

	// TODO: Start local server

	glClearColor(g_BackgroundColor.r, g_BackgroundColor.g, g_BackgroundColor.b, g_BackgroundColor.a);

	return true;
}

bool UpdateGameEngine() {
	glfwPollEvents();
	if (glfwWindowShouldClose(s_Window)) {
		return false;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glfwSwapBuffers(s_Window);
	return true;
}

void ShutdownGameEngine() {
	if (s_LaunchOptions->shutdown_callback != NULL) s_LaunchOptions->shutdown_callback();

	// TODO: Check and shutdown local server

	console_shutdown();

	if (s_Window != NULL) {
		glfwDestroyWindow(s_Window);
	}
	glfwTerminate();
}
