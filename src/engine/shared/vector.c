#include "vector.h"

#include "time.h"

bool vector_equals(const vector_t lhs, const vector_t rhs) {
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}
vector_t vector_add(const vector_t lhs, const vector_t rhs) {
	return (vector_t){ lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
}
vector_t vector_sub(const vector_t lhs, const vector_t rhs) {
	return (vector_t){ lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}
vector_t vector_mul(const vector_t lhs, const vector_t rhs) {
	return (vector_t){ lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
}
vector_t vector_mulf(const vector_t lhs, const float rhs) {
	return (vector_t){ lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
}
vector_t vector_div(const vector_t lhs, const vector_t rhs) {
	return (vector_t){ lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z };
}
vector_t vector_divf(const vector_t lhs, const float rhs) {
	return (vector_t){ lhs.x / rhs, lhs.y / rhs, lhs.z / rhs };
}
vector_t vector_normalize(const vector_t vec) {
	return vector_divf(vec, vector_length(vec));
}
vector_t vector_cross(const vector_t lhs, const vector_t rhs) {
	return (vector_t){
		lhs.x * rhs.y - lhs.y * rhs.x,
		lhs.y * rhs.z - lhs.z * rhs.y,
		lhs.z * rhs.x - lhs.x * rhs.z
	};
}
vector_t vector_lerp(const vector_t a, const vector_t b, const float t) {
	return (vector_t){
		lerp(a.x, b.x, t),
		lerp(a.y, b.y, t),
		lerp(a.z, b.z, t)
	};
}
float vector_dot(const vector_t lhs, const vector_t rhs) {
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}
float vector_length(const vector_t vec) {
	return sqrtf(vector_dot(vec, vec));
}
float vector_length_sqrt(const vector_t vec) {
	return vector_dot(vec, vec);
}
float vector4_dot(const vector4_t lhs, const vector4_t rhs) {
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w;
}
void vector_to_str(char* buffer, size_t buffer_max, vector_t vec) {
	snprintf(buffer, buffer_max, "%g %g %g", vec.x, vec.y, vec.z);
}
bool vector_from_str(const char* buffer, vector_t* vec) {
	errno = 0;
	return sscanf(buffer, "%f %f %f", &vec->x, &vec->y, &vec->z) >= 3;
}

color_t color_from_int(const uint32_t color) {
	return (color_t) {
		.r = (float)((color >> 24) & 0xFF) / 255.0f,
		.g = (float)((color >> 16) & 0xFF) / 255.0f,
		.b = (float)((color >> 8) & 0xFF) / 255.0f,
		.a = (float)(color & 0xFF) / 255.0f
	};
}
uint32_t color_to_int(const color_t color) {
	return (uint32_t)roundf(color.r * 255.0f) << 24 |
		   (uint32_t)roundf(color.g * 255.0f) << 16 |
		   (uint32_t)roundf(color.b * 255.0f) << 8  |
		   (uint32_t)roundf(color.a * 255.0f);
}
void color_to_str(char* buffer, size_t buffer_max, color_t col) {
	snprintf(buffer, buffer_max, "%d %d %d %d", (uint8_t)(col.r * 255), (uint8_t)(col.g * 255), (uint8_t)(col.b * 255), (uint8_t)(col.a * 255));
}
bool color_from_str(const char* buffer, color_t* col) {
	int r, g, b, a;
	const int res = sscanf(buffer, "%d %d %d %d", &r, &g, &b, &a);
	if (res == 3) {
		a = 255;
	} else if (res < 4) {
		return false;
	}

	*col = (color_t) {
		.r = (float)r / 255.0f,
		.g = (float)g / 255.0f,
		.b = (float)b / 255.0f,
		.a = (float)a / 255.0f
	};
	return true;
}

float lerp(const float a, const float b, const float t) {
	return a + (b - a) * t;
}
float clamp(const float x, const float min, const float max) {
	return x < min ? min : (x > max ? max : x);
}
float min(const float x, const float min) {
	return x < min ? min : x;
}
float max(const float x, const float max) {
	return x > max ? max : x;
}

float random_float() {
	return (float)rand() / (float)RAND_MAX;
}
long random_range(const long min, const long max) {
	return rand() % (max - min) + min;
}
float random_range_float(const float min, const float max) {
	return min + (max - min) * random_float();
}

matrix_t matrix_identity() {
	return (matrix_t) {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
}
matrix_t matrix_translation(const vector_t vec) {
	return (matrix_t) {
		1, 0, 0, vec.x,
		0, 1, 0, vec.y,
		0, 0, 1, vec.z,
		0, 0, 0, 1
	};
}
matrix_t matrix_scale(const vector_t scale) {
	return (matrix_t) {
		scale.x, 0, 0, 0,
		0, scale.y, 0, 0,
		0, 0, scale.z, 0,
		0, 0, 0, 1
	};
}
matrix_t matrix_rotation(const angles_t rot) {
	matrix_t matrix = matrix_identity();

	const float p = deg_to_rad(rot.pitch);
	const float y = deg_to_rad(rot.yaw);
	const float r = deg_to_rad(rot.roll);

	const float cp = cosf(p);
	const float cy = cosf(y);
	const float cr = cosf(r);
	const float sp = sinf(p);
	const float sy = sinf(y);
	const float sr = sinf(r);

	matrix.columns[0].x = cy * cr;
	matrix.columns[0].y = -cy * sr;
	matrix.columns[0].z = sy;
	matrix.columns[0].w = 0.0f;

	matrix.columns[1].x = sp * sy * cr + cp * sr;
	matrix.columns[1].y = -sp * sy * sr + cp * cr;
	matrix.columns[1].z = -sp * cy;
	matrix.columns[1].w = 0.0f;

	matrix.columns[2].x = -cp * sy * cr + sp * sr;
	matrix.columns[2].y = cp * sy * sr + sp * cr;
	matrix.columns[2].z = cp * cy;
	matrix.columns[2].w = 0.0f;

	return matrix;
}
matrix_t matrix_multiply(const matrix_t lhs, const matrix_t rhs) {
	const matrix_t t_rhs = matrix_transpose(rhs);
	return (matrix_t) {
		vector4_dot(lhs.columns[0], t_rhs.columns[0]),
		vector4_dot(lhs.columns[0], t_rhs.columns[1]),
		vector4_dot(lhs.columns[0], t_rhs.columns[2]),
		vector4_dot(lhs.columns[0], t_rhs.columns[3]),
		vector4_dot(lhs.columns[1], t_rhs.columns[0]),
		vector4_dot(lhs.columns[1], t_rhs.columns[1]),
		vector4_dot(lhs.columns[1], t_rhs.columns[2]),
		vector4_dot(lhs.columns[1], t_rhs.columns[3]),
		vector4_dot(lhs.columns[2], t_rhs.columns[0]),
		vector4_dot(lhs.columns[2], t_rhs.columns[1]),
		vector4_dot(lhs.columns[2], t_rhs.columns[2]),
		vector4_dot(lhs.columns[2], t_rhs.columns[3]),
		vector4_dot(lhs.columns[3], t_rhs.columns[0]),
		vector4_dot(lhs.columns[3], t_rhs.columns[1]),
		vector4_dot(lhs.columns[3], t_rhs.columns[2]),
		vector4_dot(lhs.columns[3], t_rhs.columns[3])
	};
}
matrix_t matrix_transpose(const matrix_t mat) {
	return (matrix_t) {
		mat.items[0], mat.items[4], mat.items[ 8], mat.items[12],
		mat.items[1], mat.items[5], mat.items[ 9], mat.items[13],
		mat.items[2], mat.items[6], mat.items[10], mat.items[14],
		mat.items[3], mat.items[7], mat.items[11], mat.items[15]
	};
}
matrix_t matrix_perspective(float fov, float aspect, float nearZ, float farZ) {
	const float t = tanf(deg_to_rad(fov * 0.5f));
	const float top = nearZ * t;
	const float right = top * aspect;
	const float fn = farZ - nearZ;

	matrix_t mat = matrix_identity();
	mat.columns[0].x = nearZ / right;
	mat.columns[1].y = nearZ / top;
	mat.columns[2].z = -(farZ + nearZ) / fn;
	mat.columns[2].w = -1.0f;
	mat.columns[3].z = -(2 * farZ * nearZ) / fn;
	mat.columns[3].w = 0.0f;
	return mat;
}