#pragma once

#include "defs.h"

BOOM_HEADER_BEGIN

BOOM_API void init_paths(const char* rootPath);
BOOM_API const char* get_root_path();

BOOM_API void get_bin_path(char* buffer);
BOOM_API void get_cfg_path(char* buffer);
BOOM_API void get_res_path(char* buffer);
BOOM_API void get_save_path(char* buffer);

BOOM_API FILE* open_cfg_file(const char* name, const char* mode);
BOOM_API FILE* open_res_file(const char* name, const char* mode);
BOOM_API FILE* open_save_file(const char* name, const char* mode);

BOOM_HEADER_END