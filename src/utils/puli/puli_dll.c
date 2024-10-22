#include "puli_dll.h"

#include "puli_fsys.h"

dll_t pDllOpen(const char* path) {
	char plat_path[PATH_MAX] = { 0 };
	pPathParent(plat_path, path);
	char temp[128] = { 0 };
	pPathGetFilenameNoExt(temp, 128, path);
#ifdef _WIN32
	pPathAppend(plat_path, temp);
	pStrCat(plat_path, PATH_MAX, ".dll");

	return LoadLibraryA(plat_path);
#else
	pPathAppend(plat_path, "lib");
	pStrCat(plat_path, PATH_MAX, temp);
	pStrCat(plat_path, PATH_MAX, ".so");

	return dlopen(plat_path, RTLD_LAZY);
#endif
}
void* pDllSym(const dll_t* dll, const char* name) {
	if (dll == NULL) return NULL;
#ifdef _WIN32
	return (void*)GetProcAddress(*dll, name);
#else
	return dlsym(*dll, name);
#endif
}
void pDllClose(dll_t* handle) {
	if (handle != NULL) {
#ifdef _WIN32
		FreeLibrary(*handle);
#else
		dlclose(*handle);
#endif
		*handle = NULL;
	}
}