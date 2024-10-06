#pragma once

#include "defs.h"

BOOM_HEADER_START

#include <math.h>

#ifdef min
	#undef min
#endif
#ifdef max
	#undef max
#endif

#define PI 3.14159265359

#define Min(__x, __min) ((__x) < (__min) ? (__min) : (__x))
#define Max(__x, __max) ((__x) > (__max) ? (__max) : (__x))
#define Clamp(__x, __min, __max) ((__x) < (__min) ? (__min) : ((__x) > (__max) ? (__max) : (__x)))
#define FloatEquals(__lhs, __rhs, __margin) (fabsf((__lhs) - (__rhs)) < (__margin))
#define DegToRad(__x) ((__x) / 180.0 * PI)
#define RadToDeg(__x) ((__x) / PI * 180)

BOOM_API float Lerp(float min, float max, float a);
BOOM_API float Remap(float x, float fromMin, float fromMax, float toMin, float toMax);

typedef struct Vector {
	float x, y, z;
} Vector;

BOOM_API bool VectorEquals(Vector lhs, Vector rhs);
BOOM_API bool VectorEqualsMargin(Vector lhs, Vector rhs, float margin);

BOOM_API Vector VectorAdd(Vector lhs, Vector rhs);
BOOM_API Vector VectorSub(Vector lhs, Vector rhs);
BOOM_API Vector VectorMul(Vector lhs, Vector rhs);
BOOM_API Vector VectorDiv(Vector lhs, Vector rhs);
BOOM_API Vector VectorMulScalar(Vector lhs, float rhs);
BOOM_API Vector VectorDivScalar(Vector lhs, float rhs);
BOOM_API Vector VectorNeg(Vector vec);

BOOM_API float VectorDot(Vector lhs, Vector rhs);
BOOM_API Vector VectorCross(Vector lhs, Vector rhs);
BOOM_API float VectorLen(Vector vec);
BOOM_API float VectorLenSqr(Vector vec);

BOOM_API bool VectorToString(char* buf, size_t maxBufSize, Vector vec);
BOOM_API bool VectorFromString(Vector* vec, const char* str);

typedef struct Rotation {
	float pitch, yaw, roll;
} Rotation;

BOOM_API Rotation RotationFromVector(Vector vec);
BOOM_API Vector RotationToVector(Rotation vec);
BOOM_API bool RotationEquals(Rotation lhs, Rotation rhs);
BOOM_API bool RotationEqualsMargin(Rotation lhs, Rotation rhs, float margin);
BOOM_API bool RotationToString(char* buf, size_t maxBufSize, Rotation rot);
BOOM_API bool RotationFromString(Rotation* rot, const char* str);

typedef union Matrix {
	float m[4][4];
} Matrix;

BOOM_API extern const Matrix MatrixIdentity;

BOOM_API Matrix MatrixTranspose(Matrix mat);
BOOM_API void MatrixGetColumn(Matrix mat, size_t idx, float value[4]);
BOOM_API void MatrixGetRow(Matrix mat, size_t idx, float value[4]);

BOOM_API Matrix MatrixMul(Matrix lhs, Matrix rhs);
BOOM_API Matrix* MatrixMulAssign(Matrix* lhs, Matrix rhs);
BOOM_API bool MatrixEquals(Matrix lhs, Matrix rhs);
BOOM_API bool MatrixEqualsMargin(Matrix lhs, Matrix rhs, float margin);

BOOM_API Matrix MatrixCreateTranslation(Vector vec);
BOOM_API Matrix MatrixCreateScale(Vector vec);
BOOM_API Matrix MatrixCreateRotation(Rotation rot);
BOOM_API Matrix MatrixCreateOrtho(float left, float right, float top, float bottom, float nearZ, float farZ);
BOOM_API Matrix MatrixCreatePerspective(float fov, float aspect, float nearZ, float farZ);

typedef struct Transform {
	Vector position;
	Vector origin;
	Vector scale;
	Rotation angle;
} Transform;

BOOM_API Matrix TransformToMatrix(Transform transform);

BOOM_HEADER_END
