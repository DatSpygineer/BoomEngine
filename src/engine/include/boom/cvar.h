#pragma once

#include "defs.h"

BOOM_HEADER_START

typedef enum CVarFlags {
	// CVar can be accessed by the server
	CVAR_FLAG_SERVER_SIDE	= 0b0001,
	// CVar can be accessed by the client
	CVAR_FLAG_CLIENT_SIDE	= 0b0010,
	// CVar requires sv_cheats to be on.
	CVAR_FLAG_CHEAT			= 0b0100,
	// CVar value is saved into user.cfg
	CVAR_FLAG_USER_DATA		= 0b1000,
} CVarFlags;

typedef struct CVar CVar;
typedef void (*CVarCommand)(int argc, const char** argv);

struct CVar {
	const CVarFlags flags;
	const char* name;
	union {
		char* value;
		CVarCommand command;
	};
	bool isCommand;

	CVar* next;
};

BOOM_API void CVarRegister(CVar* variable, const char* initialValue);
BOOM_API CVar* CVarFind(const char* name);

BOOM_API bool CVarIsCommand(const char* name);
BOOM_API bool CVarCall(const char* name, int argc, const char** argv);

BOOM_API bool CVarSetBool(const char* name, bool value);
BOOM_API bool CVarSetInt(const char* name, int value);
BOOM_API bool CVarSetFloat(const char* name, float value);
BOOM_API bool CVarSetString(const char* name, const char* value);
BOOM_API bool CVarGetBool(const char* name);
BOOM_API int CVarGetInt(const char* name);
BOOM_API float CVarGetFloat(const char* name);
BOOM_API char* CVarGetString(const char* name);

BOOM_API void CVarFreeAll();

#define CVAR(__name, __flags) CVar __name = ((CVar){ .flags = __flags, .name = #__name, .isCommand = false })
#define CVAR_CMD(__name, __ptr_fn, __flags) CVar __name = ((CVar){ .flags = __flags, .name = #__name, .command = (__ptr_fn), .isCommand = true })

#define CCOMMAND(__name, __flags) void __name##_fn(int argc, const char** argv); \
CVar __name = ((CVar){ .flags = __flags, .name = #__name, .command = &__name##_fn, .isCommand = true });\
void __name##_fn(int argc, const char** argv)

#define CCOMMAND_CUSTOM_NAME(__name, __cmd_name, __flags) void __name##_fn(int argc, const char** argv); \
CVar __name = ((CVar){ .flags = __flags, .name = __cmd_name, .command = &__name##_fn, .isCommand = true });\
void __name##_fn(int argc, const char** argv)

BOOM_HEADER_END