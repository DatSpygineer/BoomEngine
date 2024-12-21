#include "paths.h"

char g_root_path[P_PATH_MAX] = { 0 };

void init_paths(const char* rootPath) {
	pStrCpy(g_root_path, P_PATH_MAX, rootPath);
}
const char* get_root_path() {
	if (strlen(g_root_path) == 0) {
		return pPathCurrentDir(g_root_path); // If the root path is not initialized yet, use current directory.
	}
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
	P_PATH_BUFFER(path);
	get_cfg_path(path);
	pPathAppend(path, name);
	return fopen(path, mode);
}
FILE* open_res_file(const char* name, const char* mode) {
	P_PATH_BUFFER(path);
	get_res_path(path);
	pPathAppend(path, name);
	return fopen(path, mode);
}
FILE* open_save_file(const char* name, const char* mode) {
	P_PATH_BUFFER(path);
	get_save_path(path);
	pPathAppend(path, name);
	return fopen(path, mode);
}