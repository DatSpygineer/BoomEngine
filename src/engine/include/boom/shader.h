#pragma once

#include <GL\glew.h>

#include "b_math.h"
#include "defs.h"
#include "texture.h"

#ifndef BOOM_SHADER_MAX_COUNT
	#define BOOM_SHADER_MAX_COUNT 64
#endif


BOOM_HEADER_START

typedef GLuint Shader;

#define SHADER_INVALID 0

BOOM_API Shader* ShaderCompile(const char* name, const char* vertex, const char* fragment);
BOOM_API Shader* ShaderCompileFromFile(const char* name, const char* vertexPath, const char* fragmentPath);
BOOM_API Shader* ShaderGetByName(const char* name);
BOOM_API Shader* ShaderCacheRegister(const char* name, Shader shader);
BOOM_API void ShaderUnload(Shader* shader);

BOOM_API Shader* ShaderCache(const char* path);

BOOM_API void ShaderApply(const Shader* shader);

BOOM_API void ShaderSetUniform1i(const Shader* shader, const char* name, GLint value);
BOOM_API void ShaderSetUniform2i(const Shader* shader, const char* name, GLint x, GLint y);
BOOM_API void ShaderSetUniform3i(const Shader* shader, const char* name, GLint x, GLint y, GLint z);
BOOM_API void ShaderSetUniform4i(const Shader* shader, const char* name, GLint x, GLint y, GLint z, GLint w);
BOOM_API void ShaderSetUniform1u(const Shader* shader, const char* name, GLuint value);
BOOM_API void ShaderSetUniform2u(const Shader* shader, const char* name, GLuint x, GLuint y);
BOOM_API void ShaderSetUniform3u(const Shader* shader, const char* name, GLuint x, GLuint y, GLuint z);
BOOM_API void ShaderSetUniform4u(const Shader* shader, const char* name, GLuint x, GLuint y, GLuint z, GLuint w);
BOOM_API void ShaderSetUniform1f(const Shader* shader, const char* name, float value);
BOOM_API void ShaderSetUniform2f(const Shader* shader, const char* name, float x, float y);
BOOM_API void ShaderSetUniform3f(const Shader* shader, const char* name, float x, float y, float z);
BOOM_API void ShaderSetUniform4f(const Shader* shader, const char* name, float x, float y, float z, float w);
BOOM_API void ShaderSetUniform1d(const Shader* shader, const char* name, double value);
BOOM_API void ShaderSetUniform2d(const Shader* shader, const char* name, double x, double y);
BOOM_API void ShaderSetUniform3d(const Shader* shader, const char* name, double x, double y, double z);
BOOM_API void ShaderSetUniform4d(const Shader* shader, const char* name, double x, double y, double z, double w);
BOOM_API void ShaderSetUniformMatrix4x4f(const Shader* shader, const char* name, float value[16]);
BOOM_API void ShaderSetUniformVector(const Shader* shader, const char* name, Vector vec);
BOOM_API void ShaderSetUniformMatrix(const Shader* shader, const char* name, Matrix mat);
BOOM_API void ShaderSetUniformTexture(const Shader* shader, const char* name, Texture texture);
BOOM_API void ShaderSetUniformTextureData(const Shader* shader, const char* name, const TextureData* texture);

BOOM_API void ShaderGetUniform_i(Shader* shader, const char* name, GLint* value);
BOOM_API void ShaderGetUniform_u(Shader* shader, const char* name, GLuint* value);
BOOM_API void ShaderGetUniform_f(Shader* shader, const char* name, float* value);
BOOM_API void ShaderGetUniform_d(Shader* shader, const char* name, double* value);

BOOM_HEADER_END