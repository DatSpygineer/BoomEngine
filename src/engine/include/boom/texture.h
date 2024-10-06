#pragma once

#include "defs.h"

#include <GL/glew.h>

BOOM_HEADER_START

#ifndef BOOM_TEXTURE_MAX_COUNT
	#define BOOM_TEXTURE_MAX_COUNT 256
#endif

typedef struct TextureData {
	GLuint id;
	GLenum target;
	uint32_t width, height, frameCount;
} TextureData;

typedef int32_t Texture;

#define TEXTURE_INVALID -1

typedef struct TextureSettings {
	bool clampS, clampT;
	bool mipmaps;
	bool pointFilter;
} TextureSettings;

BOOM_API void TextureSetupCVars();

BOOM_API TextureSettings TextureGetDefaultSettings();

BOOM_API Texture TextureLoad(const char* name, const uint8_t* data, size_t size, TextureSettings settings);
BOOM_API Texture TextureLoadFile(const char* path, TextureSettings settings);
BOOM_API Texture TextureCacheRegister(const char* name, TextureData srcTexture);
BOOM_API TextureData* TextureGetDataByName(const char* name);
BOOM_API TextureData* TextureGetDataById(Texture tex);
BOOM_API void TextureUnload(Texture tex);
BOOM_API void TextureUnloadAll();

BOOM_API void TextureBind(Texture tex);

BOOM_API Texture TextureCache(const char* path);

BOOM_HEADER_END
