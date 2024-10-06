#include "boom/inputs.h"
#include "boom/debug.h"
#include "boom/str.h"
#include "boom/b_math.h"

BOOM_API CCOMMAND_CUSTOM_NAME(key_bind, "bind", CVAR_FLAG_CLIENT_SIDE) {
	if (argc < 3) {
		DebugLog(LogWarn, "Failed to run command 'bind': Expected 2 arguments. Usage: bind <key> <action>");
	} else {
		const char* key = argv[1];
		const char* action = argv[2];

		int code;
		InputDeviceType device;
		if (InputGetKeyFromString(key, &code, &device)) {
			InputBind(action, code, device);
		}
	}
}
BOOM_API CCOMMAND_CUSTOM_NAME(key_unbind, "unbind", CVAR_FLAG_CLIENT_SIDE) {
	if (argc < 2) {
		DebugLog(LogWarn, "Failed to run command 'unbind': Expected an argument. Usage: unbind <key>");
	} else {
		const char* key = argv[1];

		int code;
		InputDeviceType device;
		if (InputGetKeyFromString(key, &code, &device)) {
			InputUnbind(code, device);
		}
	}
}
BOOM_API CCOMMAND_CUSTOM_NAME(key_unbindall, "unbindall", CVAR_FLAG_CLIENT_SIDE) {
	if (argc < 2) {
		DebugLog(LogWarn, "Failed to run command 'unbind': Expected an argument. Usage: unbind <key>");
	} else {
		InputUnbindAll();
	}
}

BOOM_API CCOMMAND(simkey, CVAR_FLAG_CLIENT_SIDE) {
	if (argc < 3) {
		DebugLog(LogWarn, "Failed to run command 'simkey': Expected 2 arguments. Usage: simkey <action> <state>\nStates:\n- none\n- pressed\n- held\n- released");
	} else {
		const char* action = argv[1];
		const char* stateStr = argv[2];
		InputKeyState state = KEY_STATE_NONE;
		if (StrEqualsNoCase(stateStr, "none")) {
			state = KEY_STATE_NONE;
		} else if (StrEqualsNoCase(stateStr, "pressed")) {
			state = KEY_STATE_PRESSED;
		} else if (StrEqualsNoCase(stateStr, "held")) {
			state = KEY_STATE_HELD;
		} else if (StrEqualsNoCase(stateStr, "released")) {
			state = KEY_STATE_RELEASED;
		}

		InputSimAction(action, state);
	}
}

BOOM_API InputAction* g_pAction;
InputAction* g_pAction = NULL;

BOOM_API bool KeyboardInputs[256];
BOOM_API bool KeyboardInputsLast[256];
BOOM_API bool MouseInputs[5];
BOOM_API bool MouseInputsLast[5];
BOOM_API int MouseWheel;
BOOM_API int MousePosition[2];
BOOM_API int MouseMotion[2];

bool KeyboardInputs[256] = { 0 };
bool KeyboardInputsLast[256] = { 0 };
bool MouseInputs[5] = { 0 };
bool MouseInputsLast[5] = { 0 };
int MouseWheel = 0;
int MousePosition[2] = { 0 };
int MouseMotion[2] = { 0 };

enum {
	KEYCODE_MOUSE_MOTION_X_POS,
	KEYCODE_MOUSE_MOTION_X_NEG,
	KEYCODE_MOUSE_MOTION_Y_POS,
	KEYCODE_MOUSE_MOTION_Y_NEG,
};

