#include "boom/texture.h"

#include <boom/debug.h>
#include <boom/fsys.h>

#include "boom/str.h"
#include "boom/cvar.h"

#include "stb_image.h"

CVAR(r_texClampS, CVAR_FLAG_CLIENT_SIDE);
CVAR(r_texClampT, CVAR_FLAG_CLIENT_SIDE);
CVAR(r_texPointFilter, CVAR_FLAG_CLIENT_SIDE);
CVAR(r_texMipmaps, CVAR_FLAG_CLIENT_SIDE);

void TextureSetupCVars() {
	CVarRegister(&r_texClampS, "0");
	CVarRegister(&r_texClampT, "0");
	CVarRegister(&r_texPointFilter, "0");
	CVarRegister(&r_texMipmaps, "1");
}

TextureSettings TextureGetDefaultSettings() {
	return (TextureSettings){
		.clampS = CVarGetBool("r_texClampS"),
		.clampT = CVarGetBool("r_texClampT"),
		.pointFilter = CVarGetBool("r_texPointFilter"),
		.mipmaps = CVarGetBool("r_texMipmaps")
	};
}

typedef enum TextureFormat {
	TEXTURE_FORMAT_L    = 0b000,
	TEXTURE_FORMAT_LA   = 0b001,
	TEXTURE_FORMAT_RGB  = 0b010,
	TEXTURE_FORMAT_RGBA = 0b011,
} TextureFormat;

typedef union TextureFlags {
	uint8_t flags;
	struct {
		uint8_t       clampS    : 1;
		uint8_t       clampT    : 1;
		uint8_t       pointFiler: 1;
		uint8_t       mipmaps   : 1;
		uint8_t       rle       : 1;
		TextureFormat format    : 3;
	};
} TextureFlags;

typedef struct TextureCacheEntry {
	char name[BOOM_PATH_SHORT_MAX + 1];
	TextureData data;
} TextureCacheEntry;

static TextureCacheEntry g_TextureCache[BOOM_TEXTURE_MAX_COUNT] = { 0 };

static Texture TextureFindFreeId() {
	for (int32_t i = 0; i < BOOM_TEXTURE_MAX_COUNT; i++) {
		if (g_TextureCache[i].data.id == 0) {
			return i;
		}
	}
	return TEXTURE_INVALID;
}

static bool TextureInitializeForOpenGL(const Texture cacheId, const uint8_t* data, int width, int height, int frameCount, TextureFormat format, const TextureSettings settings) {
	GLuint id;
	GLenum target = frameCount > 1 ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
	GLenum glFormat = GL_RGBA;
	switch (format) {
		case TEXTURE_FORMAT_L: glFormat = GL_LUMINANCE; break;
		case TEXTURE_FORMAT_LA: glFormat = GL_LUMINANCE_ALPHA; break;
		case TEXTURE_FORMAT_RGB: glFormat = GL_RGB; break;
		case TEXTURE_FORMAT_RGBA: glFormat = GL_RGBA; break;
	}
	glGenTextures(1, &id);
	if (id == 0) {
		DebugLog(LogWarn, "Failed to generate texture id, GL error: %u", glGetError());
		return false;
	}

	glBindTexture(target, id);

	glTexParameteri(target, GL_TEXTURE_WRAP_S, settings.clampS);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, settings.clampT);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, settings.pointFilter ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER,
		settings.mipmaps ?
		(settings.pointFilter ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_LINEAR) :
		(settings.pointFilter ? GL_NEAREST : GL_LINEAR)
	);

	if (target == GL_TEXTURE_2D_ARRAY) {
		glTexImage3D(target, 0, glFormat, width, height, frameCount, 0, glFormat, GL_UNSIGNED_BYTE, data);
	} else {
		glTexImage2D(target, 0, glFormat, width, height, 0, glFormat, GL_UNSIGNED_BYTE, data);
	}

	glBindTexture(target, 0);

	return true;
}

