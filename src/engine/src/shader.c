#include "boom/shader.h"

#include "boom/debug.h"
#include "boom/str.h"
#include "boom/fsys.h"

typedef struct ShaderCacheEntry {
	char name[BOOM_PATH_SHORT_MAX + 1];
	Shader shader;
} ShaderCacheEntry;

static ShaderCacheEntry g_CachedShaders[BOOM_SHADER_MAX_COUNT] = { 0 };

static ssize_t ShaderFindFreeId() {
	for (ssize_t i = 0; i < BOOM_SHADER_MAX_COUNT; i++) {
		if (g_CachedShaders[i].shader == SHADER_INVALID) {
			return i;
		}
	}
	return -1;
}

Shader* ShaderCompile(const char* name, const char* vertex, const char* fragment) {
	GLuint vid = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vid, 1, &vertex, NULL);
	glCompileShader(vid);
	GLint success = 0;
	glGetShaderiv(vid, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		char info[128];
		glGetShaderInfoLog(vid, 128, &success, info);
		DebugLog(LogWarn, "Failed to compile vertex shader \"%s\": %s", name, info);
		return NULL;
	}

	GLuint fid = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fid, 1, &fragment, NULL);
	glCompileShader(fid);
	glGetShaderiv(fid, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		char info[128];
		glGetShaderInfoLog(fid, 128, &success, info);
		glDeleteShader(vid);
		DebugLog(LogWarn, "Failed to compile fragment shader \"%s\": %s", name, info);
		return NULL;
	}

	GLuint id = glCreateProgram();
	if (id == 0) {
		glDeleteShader(vid);
		glDeleteShader(fid);
		DebugLog(LogWarn, "Failed to create shader program for \"%s\"", name);
		return NULL;
	}

	glAttachShader(id, vid);
	glAttachShader(id, fid);
	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		char info[128];
		glGetProgramInfoLog(id, 128, &success, info);
		glDetachShader(id, vid);
		glDetachShader(id, fid);
		glDeleteShader(vid);
		glDeleteShader(fid);
		glDeleteProgram(id);

		DebugLog(LogWarn, "Failed to link shader program \"%s\": %s", name, info);
		return NULL;
	}

	glDetachShader(id, vid);
	glDetachShader(id, fid);
	glDeleteShader(vid);
	glDeleteShader(fid);

	Shader* result = ShaderCacheRegister(name, id);
	if (result == NULL) {
		glDeleteProgram(id);
		return NULL;
	}
	return result;
}

Shader* ShaderCompileFromFile(const char* name, const char* vertexPath, const char* fragmentPath) {
	FILE* fVert = FileOpenRead(vertexPath);
	FILE* fFrag = FileOpenRead(fragmentPath);

	char* vert = (char*)FileAllocAndReadAll(fVert);
	char* frag = (char*)FileAllocAndReadAll(fFrag);

	Shader* result = NULL;
	DEFER_MULTI(free, 2, vert, frag) {
		result = ShaderCompile(name, vert, frag);
	}
	return result;
}

Shader* ShaderGetByName(const char* name) {
	for (ssize_t i = 0; i < BOOM_SHADER_MAX_COUNT; i++) {
		if (StrEquals(g_CachedShaders[i].name, name)) {
			return &g_CachedShaders[i].shader;
		}
	}
	return NULL;
}

Shader* ShaderCacheRegister(const char* name, const Shader shader) {
	ssize_t idx = ShaderFindFreeId();
	if (idx == -1) {
		DebugLog(LogWarn, "Failed to cache shader \"%s\": Maximum number of shaders has been reached!", name);
		return NULL;
	}
	StrCpy(g_CachedShaders[idx].name, BOOM_PATH_SHORT_MAX, name);
	g_CachedShaders[idx].shader = shader;
	return &g_CachedShaders[idx].shader;
}

void ShaderUnload(Shader* shader) {
	for (ssize_t i = 0; i < BOOM_SHADER_MAX_COUNT; i++) {
		if (g_CachedShaders[i].shader == *shader) {
			glDeleteProgram(g_CachedShaders[i].shader);
			g_CachedShaders[i].shader = SHADER_INVALID;
			memset(&g_CachedShaders[i].name, 0, BOOM_PATH_SHORT_MAX);
			*shader = SHADER_INVALID;
			return;
		}
	}
}

Shader* ShaderCache(const char* path) {
	PathDecl(vertexPath);
	StrCpy(vertexPath, BOOM_PATH_MAX, path);
	StrAppend(vertexPath, BOOM_PATH_MAX, ".vsh");
	PathDecl(fragmentPath);
	StrCpy(fragmentPath, BOOM_PATH_MAX, path);
	StrAppend(vertexPath, BOOM_PATH_MAX, ".fsh");

	return ShaderCompileFromFile(path, vertexPath, fragmentPath);
}

void ShaderApply(const Shader* shader) {
	if (shader != NULL && *shader != SHADER_INVALID) {
		glUseProgram(*shader);
	}
}