bool InputGetKeyFromString(const char* keyname, int* code, InputDeviceType* deviceType) {
	if (StrLen(keyname) == 0) {
		DebugLog(LogWarn, "Key string is NULL or empty!");
		return false;
	}

	*deviceType = INPUT_DEVICE_KEYBOARD;
	if (StrLen(keyname) == 1) {
		char c = keyname[0];
		if (CharIsLetter(c)) {
			*code = SDLK_a + (CharToLowerCase(c) - 'a');
			return true;
		} else if (CharIsNumber(c)) {
			*code = SDLK_0 + (c - '0');
			return true;
		} else {
			DebugLog(LogWarn, "Unrecognized key '%c'", c);
			return false;
		}
	} else if (StrStartWithChar(keyname, 'f') || StrStartWithChar(keyname, 'F')) {
		unsigned fKey;
		if (StrToUIntRadix(keyname + 1, &fKey, 10)) {
			if (fKey <= 12) {
				*code = SDLK_F1 + (int)fKey;
				return true;
			} else {
				DebugLog(LogWarn, "Unrecognized key '%s'", keyname);
				return false;
			}
		}
	} else if (StrEqualsNoCaseN(keyname, "kp_", 3)) {
		unsigned kpKey;
		if (StrToUIntRadix(keyname + 1, &kpKey, 10)) {
			if (kpKey < 10) {
				*code = SDLK_KP_0 + (int)kpKey;
				return true;
			} else {
				DebugLog(LogWarn, "Unrecognized key '%s'", keyname);
				return false;
			}
		} else {
			if (StrEqualsNoCase(keyname, "kp_enter")) {
				*code = SDLK_KP_ENTER;
			} else if (StrEqualsNoCase(keyname, "kp_divide")) {
				*code = SDLK_KP_DIVIDE;
			} else if (StrEqualsNoCase(keyname, "kp_multiply")) {
				*code = SDLK_KP_MULTIPLY;
			} else if (StrEqualsNoCase(keyname, "kp_plus")) {
				*code = SDLK_KP_PLUS;
			} else if (StrEqualsNoCase(keyname, "kp_minus")) {
				*code = SDLK_KP_MINUS;
			} else if (StrEqualsNoCase(keyname, "kp_comma")) {
				*code = SDLK_KP_COMMA;
			} else {
				DebugLog(LogWarn, "Unrecognized key '%s'", keyname);
				return false;
			}
			return true;
		}
	} else if (StrEqualsNoCase(keyname, "up") || StrEqualsNoCase(keyname, "up_arrow")) {
		*code = SDLK_UP;
		return true;
	} else if (StrEqualsNoCase(keyname, "down") || StrEqualsNoCase(keyname, "down_arrow")) {
		*code = SDLK_DOWN;
		return true;
	} else if (StrEqualsNoCase(keyname, "left") || StrEqualsNoCase(keyname, "left_arrow")) {
		*code = SDLK_LEFT;
		return true;
	} else if (StrEqualsNoCase(keyname, "right") || StrEqualsNoCase(keyname, "right_arrow")) {
		*code = SDLK_RIGHT;
		return true;
	} else if (StrEqualsNoCase(keyname, "ins") || StrEqualsNoCase(keyname, "insert")) {
		*code = SDLK_INSERT;
		return true;
	} else if (StrEqualsNoCase(keyname, "del") || StrEqualsNoCase(keyname, "delete")) {
		*code = SDLK_DELETE;
		return true;
	} else if (StrEqualsNoCase(keyname, "home")) {
		*code = SDLK_HOME;
		return true;
	} else if (StrEqualsNoCase(keyname, "end")) {
		*code = SDLK_END;
		return true;
	} else if (StrEqualsNoCase(keyname, "pg_up")) {
		*code = SDLK_PAGEUP;
		return true;
	} else if (StrEqualsNoCase(keyname, "pg_down")) {
		*code = SDLK_PAGEDOWN;
		return true;
	} else if (StrEqualsNoCase(keyname, "pause")) {
		*code = SDLK_PAUSE;
		return true;
	} else if (StrEqualsNoCase(keyname, "scroll_lock")) {
		*code = SDLK_SCROLLLOCK;
		return true;
	} else if (StrEqualsNoCase(keyname, "num_lock")) {
		*code = SDLK_NUMLOCKCLEAR;
		return true;
	} else if (StrEqualsNoCase(keyname, "caps_lock")) {
		*code = SDLK_CAPSLOCK;
		return true;
	} else if (StrEqualsNoCase(keyname, "tab")) {
		*code = SDLK_TAB;
		return true;
	} else if (StrEqualsNoCase(keyname, "space")) {
		*code = SDLK_SPACE;
		return true;
	} else if (StrEqualsNoCase(keyname, "backspace")) {
		*code = SDLK_BACKSPACE;
		return true;
	} else if (StrEqualsNoCase(keyname, "enter") || StrEqualsNoCase(keyname, "return")) {
		*code = SDLK_RETURN;
		return true;
	} else if (StrEqualsNoCase(keyname, "esc") || StrEqualsNoCase(keyname, "escape")) {
		*code = SDLK_ESCAPE;
		return true;
	} else if (StrEqualsNoCase(keyname, "semicolon")) {
		*code = SDLK_SEMICOLON;
		return true;
	} else if (StrEqualsNoCase(keyname, "period")) {
		*code = SDLK_PERIOD;
		return true;
	} else if (StrEqualsNoCase(keyname, "comma")) {
		*code = SDLK_COMMA;
		return true;
	} else if (StrEqualsNoCase(keyname, "plus")) {
		*code = SDLK_PLUS;
		return true;
	} else if (StrEqualsNoCase(keyname, "minus")) {
		*code = SDLK_MINUS;
		return true;
	} else if (StrEqualsNoCase(keyname, "colon")) {
		*code = SDLK_COLON;
		return true;
	} else if (StrEqualsNoCase(keyname, "at")) {
		*code = SDLK_AT;
		return true;
	} else if (StrEqualsNoCase(keyname, "ctrl") || StrEqualsNoCase(keyname, "lctrl")) {
		*code = SDLK_LCTRL;
		return true;
	} else if (StrEqualsNoCase(keyname, "rctrl")) {
		*code = SDLK_RCTRL;
		return true;
	} else if (StrEqualsNoCase(keyname, "shift") || StrEqualsNoCase(keyname, "lshift")) {
		*code = SDLK_LSHIFT;
		return true;
	} else if (StrEqualsNoCase(keyname, "rshift")) {
		*code = SDLK_RSHIFT;
		return true;
	} else if (StrEqualsNoCase(keyname, "alt") || StrEqualsNoCase(keyname, "lalt")) {
		*code = SDLK_LALT;
		return true;
	} else if (StrEqualsNoCase(keyname, "ralt")) {
		*code = SDLK_RALT;
		return true;
	} else if (StrEqualsNoCase(keyname, "win") || StrEqualsNoCase(keyname, "lwin")) {
		*code = SDLK_LGUI;
		return true;
	} else if (StrEqualsNoCase(keyname, "rwin")) {
		*code = SDLK_RGUI;
		return true;
	} else if (StrEqualsNoCase(keyname, "menu")) {
		*code = SDLK_MENU;
		return true;
	}

	*deviceType = INPUT_DEVICE_MOUSE;
	if (StrEqualsNoCaseN(keyname, "mouse", 5)) {
		if (StrEqualsNoCase(keyname, "mouse_left")) {
			*code = 0;
		} else if (StrEqualsNoCase(keyname, "mouse_right")) {
			*code = 1;
		} else if (StrEqualsNoCase(keyname, "mouse_center")) {
			*code = 2;
		} else if (StrEqualsNoCase(keyname, "mouse_x_pos")) {
			*deviceType = INPUT_DEVICE_MOUSE_MOTION;
			*code = KEYCODE_MOUSE_MOTION_X_POS;
		} else if (StrEqualsNoCase(keyname, "mouse_x_neg")) {
			*deviceType = INPUT_DEVICE_MOUSE_MOTION;
			*code = KEYCODE_MOUSE_MOTION_X_NEG;
		} else if (StrEqualsNoCase(keyname, "mouse_y_pos")) {
			*deviceType = INPUT_DEVICE_MOUSE_MOTION;
			*code = KEYCODE_MOUSE_MOTION_Y_POS;
		} else if (StrEqualsNoCase(keyname, "mouse_y_neg")) {
			*deviceType = INPUT_DEVICE_MOUSE_MOTION;
			*code = KEYCODE_MOUSE_MOTION_Y_NEG;
		} else if (StrEqualsNoCase(keyname, "mwheelup") || StrEqualsNoCase(keyname, "mouse_wheel_up")) {
			*deviceType = INPUT_DEVICE_MOUSE_WHEEL;
			*code = 1;
		} else if (StrEqualsNoCase(keyname, "mwheeldown") || StrEqualsNoCase(keyname, "mouse_wheel_down")) {
			*deviceType = INPUT_DEVICE_MOUSE_WHEEL;
			*code = -1;
		} else {
			int mb;
			if (StrToIntRadix(keyname + 5, &mb, 10)) {
				*code = mb;
			} else {
				DebugLog(LogWarn, "Unrecognized key '%s'", keyname);
				return false;
			}
		}
		return true;
	}

	DebugLog(LogWarn, "Unrecognized key '%s'", keyname);
	return false;
}
bool InputGetStringFromKey(int code, InputDeviceType deviceType, char* buf, size_t maxBufSize) {
	if (buf == NULL) return false;

	switch (deviceType) {
		case INPUT_DEVICE_KEYBOARD: {
			if (code >= SDLK_a && code <= SDLK_z) {
				snprintf(buf, maxBufSize, "%c", 'A' + (code - SDLK_a));
			} else if (code >= SDLK_0 && code <= SDLK_9) {
				snprintf(buf, maxBufSize, "%d", (code - SDLK_0));
			} else if (code >= SDLK_KP_1 && code <= SDLK_KP_9) {
				snprintf(buf, maxBufSize, "kp_%d", ((code - SDLK_KP_1) + 1));
			} else if (code >= SDLK_F1 && code <= SDLK_F12) {
				snprintf(buf, maxBufSize, "F%d", ((code - SDLK_F1) + 1));
			} else {
				switch (code) {
					case SDLK_KP_ENTER: StrCpy(buf, maxBufSize, "kp_enter"); break;
					case SDLK_KP_PLUS: StrCpy(buf, maxBufSize, "kp_plus"); break;
					case SDLK_KP_MINUS: StrCpy(buf, maxBufSize, "kp_minus"); break;
					case SDLK_KP_DIVIDE: StrCpy(buf, maxBufSize, "kp_divide"); break;
					case SDLK_KP_MULTIPLY: StrCpy(buf, maxBufSize, "kp_multiply"); break;
					case SDLK_KP_COMMA: StrCpy(buf, maxBufSize, "kp_comma"); break;
					case SDLK_RETURN: StrCpy(buf, maxBufSize, "enter"); break;
					case SDLK_ESCAPE: StrCpy(buf, maxBufSize, "esc"); break;
					case SDLK_PAUSE: StrCpy(buf, maxBufSize, "pause"); break;
					case SDLK_HOME: StrCpy(buf, maxBufSize, "home"); break;
					case SDLK_END: StrCpy(buf, maxBufSize, "end"); break;
					case SDLK_INSERT: StrCpy(buf, maxBufSize, "ins"); break;
					case SDLK_PAGEUP: StrCpy(buf, maxBufSize, "page_up"); break;
					case SDLK_PAGEDOWN: StrCpy(buf, maxBufSize, "page_down"); break;
					case SDLK_UP: StrCpy(buf, maxBufSize, "up"); break;
					case SDLK_DOWN: StrCpy(buf, maxBufSize, "down"); break;
					case SDLK_LEFT: StrCpy(buf, maxBufSize, "left"); break;
					case SDLK_RIGHT: StrCpy(buf, maxBufSize, "right"); break;
					case SDLK_TAB: StrCpy(buf, maxBufSize, "tab"); break;
					case SDLK_SPACE: StrCpy(buf, maxBufSize, "space"); break;
					case SDLK_BACKSPACE: StrCpy(buf, maxBufSize, "backspace"); break;
					case SDLK_CAPSLOCK: StrCpy(buf, maxBufSize, "caps_lock"); break;
					case SDLK_NUMLOCKCLEAR: StrCpy(buf, maxBufSize, "num_lock"); break;
					case SDLK_SCROLLLOCK: StrCpy(buf, maxBufSize, "scroll_lock"); break;
					case SDLK_LCTRL: StrCpy(buf, maxBufSize, "lctrl"); break;
					case SDLK_RCTRL: StrCpy(buf, maxBufSize, "rctrl"); break;
					case SDLK_LSHIFT: StrCpy(buf, maxBufSize, "lshift"); break;
					case SDLK_RSHIFT: StrCpy(buf, maxBufSize, "rshift"); break;
					case SDLK_LALT: StrCpy(buf, maxBufSize, "lalt"); break;
					case SDLK_RALT: StrCpy(buf, maxBufSize, "ralt"); break;
					case SDLK_LGUI: StrCpy(buf, maxBufSize, "lwin"); break;
					case SDLK_RGUI: StrCpy(buf, maxBufSize, "rwin"); break;
					case SDLK_SEMICOLON: StrCpy(buf, maxBufSize, "semicolon"); break;
					case SDLK_COMMA: StrCpy(buf, maxBufSize, "comma"); break;
					case SDLK_PERIOD: StrCpy(buf, maxBufSize, "period"); break;
					case SDLK_PLUS: StrCpy(buf, maxBufSize, "plus"); break;
					case SDLK_MINUS: StrCpy(buf, maxBufSize, "minus"); break;
					case SDLK_COLON: StrCpy(buf, maxBufSize, "colon"); break;
					case SDLK_AT: StrCpy(buf, maxBufSize, "at"); break;
					default: { DebugLog(LogWarn, "Unsupported keycode '%d'", code); return false; }
				}
			}
		} break;
		case INPUT_DEVICE_MOUSE: {
			snprintf(buf, maxBufSize, "mouse%d", code);
		} break;
		case INPUT_DEVICE_MOUSE_WHEEL: {
			if (code == 0) {
				DebugLog(LogWarn, "Invalid value for input type \"Mouse wheel\": 0, expected 1 or -1");
				return false;
			}
			StrCpy(buf, maxBufSize, code > 0 ? "mwheelup" : "mwheeldown");
		} break;
		case INPUT_DEVICE_MOUSE_MOTION: {
			switch (code) {
				case KEYCODE_MOUSE_MOTION_X_POS: StrCpy(buf, maxBufSize, "mouse_x_pos"); break;
				case KEYCODE_MOUSE_MOTION_X_NEG: StrCpy(buf, maxBufSize, "mouse_x_neg"); break;
				case KEYCODE_MOUSE_MOTION_Y_POS: StrCpy(buf, maxBufSize, "mouse_y_pos"); break;
				case KEYCODE_MOUSE_MOTION_Y_NEG: StrCpy(buf, maxBufSize, "mouse_y_neg"); break;
				default: {
					DebugLog(LogWarn, "Invalid value for input type \"Mouse motion\": %d, expected a number in range 0 - 3", code);
					return false;
				}
			}
		} break;
		default: { DebugLog(LogWarn, "Unsupported input type!"); return false; }
	}

	return true;
}

