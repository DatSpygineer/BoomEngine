#pragma once

#include <GL/glew.h>

#include "shader.h"
#include "b_math.h"
#include "texture.h"

#ifndef BOOM_MESH_MAX_TEXTURES
	#define BOOM_MESH_MAX_TEXTURES 8
#endif

#if BOOM_MESH_MAX_TEXTURES > UINT8_MAX
	#error Macro value "BOOM_MESH_MAX_TEXTURES" must be less then 256!
#endif

#ifndef BOOM_MESH_MAX_COUNT
	#define BOOM_MESH_MAX_COUNT 128
#endif

typedef struct MeshData {
	GLuint vao, vbo, ebo;
	const Shader* shader;
	const Texture* texture[BOOM_MESH_MAX_TEXTURES];
} MeshData;

typedef int32_t Mesh;

#define MESH_INVALID -1

BOOM_API Mesh MeshLoad(const char* name, const uint8_t* data, size_t size);
BOOM_API Mesh MeshLoadFile(const char* path);
BOOM_API Mesh MeshCreateRaw(const char* name, const Shader* shader, const float* data, size_t vertexCount, size_t textureCount, const Texture** textures);
BOOM_API Mesh MeshCacheRegister(const char* name, MeshData meshData);
BOOM_API void MeshUnload(Mesh mesh);
BOOM_API void MeshUnloadData(MeshData* meshData);
BOOM_API void MeshUnloadAll();

BOOM_API MeshData* MeshGetDataById(Mesh mesh);
BOOM_API MeshData* MeshGetDataByName(const char* name);
BOOM_API bool MeshDataIsValid(const MeshData* data);

BOOM_API void MeshDraw(Mesh mesh, Transform transform);

BOOM_API Mesh MeshCache(const char* path);