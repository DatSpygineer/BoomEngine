#include "paths.h"

char g_root_path[P_PATH_MAX] = { 0 };

void init_paths(const char* rootPath) {
	pStrCpy(g_root_path, P_PATH_MAX, rootPath);
}
const char* get_root_path() {
	return g_root_path;
}

void get_bin_path(char* buffer) {
	pPathAbsoluteTo(buffer, "bin", g_root_path);
}
void get_cfg_path(char* buffer) {
	pPathAbsoluteTo(buffer, "cfg", g_root_path);
}
void get_res_path(char* buffer) {
	pPathAbsoluteTo(buffer, "res", g_root_path);
}
void get_save_path(char* buffer) {
	pPathAbsoluteTo(buffer, "save", g_root_path);
}

FILE* open_cfg_file(const char* name, const char* mode) {
	char path[P_PATH_MAX] = { 0 };
	pStrCpy(path, P_PATH_MAX, g_root_path);
	pPathAppend(path, "cfg");
	pPathAppend(path, name);
	return fopen(path, mode);
}
FILE* open_res_file(const char* name, const char* mode) {
	char path[P_PATH_MAX] = { 0 };
	pStrCpy(path, P_PATH_MAX, g_root_path);
	pPathAppend(path, "res");
	pPathAppend(path, name);
	return fopen(path, mode);
}
FILE* open_save_file(const char* name, const char* mode) {
	char path[P_PATH_MAX] = { 0 };
	pStrCpy(path, P_PATH_MAX, g_root_path);
	pPathAppend(path, "save");
	pPathAppend(path, name);
	return fopen(path, mode);
}