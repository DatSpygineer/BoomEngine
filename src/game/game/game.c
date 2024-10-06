#include "boom/gamedll.h"

INPUT_ACTION(test);

GAME_DLL void GameRegisterCVars() {
	InputRegisterAction(&test_action);
	InputBind("test", SDLK_RETURN, INPUT_DEVICE_KEYBOARD);

	DebugLog(LogInfo, "Test");
	DebugLog(LogWarn, "Test");
}
GAME_DLL void GameFixedUpdate(double dt) {
	if (InputIsPressed("test")) {
		DebugLog(LogInfo, "Hello world!");
	}
}

GAME_DLL const char* GameTitle() {
	return "Test";
}