void InputInit() {
	CVarRegister(&key_bind, NULL);
	CVarRegister(&key_unbind, NULL);
	CVarRegister(&key_unbindall, NULL);

	int mx, my;
	SDL_GetMouseState(&mx, &my);
	MousePosition[0] = mx;
	MousePosition[1] = my;
}
void InputPoll(SDL_Event* event) {
	int keyCount;
	const uint8_t* keys = SDL_GetKeyboardState(&keyCount);
	for (int i = 0; i < Min(keyCount, 255); i++) {
		KeyboardInputsLast[i] = KeyboardInputs[i];
		KeyboardInputs[i] = keys[i];
	}

	uint32_t mb = SDL_GetMouseState(NULL, NULL);
	for (int i = 0; i < 5; i++) {
		MouseInputsLast[i] = MouseInputs[i];
		MouseInputs[i] = mb & SDL_BUTTON(i + 1);
	}

	switch (event->type) {
		case SDL_MOUSEWHEEL: {
			MouseWheel = event->wheel.y;
		} break;
		case SDL_MOUSEMOTION: {
			MousePosition[0] = event->motion.x;
			MousePosition[1] = event->motion.y;
			MouseMotion[0] = event->motion.xrel;
			MouseMotion[1] = event->motion.yrel;
		} break;
	}
}
void InputWriteOutKeys(FILE* f) {
	InputAction* action = g_pAction;
	char keyName[64] = { 0 };
	while (action != NULL) {
		for (int i = 0; i < action->keyCount; i++) {
			InputGetStringFromKey(action->keys[i].code, action->keys[i].device, keyName, 64);
			fprintf(f, "bind \"%s\" \"%s\"\n", keyName, action->name);
		}
		action = action->next;
	}
}

