#include "cvar.h"

#include <debug.h>
#include <math.h>
#include <paths.h>

CVAR(sv_cheats, CVAR_FLAG_SERVER_SIDE);

enum {
	CMD_TOKENIZER_DEFAULT,
	CMD_TOKENIZER_STRING,
	CMD_TOKENIZER_STRING_WITH_QUOTES,
};

static cvar_t* g_cvars = NULL;

cvar_t* cvar_find(const char* name) {
	if (g_cvars == NULL) return NULL;

	cvar_t* cur = g_cvars;
	while (cur != NULL) {
		if (strcmp(cur->name, name) == 0) {
			return cur;
		}
		cur = cur->next;
	}
	return NULL;
}
char* cvar_get(const char* name) {
	const cvar_t* cvar = cvar_find(name);
	if (cvar == NULL) {
		DevWarn("Cvar \"%s\" is undefined!");
		return NULL;
	}

#ifdef CLIENT_DLL
	if ((cvar->flags & CVAR_FLAG_CLIENT_SIDE) == 0) {
		return NULL; // Quit silently, this cvar is not meant for the client.
	}
#else
	if ((cvar->flags & CVAR_FLAG_SERVER_SIDE) == 0) {
		return NULL; // Quit silently, this cvar is not meant for the server.
	}
#endif

	return cvar->value;
}
bool cvar_get_bool(const char* name) {
	return cvar_get_int(name) != 0;
}
long cvar_get_int(const char* name) {
	const char* value = cvar_get(name);
	if (value == NULL) return 0;

	errno = 0;
	const long i = strtol(value, NULL, 0);
	if (errno != 0) {
		DevWarn("Parse error \"%s\" => Invalid value, expected integer", value);
		return 0;
	}
	return i;
}
float cvar_get_float(const char* name) {
	const char* value = cvar_get(name);
	if (value == NULL) return 0;

	errno = 0;
	const float f = strtof(value, NULL);
	if (errno != 0) {
		DevWarn("Parse error \"%s\" => Invalid value, expected number", value);
		return 0;
	}
	return f;
}

void cvar_set(const char* name, const char* value) {
	cvar_t* cvar = cvar_find(name);
	if (cvar == NULL) {
		DevWarn("Cvar \"%s\" is undefined!");
		return;
	}

#ifdef CLIENT_DLL
	if ((cvar->flags & CVAR_FLAG_CLIENT_SIDE) == 0) {
		return; // Quit silently, this cvar is not meant for the client.
	}
#else
	if ((cvar->flags & CVAR_FLAG_SERVER_SIDE) == 0) {
		return; // Quit silently, this cvar is not meant for the server.
	}
#endif

	if (cvar->value != NULL) {
		free(cvar->value);
	}
	cvar->value = pStrDup(value);

	if (cvar->set_callback != NULL) {
		cvar->set_callback(cvar);
	}
}
void cvar_set_bool(const char* name, bool value) {
	cvar_set_int(name, value ? 1 : 0);
}
void cvar_set_int(const char* name, int value) {
	char buffer[64];
	snprintf(buffer, 64, "%d", value);
	cvar_set(name, buffer);
}
void cvar_set_float(const char* name, float value) {
	char buffer[64];
	snprintf(buffer, 64, "%f", value);
	cvar_set(name, buffer);
}

bool cvar_is_callable(const char* name) {
	const cvar_t* cvar = cvar_find(name);
	if (cvar == NULL) return false;
	return cvar->callable;
}
bool cvar_is_clientside(const char* name) {
	const cvar_t* cvar = cvar_find(name);
	return cvar != NULL && cvar->flags & CVAR_FLAG_CLIENT_SIDE;
}
bool cvar_is_serverside(const char* name) {
	const cvar_t* cvar = cvar_find(name);
	return cvar != NULL && cvar->flags & CVAR_FLAG_CLIENT_SIDE;
}

