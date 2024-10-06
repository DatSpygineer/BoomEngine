#include "boom/cvar.h"
#include "boom/str.h"
#include "boom/debug.h"

#include <stdio.h>

BOOM_API CVar* g_CVars;
CVar* g_CVars = NULL;

CVAR(sv_cheats, CVAR_FLAG_SERVER_SIDE);

void CVarRegister(CVar* variable, const char* initialValue) {
	if (variable == NULL) return;

	if (CVarFind(variable->name) != NULL) {
		DebugLog(LogWarn, "CVar \"%s\" is already defined!", variable->name);
		return;
	}

	if (!variable->isCommand) {
		size_t size = initialValue == NULL ? 0 : strlen(initialValue);
		variable->value =  malloc(size + 1);
		strcpy(variable->value, initialValue);
	}

	if (g_CVars == NULL) {
		g_CVars = variable;
	} else {
		CVar* tail = g_CVars;
		while (tail->next) {
			tail = tail->next;
		}
		tail->next = variable;
	}
}

CVar* CVarFind(const char* name) {
	if (g_CVars == NULL) {
		return NULL;
	}

	CVar* current = g_CVars;
	do {
		if (StrEquals(current->name, name)) {
			return current;
		}
		current = current->next;
	} while (current != NULL);

	return NULL;
}

bool CVarIsCommand(const char* name) {
	const CVar* cvar = CVarFind(name);
	if (cvar == NULL) {
		return false;
	}
	return cvar->isCommand;
}
bool CVarCall(const char* name, int argc, const char** argv) {
	const CVar* cvar = CVarFind(name);
	if (cvar == NULL || !cvar->isCommand) {
		return false;
	}

	if ((cvar->flags & CVAR_FLAG_CHEAT) == 0 || CVarGetBool("sv_cheats")) {
		cvar->command(argc, argv);
		return true;
	}
	return false;
}

bool CVarSetBool(const char* name, bool value) {
	return CVarSetString(name, value ? "1" : "0");
}
bool CVarSetInt(const char* name, int value) {
	char buf[64] = { 0 };
	sprintf(buf, "%d", value);
	return CVarSetString(name, buf);
}
bool CVarSetFloat(const char* name, float value) {
	char buf[64] = { 0 };
	sprintf(buf, "%f", value);
	return CVarSetString(name, buf);
}
bool CVarSetString(const char* name, const char* value) {
	CVar* cvar = CVarFind(name);
	if (cvar == NULL || cvar->isCommand) {
		return false;
	}

	if ((cvar->flags & CVAR_FLAG_CHEAT) == 0 || CVarGetBool("sv_cheats")) {
		if (cvar->value != NULL) {
			free(cvar->value);
		}

		cvar->value = malloc(strlen(value) + 1);
		strcpy(cvar->value, value);
		return true;
	}
	return false;
}

bool CVarGetBool(const char* name) {
	return CVarGetInt(name) != 0;
}
int CVarGetInt(const char* name) {
	char* p = CVarGetString(name);
	if (p == NULL) {
		return 0;
	}
	return (int)strtol(p, NULL, 10); // TODO: Check for errors
}
float CVarGetFloat(const char* name) {
	char* p = CVarGetString(name);
	if (p == NULL) {
		return 0;
	}
	return strtof(p, NULL); // TODO: Check for errors
}
char* CVarGetString(const char* name) {
	CVar* cvar = CVarFind(name);
	if (cvar == NULL || cvar->isCommand) {
		return NULL;
	}
	return cvar->value;
}

void CVarFreeAll() {
	while (g_CVars != NULL) {
		if (!g_CVars->isCommand) {
			free(g_CVars->value);
		}
		g_CVars = g_CVars->next;
	}
}