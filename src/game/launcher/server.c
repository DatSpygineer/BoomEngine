
#ifdef _WIN32
#include <windows.h>

static char** CommandLineToArgvA(int* argc) {
	wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), argc);
	char** argv = (char**)malloc(sizeof(char*) * *argc);
	for (int i = 0; i < *argc; i++) {
		const size_t len = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, NULL, 0, NULL, NULL);
		argv[i] = (char*)malloc(len + 1);
		WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, argv[i], len, NULL, NULL);
	}
	return argv;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hpInstance, LPSTR nCmdLine, int iCmdShow) {
	int argc;
	char** argv = CommandLineToArgvA(&argc);
#else
int main(int argc, char** argv) {
#endif
	
	return 0;
}