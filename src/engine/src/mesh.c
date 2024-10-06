#include "boom/mesh.h"
#include "boom/fsys.h"

typedef struct MeshCacheEntry {
	char name[BOOM_PATH_SHORT_MAX + 1];
	MeshData data;
} MeshCacheEntry;

static MeshCacheEntry g_MeshCache[BOOM_MESH_MAX_COUNT] = { 0 };

static Mesh MeshFindFreeId() {
	for (int32_t i = 0; i < BOOM_TEXTURE_MAX_COUNT; i++) {
		if (g_MeshCache[i].data.vbo == 0) {
			return i;
		}
	}
	return MESH_INVALID;
}