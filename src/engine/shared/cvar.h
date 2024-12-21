#pragma once

#include "defs.h"

BOOM_HEADER_BEGIN

#include "vector.h"

typedef enum {
	CVAR_FLAG_NONE        = 0b0000,
	CVAR_FLAG_CLIENT_SIDE = 0b0001,
	CVAR_FLAG_SERVER_SIDE = 0b0010,
	CVAR_FLAG_USER_DATA   = 0b0100,
	CVAR_FLAG_IS_CHEAT    = 0b1000
} cvarflags_t;

typedef struct cvar_s cvar_t;

typedef void (*ccommand_t)(int argc, const char** argv);
typedef void (*cvar_set_callback_t)(cvar_t* self);

struct cvar_s {
	const char* name;
	union {
		char* value;
		ccommand_t command;
	};
	cvarflags_t flags;
	bool callable;
	cvar_t* next;
	cvar_set_callback_t set_callback;
};

#define CVAR(__name, __flags) cvar_t __name = { .name = #__name, .flags = __flags, .value = NULL, .callable = false, .next = NULL, .set_callback = NULL }
#define CVAR_WITH_CALLBACK(__name, __flags, __callback) cvar_t __name = { .name = #__name, .flags = __flags, .value = NULL, .callable = false, .next = NULL, .set_callback = __callback }
#define CCOMMAND(__name, __flags) void __cvar_##__name(int argc, const char** argv); CVAR(__name, __flags); void __cvar_##__name(int argc, const char** argv)

extern cvar_t sv_cheats;

BOOM_API cvar_t* cvar_find(const char* name);

#define cvar_to_str(cvar_p) ((cvar_p)->value)
BOOM_API bool cvar_to_bool(cvar_t* cvar);
BOOM_API long cvar_to_int(cvar_t* cvar);
BOOM_API float cvar_to_float(cvar_t* cvar);
BOOM_API vector_t cvar_to_vector(cvar_t* cvar);
BOOM_API color_t cvar_to_color(cvar_t* cvar);

BOOM_API void str_to_cvar(cvar_t* cvar, const char* str);
BOOM_API void bool_to_cvar(cvar_t* cvar, bool value);
BOOM_API void int_to_cvar(cvar_t* cvar, long value);
BOOM_API void float_to_cvar(cvar_t* cvar, float value);
BOOM_API void vector_to_cvar(cvar_t* cvar, vector_t value);
BOOM_API void color_to_cvar(cvar_t* cvar, color_t value);

BOOM_API char* cvar_get(const char* name);
BOOM_API bool cvar_get_bool(const char* name);
BOOM_API long cvar_get_int(const char* name);
BOOM_API float cvar_get_float(const char* name);
BOOM_API long cvar_get_int(const char* name);
BOOM_API float cvar_get_float(const char* name);
BOOM_API vector_t cvar_get_vector(const char* name);
BOOM_API color_t cvar_get_color(const char* name);

BOOM_API void cvar_set(const char* name, const char* value);
BOOM_API void cvar_set_bool(const char* name, bool value);
BOOM_API void cvar_set_int(const char* name, long value);
BOOM_API void cvar_set_float(const char* name, float value);
BOOM_API void cvar_set_vector(const char* name, vector_t value);
BOOM_API void cvar_set_color(const char* name, color_t value);

BOOM_API bool cvar_is_callable(const char* name);
BOOM_API bool cvar_is_clientside(const char* name);
BOOM_API bool cvar_is_serverside(const char* name);

BOOM_API bool cvar_register(cvar_t* cvar, const char* default_value);

BOOM_API void cvar_call(const char* name, int argc, const char** argv);

BOOM_API void console_init();
BOOM_API void console_shutdown();
BOOM_API void console_exec(const char* command);
BOOM_API void console_exec_file(const char* name);
BOOM_API bool console_cfg_exists(const char* name);

BOOM_HEADER_END