Texture TextureLoad(const char* name, const uint8_t* data, const size_t size, const TextureSettings settings) {
	const Texture tex = TextureFindFreeId();
	if (tex != TEXTURE_INVALID) {
		char identifier[5] = {
			data[0], data[1], data[2], data[3], '\0'
		};

		// Check the texture identifier
		if (StrEquals(identifier, "BTEX") || StrEquals(identifier, "XETB")) {
			const uint8_t* p = data + 4;
			uint8_t version = *p++;
			TextureFlags flags = { .flags = *p++ };
			uint16_t width = *((const uint16_t*)p);
			p += 2;
			uint16_t height = *((const uint16_t*)p);
			p += 2;
			uint16_t frames = *((const uint16_t*)p);
			p += 2;

			if (!TextureInitializeForOpenGL(tex, p, width, height, frames, flags.format, settings)) {
				return TEXTURE_INVALID;
			}
		} else {
			int w, h, c;
			uint8_t* imgData = stbi_load_from_memory(data, size, &w, &h, &c, 0);
			bool result = TextureInitializeForOpenGL(tex, imgData, w, h, 0, c, settings);
			stbi_image_free(imgData);

			if (!result) {
				return TEXTURE_INVALID;
			}
		}
	}
	return tex;
}
Texture TextureLoadFile(const char* path, const TextureSettings settings) {
	PathDecl(fullpath);
	PathAbsolute(fullpath, path);
	FILE* f = fopen(fullpath, "rb");
	if (f == NULL) {
		return TEXTURE_INVALID;
	}

	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	fseek(f, 0, SEEK_SET);

	uint8_t* data = (uint8_t*)malloc(size);
	if (data == NULL) {
		DebugLog(LogWarn, "Failed to allocate buffer for texture data!");
		fclose(f);
		return TEXTURE_INVALID;
	}

	fread(data, size, 1, f);
	fclose(f);
	const Texture tex = TextureLoad(path, data, size, settings);
	free(data);
	return tex;
}

Texture TextureCacheRegister(const char* name, const TextureData srcTexture) {
	const Texture tex = TextureFindFreeId();
	if (tex != TEXTURE_INVALID) {
		if (TextureGetDataByName(name) == NULL) {
			g_TextureCache[tex] = (TextureCacheEntry){
				.data = srcTexture
			};
			StrCpy(g_TextureCache[tex].name, BOOM_PATH_SHORT_MAX, name);
		} else {
			DebugLog(LogWarn, "Failed to cache texture \"%s\": Texture with same name already cached!", name);
			return TEXTURE_INVALID;
		}
	} else {
		DebugLog(LogWarn, "Failed to cache texture \"%s\": Maximum number of textures reached!", name);
	}
	return tex;
}
TextureData* TextureGetDataByName(const char* name) {
	for (int i = 0; i < BOOM_TEXTURE_MAX_COUNT; i++) {
		if (g_TextureCache[i].data.id != 0 && StrEquals(g_TextureCache[i].name, name)) {
			return &g_TextureCache[i].data;
		}
	}
	return NULL;
}
TextureData* TextureGetDataById(const Texture tex) {
	return tex < BOOM_TEXTURE_MAX_COUNT && g_TextureCache[tex].data.id != 0 ? &g_TextureCache[tex].data : NULL;
}
void TextureUnload(const Texture tex) {
	if (tex < BOOM_TEXTURE_MAX_COUNT && g_TextureCache[tex].data.id != 0) {
		glDeleteTextures(1, &g_TextureCache[tex].data.id);
		g_TextureCache[tex].data.id = 0;
		memset(g_TextureCache[tex].name, 0, 64);
	}
}

void TextureUnloadAll() {
	for (int i = 0; i < BOOM_TEXTURE_MAX_COUNT; i++) {
		TextureUnload(i);
	}
}

void TextureBind(Texture tex) {
	GLuint id = g_TextureCache[tex].data.id;
	if (id != 0) {
		glBindTexture(g_TextureCache[tex].data.frameCount > 0 ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D, id);
	}
}

Texture TextureCache(const char* path) {
	return TextureLoadFile(path, TextureGetDefaultSettings());
}
