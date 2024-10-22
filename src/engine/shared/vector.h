#pragma once

#include "defs.h"

BOOM_HEADER_BEGIN

#ifdef max
	#undef max
#endif
#ifdef min
	#undef min
#endif

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

#ifndef degtorad
	#define deg_to_rad(__x) ((__x) / 180.0 * M_PI)
	#define rad_to_deg(__x) ((__x) / M_PI * 180.0)
#endif

#include <math.h>

typedef union vector_s {
	struct { float x, y, z; };
	float items[3];
} vector_t;
typedef union rotator_s {
	struct { float pitch, yaw, roll; };
	float items[3];
} angles_t;

typedef union vector4_s {
	struct { float x, y, z, w; };
	float items[4];
} vector4_t;

typedef union matrix_s {
	float items[16];
	vector4_t columns[4];
} matrix_t;

typedef struct color_s {
	float r, g, b, a;
} color_t;

BOOM_API bool vector_equals(vector_t lhs, vector_t rhs);
BOOM_API vector_t vector_add(vector_t lhs, vector_t rhs);
BOOM_API vector_t vector_sub(vector_t lhs, vector_t rhs);
BOOM_API vector_t vector_mul(vector_t lhs, vector_t rhs);
BOOM_API vector_t vector_mulf(vector_t lhs, float rhs);
BOOM_API vector_t vector_div(vector_t lhs, vector_t rhs);
BOOM_API vector_t vector_divf(vector_t lhs, float rhs);
BOOM_API vector_t vector_normalize(vector_t vec);
BOOM_API vector_t vector_cross(vector_t lhs, vector_t rhs);
BOOM_API vector_t vector_lerp(vector_t a, vector_t b, float t);
BOOM_API float vector_dot(vector_t lhs, vector_t rhs);
BOOM_API float vector_length(vector_t vec);
BOOM_API float vector_length_sqrt(vector_t vec);

BOOM_API void vector_to_str(char* buffer, size_t buffer_max, vector_t vec);
BOOM_API bool vector_from_str(const char* buffer, vector_t* vec);

BOOM_API float vector4_dot(vector4_t lhs, vector4_t rhs);

BOOM_API color_t color_from_int(uint32_t color);
BOOM_API uint32_t color_to_int(color_t color);
BOOM_API void color_to_str(char* buffer, size_t buffer_max, color_t col);
BOOM_API bool color_from_str(const char* buffer, color_t* col);

BOOM_API float lerp(float a, float b, float t);
BOOM_API float clamp(float x, float min, float max);
BOOM_API float min(float x, float min);
BOOM_API float max(float x, float max);

BOOM_API float random_float();
BOOM_API long random_range(long min, long max);
BOOM_API float random_range_float(float min, float max);

BOOM_API matrix_t matrix_identity();
BOOM_API matrix_t matrix_translation(vector_t vec);
BOOM_API matrix_t matrix_scale(vector_t scale);
BOOM_API matrix_t matrix_rotation(angles_t rot);
BOOM_API matrix_t matrix_multiply(matrix_t lhs, matrix_t rhs);
BOOM_API matrix_t matrix_transpose(matrix_t mat);
BOOM_API matrix_t matrix_perspective(float fov, float aspect, float nearZ, float farZ);

BOOM_HEADER_END