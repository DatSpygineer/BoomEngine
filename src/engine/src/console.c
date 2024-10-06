#include "boom/console.h"
#include "boom/entry.h"
#include "boom/str.h"
#include "boom/fsys.h"
#include "boom/inputs.h"

extern CVar* g_CVars;

#ifndef BOOM_CONSOLE_MAX_LINES
	#define BOOM_CONSOLE_MAX_LINES 256
#endif
#ifndef BOOM_CONSOLE_MAX_LINE_LENGTH
	#define BOOM_CONSOLE_MAX_LINE_LENGTH 128
#endif

static size_t g_uConsoleLine;
static char g_ppConsoleMessages[BOOM_CONSOLE_MAX_LINES][BOOM_CONSOLE_MAX_LINE_LENGTH];

BOOM_API CCOMMAND(echo, CVAR_FLAG_CLIENT_SIDE | CVAR_FLAG_SERVER_SIDE) {
	char message[BOOM_CONSOLE_MAX_LINE_LENGTH] = { 0 };
	for (int i = 1; i < argc; i++) {
		StrAppend(message, BOOM_CONSOLE_MAX_LINE_LENGTH, argv[i]);
	}
	ConsoleAppendMessage(message, DEBUG_LEVEL_INFO);
}

void ConsoleInitialize() {
	CVarRegister(&echo, NULL);

	PathDecl(temp) = { 0 };
	PathAbsoluteTo(temp, "user.cfg", BoomCfgDirectory());
	if (PathExists(temp)) {
		ConsoleExecFile(temp);
	} else {
		PathAbsoluteTo(temp, "default.cfg", BoomCfgDirectory());
		if (PathExists(temp)) {
			ConsoleExecFile(temp);
		}
	}

	PathAbsoluteTo(temp, "autoexec.cfg", BoomCfgDirectory());
	if (PathExists(temp)) {
		ConsoleExecFile(temp);
	}
}
void ConsoleShutdown() {
	FILE* f = FileOpenCfgWrite("user.cfg");

	if (f != NULL) {
		const CVar* cvar = g_CVars;
		while (cvar != NULL) {
			if (!cvar->isCommand && (cvar->flags & CVAR_FLAG_USER_DATA) != 0) {
				fprintf(f, "%s \"%s\"\n", cvar->name, cvar->value);
			}
			cvar = cvar->next;
		}
	}

	fputs("\n// User defined keybinds:\n", f);

	InputWriteOutKeys(f);

	fflush(f);
	fclose(f);

	CVarFreeAll();
}

static int TokenizeCommand(const char* command, char tokens[256][256]) {
	int writeCount = 0;
	int tokenCount = 0;
	bool quotes = false;

	while (*command != '\0') {
		if (writeCount >= 256) {
			DebugLog(LogWarn, "Parsing command \"%s\": Command length is out of range!\n", command);
			return 0;
		}

		if (*command == '\"') {
			if (writeCount > 0) {
				writeCount = 0;
				tokenCount++;
				quotes = false;
			} else {
				quotes = true;
			}
		} else {
			if (CharIsWhiteSpace(*command) && !quotes) {
				if (writeCount > 0) {
					writeCount = 0;
					tokenCount++;
				}
			} else {
				tokens[tokenCount][writeCount++] = *command;
			}
		}

		command++;
	}
	if (writeCount != 0) {
		tokenCount++;
	}
	return tokenCount;
}

static void ConsoleExecSingle(const char* command) {
	char tokens[256][256] = { 0 };
	int tokenCount = TokenizeCommand(command, tokens);

	if (tokenCount > 0) {
		if (CVarIsCommand(tokens[0])) {
			CVarCall(tokens[0], tokenCount, (const char**)tokens);
		} else {
			if (tokenCount > 1) {
				CVarSetString(tokens[0], tokens[1]);
			}
		}
	}
}

void ConsoleExec(const char* command) {
	char temp[256] = { 0 };
	command = StrGetTok(temp, 256, command, ';');
	while (command != NULL) {
		ConsoleExecSingle(temp);
		command = StrGetTok(temp, 256, command, ';');
	}
}
void ConsoleExecFile(const char* cfgPath) {
	FILE* f = fopen(cfgPath, "r");
	char temp[512] = { 0 };
	while (FileReadLine(f, temp, 512) > 0) {
		ssize_t idx = StrFindStr(temp, "//");
		if (idx > -1) {
			temp[idx] = '\0';
		}
		ConsoleExec(temp);
	}
	fclose(f);
}

void ConsoleAppendMessage(const char* message, DebugLevel level) {
	if (g_uConsoleLine == BOOM_CONSOLE_MAX_LINES) {
		for (int i = 0; i < BOOM_CONSOLE_MAX_LINES - 1; i++) {
			StrCpy(g_ppConsoleMessages[i], BOOM_CONSOLE_MAX_LINE_LENGTH, g_ppConsoleMessages[i + 1]);
		}
	} else {
		StrCpy(g_ppConsoleMessages[g_uConsoleLine++], BOOM_CONSOLE_MAX_LINE_LENGTH, message);
	}
}
void ConsoleClearMessages() {
	for (int i = 0; i < BOOM_CONSOLE_MAX_LINES - 1; i++) {
		memset(g_ppConsoleMessages[i], 0, BOOM_CONSOLE_MAX_LINE_LENGTH);
	}
	g_uConsoleLine = 0;
}

char** ConsoleGetLines() {
	return (char**)g_ppConsoleMessages;
}