void ShaderSetUniform1i(const Shader* shader, const char* name, GLint value) {
	if (shader == NULL) return;
	const GLint loc = glGetUniformLocation(*shader, name);
	if (loc == -1) {
		return;
	}
	glUniform1i(loc, value);
}
void ShaderSetUniform2i(const Shader* shader, const char* name, GLint x, GLint y) {
	if (shader == NULL) return;
	const GLint loc = glGetUniformLocation(*shader, name);
	if (loc == -1) {
		return;
	}
	glUniform2i(loc, x, y);
}
void ShaderSetUniform3i(const Shader* shader, const char* name, GLint x, GLint y, GLint z) {
	if (shader == NULL) return;
	const GLint loc = glGetUniformLocation(*shader, name);
	if (loc == -1) {
		return;
	}
	glUniform3i(loc, x, y, z);
}
void ShaderSetUniform4i(const Shader* shader, const char* name, GLint x, GLint y, GLint z, GLint w) {
	if (shader == NULL) return;
	const GLint loc = glGetUniformLocation(*shader, name);
	if (loc == -1) {
		return;
	}
	glUniform4i(loc, x, y, z, w);
}
void ShaderSetUniform1u(const Shader* shader, const char* name, GLuint value) {
	if (shader == NULL) return;
	const GLint loc = glGetUniformLocation(*shader, name);
	if (loc == -1) {
		return;
	}
	glUniform1ui(loc, value);
}
void ShaderSetUniform2u(const Shader* shader, const char* name, GLuint x, GLuint y) {
	if (shader == NULL) return;
	const GLint loc = glGetUniformLocation(*shader, name);
	if (loc == -1) {
		return;
	}
	glUniform2ui(loc, x, y);
}
void ShaderSetUniform3u(const Shader* shader, const char* name, GLuint x, GLuint y, GLuint z) {
	if (shader == NULL) return;
	const GLint loc = glGetUniformLocation(*shader, name);
	if (loc == -1) {
		return;
	}
	glUniform3ui(loc, x, y, z);
}
void ShaderSetUniform4u(const Shader* shader, const char* name, GLuint x, GLuint y, GLuint z, GLuint w) {
	if (shader == NULL) return;
	const GLint loc = glGetUniformLocation(*shader, name);
	if (loc == -1) {
		return;
	}
	glUniform4ui(loc, x, y, z, w);
}
void ShaderSetUniform1f(const Shader* shader, const char* name, float value) {
	if (shader == NULL) return;
	const GLint loc = glGetUniformLocation(*shader, name);
	if (loc == -1) {
		return;
	}
	glUniform1f(loc, value);
}
void ShaderSetUniform2f(const Shader* shader, const char* name, float x, float y) {
	if (shader == NULL) return;
	const GLint loc = glGetUniformLocation(*shader, name);
	if (loc == -1) {
		return;
	}
	glUniform2f(loc, x, y);
}
void ShaderSetUniform3f(const Shader* shader, const char* name, float x, float y, float z) {
	if (shader == NULL) return;
	const GLint loc = glGetUniformLocation(*shader, name);
	if (loc == -1) {
		return;
	}
	glUniform3f(loc, x, y, z);
}
void ShaderSetUniform4f(const Shader* shader, const char* name, float x, float y, float z, float w) {
	if (shader == NULL) return;
	const GLint loc = glGetUniformLocation(*shader, name);
	if (loc == -1) {
		return;
	}
	glUniform4f(loc, x, y, z, w);
}
void ShaderSetUniform1d(const Shader* shader, const char* name, double value) {
	if (shader == NULL) return;
	const GLint loc = glGetUniformLocation(*shader, name);
	if (loc == -1) {
		return;
	}
	glUniform1d(loc, value);
}
void ShaderSetUniform2d(const Shader* shader, const char* name, double x, double y) {
	if (shader == NULL) return;
	const GLint loc = glGetUniformLocation(*shader, name);
	if (loc == -1) {
		return;
	}
	glUniform2d(loc, x, y);
}
void ShaderSetUniform3d(const Shader* shader, const char* name, double x, double y, double z) {
	if (shader == NULL) return;
	const GLint loc = glGetUniformLocation(*shader, name);
	if (loc == -1) {
		return;
	}
	glUniform3d(loc, x, y, z);
}
void ShaderSetUniform4d(const Shader* shader, const char* name, double x, double y, double z, double w) {
	if (shader == NULL) return;
	const GLint loc = glGetUniformLocation(*shader, name);
	if (loc == -1) {
		return;
	}
	glUniform4d(loc, x, y, z, w);
}
void ShaderSetUniformMatrix4x4f(const Shader* shader, const char* name, float value[16]) {
	if (shader == NULL) return;
	const GLint loc = glGetUniformLocation(*shader, name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, value);
}
void ShaderSetUniformVector(const Shader* shader, const char* name, Vector vec) {
	ShaderSetUniform3f(shader, name, vec.x, vec.y, vec.z);
}
void ShaderSetUniformMatrix(const Shader* shader, const char* name, Matrix mat) {
	float value[16];
	memcpy(value, mat.m[0], 4 * sizeof(float));
	memcpy(value + 4, mat.m[1], 4 * sizeof(float));
	memcpy(value + 8, mat.m[2], 4 * sizeof(float));
	memcpy(value + 12, mat.m[3], 4 * sizeof(float));
	ShaderSetUniformMatrix4x4f(shader, name, value);
}

void ShaderSetUniformTexture(const Shader* shader, const char* name, Texture texture) {
	ShaderSetUniformTextureData(shader, name, TextureGetDataById(texture));
}

void ShaderSetUniformTextureData(const Shader* shader, const char* name, const TextureData* texture) {
	if (texture != NULL) {
		ShaderSetUniform1u(shader, name, texture->id);
	}
}
