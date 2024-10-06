#pragma once

#include "defs.h"
#include "cvar.h"

#include <SDL2/SDL.h>

BOOM_HEADER_START

#ifndef BOOM_INPUT_ACTION_MAX_KEYS
	#define BOOM_INPUT_ACTION_MAX_KEYS 8
#endif

typedef enum InputDeviceType {
	INPUT_DEVICE_KEYBOARD,
	INPUT_DEVICE_MOUSE,
	INPUT_DEVICE_MOUSE_WHEEL,
	INPUT_DEVICE_MOUSE_MOTION,
	INPUT_DEVICE_CONTROLLER,
	INPUT_DEVICE_CONTROLLER_STICK,
} InputDeviceType;

typedef enum InputKeyState {
	KEY_STATE_NONE,
	KEY_STATE_PRESSED,
	KEY_STATE_HELD,
	KEY_STATE_RELEASED,
	KEY_STATE_SIM_OFF = 0xFF
} InputKeyState;

typedef struct InputKey {
	int code;
	InputDeviceType device;
} InputKey;

typedef struct InputAction InputAction;
struct InputAction {
	const char* name;
	size_t keyCount;
	InputKey keys[BOOM_INPUT_ACTION_MAX_KEYS];
	InputKeyState symState;

	InputAction* next;
};

#define INPUT_ACTION(__name) InputAction __name##_action = { .name = #__name, .keyCount = 0, .next = NULL, .symState = KEY_STATE_SIM_OFF }

BOOM_API void InputInit();
BOOM_API void InputPoll(SDL_Event* event);
BOOM_API void InputWriteOutKeys(FILE* f);

BOOM_API bool InputGetKeyFromString(const char* keyname, int* code, InputDeviceType* deviceType);
BOOM_API bool InputGetStringFromKey(int code, InputDeviceType deviceType, char* buf, size_t maxBufSize);

BOOM_API bool InputRegisterAction(InputAction* action);
BOOM_API InputAction* InputFindAction(const char* name);

BOOM_API bool InputBind(const char* name, int code, InputDeviceType device);
BOOM_API void InputUnbind(int code, InputDeviceType device);
BOOM_API void InputUnbindAll();
BOOM_API bool InputIsPressed(const char* name);
BOOM_API bool InputIsReleased(const char* name);
BOOM_API bool InputIsDown(const char* name);
BOOM_API bool InputIsUp(const char* name);
BOOM_API void InputSimAction(const char* name, InputKeyState state);

BOOM_API void InputGetMousePos(int* x, int* y);
BOOM_API void InputGetMousePosRelative(int* x, int* y);

BOOM_HEADER_END