bool cvar_register(cvar_t* cvar, const char* default_value) {
	if (cvar_find(cvar->name) != NULL) {
		return false;
	}

#ifdef CLIENT_DLL
	if ((cvar->flags & CVAR_FLAG_CLIENT_SIDE) == 0) {
		return false; // Quit silently, this cvar is not meant for the client.
	}
#else
	if ((cvar->flags & CVAR_FLAG_SERVER_SIDE) == 0) {
		return false; // Quit silently, this cvar is not meant for the server.
	}
#endif

	if (default_value != NULL) {
		cvar->value = pStrDup(default_value);
	} else {
		cvar->value = pStrDup("");
	}

	if (g_cvars == NULL) {
		g_cvars = cvar;
	} else {
		cvar_t* cur = g_cvars;
		while (cur->next != NULL) {
			cur = cur->next;
		}
		cur->next = cvar;
	}
	return true;
}

void cvar_call(const char* name, int argc, const char** argv) {
	cvar_t* cvar = cvar_find(name);
	if (cvar == NULL) {
		DevWarn("Cvar \"%s\" is undefined!");
	} else {
		if (cvar->callable) {
			cvar->command(argc, argv);
		} else {
			DevWarn("Cvar \"%s\" is not callable!");
		}
	}
}

void console_init() {
#ifdef CLIENT_DLL
	console_exec_file("user.cfg");
	console_exec_file("autoexec.cfg");
#else
	console_exec_file("server.cfg");
#endif
}
void console_shutdown() {
#ifdef CLIENT_DLL
	FILE* f = NULL;
#endif
	const cvar_t* cur = g_cvars;
	while (cur != NULL) {
#ifdef CLIENT_DLL
		if (!cur->callable && (cur->flags & (CVAR_FLAG_USER_DATA | CVAR_FLAG_CLIENT_SIDE)) != 0) {
			if (f == NULL) f = open_cfg_file("user.cfg", "w");
			fprintf(f, "\"%s\" \"%s\"\n", cur->name, cur->value);
		}
#endif
		if (!cur->callable) free(cur->value);
		cur = cur->next;
	}
#ifdef CLIENT_DLL
	if (f != NULL) fclose(f);
#endif
}

static void console_command_split(const char* cmd, char tokens[10][1024], int* token_count) {
	int token_i = 0;
	int write_i = 0;
	int parsing_state = CMD_TOKENIZER_DEFAULT;
	bool escape = false;

	while (*cmd != '\0' && token_i < 10) {
		switch (parsing_state) {
			default: case CMD_TOKENIZER_DEFAULT: {
				if (!pCharIsSpace(*cmd)) {
					if (*cmd++ == '\"') {
						parsing_state = CMD_TOKENIZER_STRING_WITH_QUOTES;
					} else {
						parsing_state = CMD_TOKENIZER_STRING;
					}
				}
			} break;
			case CMD_TOKENIZER_STRING: {
				if (!pCharIsSpace(*cmd)) {
					tokens[token_i][write_i++] = *cmd++;
				} else {
					token_i++;
					write_i = 0;
					parsing_state = CMD_TOKENIZER_DEFAULT;
				}
			} break;
			case CMD_TOKENIZER_STRING_WITH_QUOTES: {
				if (*cmd == '\"') {
					if (escape) {
						tokens[token_i][write_i++] = '\"';
						escape = false;
					}
				} else {
					if (*cmd == '\\' && !escape) {
						escape = true;
					} else {
						tokens[token_i][write_i++] = *cmd++;
						if (escape) escape = false;
					}
				}
			} break;
		}
	}

	*token_count = token_i;
}

void console_exec(const char* command) {
	char single_cmd[1024] = { 0 };
	while ((command = pStrGetTok(command, single_cmd, 1024, ';')) != NULL) {
		char tokens[10][1024] = { 0 };
		int token_i = 0;
		console_command_split(single_cmd, tokens, &token_i);
		if (cvar_is_callable(tokens[0])) {
			cvar_call(tokens[0], token_i, (const char**)tokens);
		} else {
			cvar_set(tokens[0], tokens[1]);
		}
	}
}
void console_exec_file(const char* name) {
	FILE* f = open_cfg_file(name, "r");
	if (f == NULL) {
		DevWarn("Config file \"%s\" doesn't exists!");
	} else {
		char line[1024] = { 0 };
		while (pFileReadLine(f, line, 1024)) {
			console_exec(line);
		}
		fclose(f);
	}
}
bool console_cfg_exists(const char* name) {
	char path[P_PATH_MAX] = { 0 };
	get_cfg_path(path);
	pPathAppend(path, name);
	return pPathIsDir(name);
}