static int InputGetKeyState(InputKey key) {
	switch (key.device) {
		case INPUT_DEVICE_KEYBOARD: {
			const int code = SDL_GetScancodeFromKey(key.code);
			const int dir = KeyboardInputs[code] - KeyboardInputsLast[code];
			if (dir == 0) {
				return KeyboardInputs[code] ? KEY_STATE_HELD : KEY_STATE_NONE;
			} else if (dir > 0) {
				return KEY_STATE_PRESSED;
			} else {
				return KEY_STATE_RELEASED;
			}
		}
		case INPUT_DEVICE_MOUSE: {
			const int dir = MouseInputs[key.code] - MouseInputsLast[key.code];
			if (dir == 0) {
				return MouseInputs[key.code] ? KEY_STATE_HELD : KEY_STATE_NONE;
			} else if (dir > 0) {
				return KEY_STATE_PRESSED;
			} else {
				return KEY_STATE_RELEASED;
			}
		}
		case INPUT_DEVICE_MOUSE_WHEEL: {
			return MouseWheel == key.code;
		}
		case INPUT_DEVICE_MOUSE_MOTION: {
			switch (key.code) {
				case KEYCODE_MOUSE_MOTION_X_POS: return MouseMotion[0] > 0;
				case KEYCODE_MOUSE_MOTION_X_NEG: return MouseMotion[0] < 0;
				case KEYCODE_MOUSE_MOTION_Y_POS: return MouseMotion[1] > 0;
				case KEYCODE_MOUSE_MOTION_Y_NEG: return MouseMotion[1] < 0;
				default: return 0;
			}
		}
		default: return 0;
	}
}

bool InputRegisterAction(InputAction* action) {
	if (action != NULL && InputFindAction(action->name) == NULL) {
		if (g_pAction == NULL) {
			g_pAction = action;
		} else {
			InputAction* tail = g_pAction;
			while (tail->next) {
				tail = tail->next;
			}
			tail->next = action;
		}
	} else {
		if (action == NULL) {
			DebugLog(LogWarn, "Input action is NULL!");
			return false;
		} else {
			DebugLog(LogWarn, "Input action \"%s\" is already defined!", action->name);
			return false;
		}
	}
	return true;
}
InputAction* InputFindAction(const char* name) {
	InputAction* cur = g_pAction;
	while (cur != NULL) {
		if (StrEqualsNoCase(cur->name, name)) {
			return cur;
		}
		cur = cur->next;
	}
	return NULL;
}
bool InputBind(const char* name, int code, InputDeviceType device) {
	InputAction* action = InputFindAction(name);
	if (action == NULL) {
		DebugLog(LogWarn, "Failed to bind input \"%s\": No such input action is defined!", name);
		return false;
	}
	if (action->keyCount >= BOOM_INPUT_ACTION_MAX_KEYS) {
		DebugLog(LogWarn, "Failed to bind input \"%s\": Input action key buffer is full!", name);
		return false;
	}

	for (int i = 0; i < action->keyCount; i++) {
		if (action->keys[i].code == code && action->keys[i].device == device) {
			char keyName[64];
			InputGetStringFromKey(code, device, keyName, 64);
			DebugLog(LogWarn, "Input action \"%s\" has the key %s already bound!", keyName);
			return false;
		}
	}

	action->keys[action->keyCount++] = (InputKey){
		.code = code,
		.device = device
	};
	return true;
}
void InputUnbind(int code, InputDeviceType device) {
	InputAction* action = g_pAction;
	while (action != NULL) {
		for (int i = 0; i < action->keyCount; i++) {
			if (action->keys[i].code == code && action->keys[i].device == device) {
				memcpy(&action->keys[i], &action->keys[i + 1], (action->keyCount - i + 1) * sizeof(InputKey));
				break;
			}
		}
		action = action->next;
	}
}
void InputUnbindAll() {
	InputAction* action = g_pAction;
	while (action != NULL) {
		memset(action->keys, 0, sizeof(action->keys));
		action->keyCount = 0;
		action = action->next;
	}
}
bool InputIsPressed(const char* name) {
	InputAction* action = InputFindAction(name);
	if (action == NULL) {
		DebugLog(LogWarn, "No input action \"%s\" is defined!", name);
		return false;
	}

	if (action->symState != KEY_STATE_SIM_OFF) {
		bool result = action->symState == KEY_STATE_PRESSED;
		action->symState = KEY_STATE_SIM_OFF;
		return result;
	}

	for (int i = 0; i < action->keyCount; i++) {
		if (InputGetKeyState(action->keys[i]) == KEY_STATE_PRESSED) {
			return true;
		}
	}
	return false;
}
bool InputIsReleased(const char* name) {
	InputAction* action = InputFindAction(name);
	if (action == NULL) {
		DebugLog(LogWarn, "No input action \"%s\" is defined!", name);
		return false;
	}

	if (action->symState != KEY_STATE_SIM_OFF) {
		bool result = action->symState == KEY_STATE_RELEASED;
		action->symState = KEY_STATE_SIM_OFF;
		return result;
	}

	for (int i = 0; i < action->keyCount; i++) {
		if (InputGetKeyState(action->keys[i]) == KEY_STATE_RELEASED) {
			return true;
		}
	}
	return false;
}
bool InputIsDown(const char* name) {
	InputAction* action = InputFindAction(name);
	if (action == NULL) {
		DebugLog(LogWarn, "No input action \"%s\" is defined!", name);
		return false;
	}

	if (action->symState != KEY_STATE_SIM_OFF) {
		bool result = action->symState > KEY_STATE_NONE && action->symState < KEY_STATE_RELEASED;
		action->symState = KEY_STATE_SIM_OFF;
		return result;
	}

	for (int i = 0; i < action->keyCount; i++) {
		int state = InputGetKeyState(action->keys[i]);
		if (state > KEY_STATE_NONE && state < KEY_STATE_RELEASED) {
			return true;
		}
	}
	return false;
}
bool InputIsUp(const char* name) {
	InputAction* action = InputFindAction(name);
	if (action == NULL) {
		DebugLog(LogWarn, "No input action \"%s\" is defined!", name);
		return false;
	}

	if (action->symState != KEY_STATE_SIM_OFF) {
		bool result = action->symState == KEY_STATE_NONE || action->symState == KEY_STATE_RELEASED;
		action->symState = KEY_STATE_SIM_OFF;
		return result;
	}

	for (int i = 0; i < action->keyCount; i++) {
		int state = InputGetKeyState(action->keys[i]);
		if (state == KEY_STATE_NONE || state == KEY_STATE_RELEASED) {
			return true;
		}
	}
	return false;
}

void InputSimAction(const char* name, InputKeyState state) {
	InputAction* action = InputFindAction(name);
	if (action == NULL) {
		DebugLog(LogWarn, "Failed to simulate action \"%s\": action is undefined!", name);
		return;
	}
	action->symState = state;
}

void InputGetMousePos(int* x, int* y) {
	if (x != NULL) *x = MousePosition[0];
	if (y != NULL) *y = MousePosition[1];
}
void InputGetMousePosRelative(int* x, int* y) {
	if (x != NULL) *x = MouseMotion[0];
	if (y != NULL) *y = MouseMotion[1];
}