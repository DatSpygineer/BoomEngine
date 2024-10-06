#include "boom/b_math.h"

float Lerp(const float min, const float max, const float a) {
	return ((max - min) * a) + min;
}
float Remap(const float x, const float fromMin, const float fromMax, const float toMin, const float toMax) {
	return Lerp(toMin, toMax, (x - fromMin) / (fromMax - fromMin));
}

bool VectorEquals(const Vector lhs, const Vector rhs) {
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}
bool VectorEqualsMargin(const Vector lhs, const Vector rhs, float margin) {
	return FloatEquals(lhs.x, rhs.x, margin);
}

Vector VectorAdd(const Vector lhs, const Vector rhs) {
	return (Vector){ lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
}
Vector VectorSub(const Vector lhs, const Vector rhs) {
	return (Vector){ lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}
Vector VectorMul(const Vector lhs, const Vector rhs) {
	return (Vector){ lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
}
Vector VectorDiv(const Vector lhs, const Vector rhs) {
	return (Vector){ lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z };
}
Vector VectorMulScalar(const Vector lhs, const float rhs) {
	return (Vector){ lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
}
Vector VectorDivScalar(const Vector lhs, const float rhs) {
	return (Vector){ lhs.x / rhs, lhs.y / rhs, lhs.z / rhs };
}

Vector VectorNeg(const Vector vec) {
	return (Vector){ -vec.x, -vec.y };
}

float VectorDot(const Vector lhs, const Vector rhs) {
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}
Vector VectorCross(const Vector lhs, const Vector rhs) {
	return (Vector){
		lhs.x * rhs.y - lhs.y * rhs.x,
		lhs.y * rhs.z - lhs.z * rhs.y,
		lhs.z * rhs.x - lhs.x * rhs.z
	};
}

float VectorLen(const Vector vec) {
	return sqrtf(VectorLenSqr(vec));
}
float VectorLenSqr(const Vector vec) {
	return VectorDot(vec, vec);
}

bool VectorToString(char* buf, size_t maxBufSize, const Vector vec) {
	errno = 0;
	snprintf(buf, maxBufSize, "%g %g %g", vec.x, vec.y, vec.z);
	return errno == 0;
}
bool VectorFromString(Vector* vec, const char* str) {
	if (vec == NULL) return false;

	float x, y, z;
	if (sscanf(str, "%f %f %f", &x, &y, &z) < 3) {
		return false;
	}
	vec->x = x;
	vec->y = y;
	vec->z = z;
	return true;
}

Rotation RotationFromVector(const Vector vec) {
	return (Rotation){ vec.x, vec.y, vec.z };
}
Vector RotationToVector(const Rotation vec) {
	return (Vector){ vec.pitch, vec.yaw, vec.roll };
}
bool RotationEquals(const Rotation lhs, const Rotation rhs) {
	return lhs.pitch == rhs.pitch && lhs.roll == rhs.roll && lhs.yaw == rhs.yaw;
}
bool RotationEqualsMargin(const Rotation lhs, const Rotation rhs, const float margin) {
	return FloatEquals(lhs.pitch, rhs.pitch, margin) && FloatEquals(lhs.roll, rhs.roll, margin) && FloatEquals(lhs.yaw, rhs.yaw, margin);
}
bool RotationToString(char* buf, const size_t maxBufSize, const Rotation rot) {
	errno = 0;
	snprintf(buf, maxBufSize, "%g %g %g", rot.pitch, rot.yaw, rot.roll);
	return errno == 0;
}
bool RotationFromString(Rotation* rot, const char* str) {
	if (rot == NULL) return false;

	float x, y, z;
	if (sscanf(str, "%f %f %f", &x, &y, &z) < 3) {
		return false;
	}
	rot->pitch = x;
	rot->yaw = y;
	rot->roll = z;
	return true;
}

const Matrix MatrixIdentity = {
	.m = {
		{ 1, 0, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 0, 0, 1 }
	}
};

Matrix MatrixTranspose(const Matrix mat) {
	return (Matrix){
		.m = {
			{ mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0] },
			{ mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1] },
			{ mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2] },
			{ mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3] }
		}
	};
}
void MatrixGetColumn(const Matrix mat, const size_t idx, float value[4]) {
	memcpy(value, mat.m[idx], sizeof(float) * 4);
}
void MatrixGetRow(const Matrix mat, const size_t idx, float value[4]) {
	value[0] = mat.m[0][idx];
	value[1] = mat.m[1][idx];
	value[2] = mat.m[2][idx];
	value[3] = mat.m[3][idx];
}

Matrix MatrixMul(const Matrix lhs, const Matrix rhs) {
	return (Matrix){
		.m = {
			{
				lhs.m[0][0] * rhs.m[0][0] + lhs.m[1][0] * rhs.m[0][1] + lhs.m[2][0] * rhs.m[0][2] + lhs.m[3][0] * rhs.m[0][3],
				lhs.m[0][0] * rhs.m[1][0] + lhs.m[1][0] * rhs.m[1][1] + lhs.m[2][0] * rhs.m[1][2] + lhs.m[3][0] * rhs.m[1][3],
				lhs.m[0][0] * rhs.m[2][0] + lhs.m[1][0] * rhs.m[2][1] + lhs.m[2][0] * rhs.m[2][2] + lhs.m[3][0] * rhs.m[2][3],
				lhs.m[0][0] * rhs.m[3][0] + lhs.m[1][0] * rhs.m[2][1] + lhs.m[2][0] * rhs.m[3][2] + lhs.m[3][0] * rhs.m[3][3]
			},
			{
				lhs.m[0][1] * rhs.m[0][0] + lhs.m[1][1] * rhs.m[0][1] + lhs.m[2][1] * rhs.m[0][2] + lhs.m[3][1] * rhs.m[0][3],
				lhs.m[0][1] * rhs.m[1][0] + lhs.m[1][1] * rhs.m[1][1] + lhs.m[2][1] * rhs.m[1][2] + lhs.m[3][1] * rhs.m[1][3],
				lhs.m[0][1] * rhs.m[2][0] + lhs.m[1][1] * rhs.m[2][1] + lhs.m[2][1] * rhs.m[2][2] + lhs.m[3][1] * rhs.m[2][3],
				lhs.m[0][1] * rhs.m[3][0] + lhs.m[1][1] * rhs.m[2][1] + lhs.m[2][1] * rhs.m[3][2] + lhs.m[3][1] * rhs.m[3][3]
			},
			{
				lhs.m[0][2] * rhs.m[0][0] + lhs.m[1][2] * rhs.m[0][1] + lhs.m[2][2] * rhs.m[0][2] + lhs.m[3][2] * rhs.m[0][3],
				lhs.m[0][2] * rhs.m[1][0] + lhs.m[1][2] * rhs.m[1][1] + lhs.m[2][2] * rhs.m[1][2] + lhs.m[3][2] * rhs.m[1][3],
				lhs.m[0][2] * rhs.m[2][0] + lhs.m[1][2] * rhs.m[2][1] + lhs.m[2][2] * rhs.m[2][2] + lhs.m[3][2] * rhs.m[2][3],
				lhs.m[0][2] * rhs.m[3][0] + lhs.m[1][2] * rhs.m[2][1] + lhs.m[2][2] * rhs.m[3][2] + lhs.m[3][2] * rhs.m[3][3]
			},
			{
				lhs.m[0][3] * rhs.m[0][0] + lhs.m[1][3] * rhs.m[0][1] + lhs.m[2][3] * rhs.m[0][2] + lhs.m[3][3] * rhs.m[0][3],
				lhs.m[0][3] * rhs.m[1][0] + lhs.m[1][3] * rhs.m[1][1] + lhs.m[2][3] * rhs.m[1][2] + lhs.m[3][3] * rhs.m[1][3],
				lhs.m[0][3] * rhs.m[2][0] + lhs.m[1][3] * rhs.m[2][1] + lhs.m[2][3] * rhs.m[2][2] + lhs.m[3][3] * rhs.m[2][3],
				lhs.m[0][3] * rhs.m[3][0] + lhs.m[1][3] * rhs.m[2][1] + lhs.m[2][3] * rhs.m[3][2] + lhs.m[3][3] * rhs.m[3][3]
			}
		}
	};
}
Matrix* MatrixMulAssign(Matrix* lhs, const Matrix rhs) {
	const float m00 = lhs->m[0][0] * rhs.m[0][0] + lhs->m[1][0] * rhs.m[0][1] + lhs->m[2][0] * rhs.m[0][2] + lhs->m[3][0] * rhs.m[0][3];
	const float m01 = lhs->m[0][0] * rhs.m[1][0] + lhs->m[1][0] * rhs.m[1][1] + lhs->m[2][0] * rhs.m[1][2] + lhs->m[3][0] * rhs.m[1][3];
	const float m02 = lhs->m[0][0] * rhs.m[2][0] + lhs->m[1][0] * rhs.m[2][1] + lhs->m[2][0] * rhs.m[2][2] + lhs->m[3][0] * rhs.m[2][3];
	const float m03 = lhs->m[0][0] * rhs.m[3][0] + lhs->m[1][0] * rhs.m[2][1] + lhs->m[2][0] * rhs.m[3][2] + lhs->m[3][0] * rhs.m[3][3];

	const float m10 = lhs->m[0][1] * rhs.m[0][0] + lhs->m[1][1] * rhs.m[0][1] + lhs->m[2][1] * rhs.m[0][2] + lhs->m[3][1] * rhs.m[0][3];
	const float m11 = lhs->m[0][1] * rhs.m[1][0] + lhs->m[1][1] * rhs.m[1][1] + lhs->m[2][1] * rhs.m[1][2] + lhs->m[3][1] * rhs.m[1][3];
	const float m12 = lhs->m[0][1] * rhs.m[2][0] + lhs->m[1][1] * rhs.m[2][1] + lhs->m[2][1] * rhs.m[2][2] + lhs->m[3][1] * rhs.m[2][3];
	const float m13 = lhs->m[0][1] * rhs.m[3][0] + lhs->m[1][1] * rhs.m[2][1] + lhs->m[2][1] * rhs.m[3][2] + lhs->m[3][1] * rhs.m[3][3];

	const float m20 = lhs->m[0][2] * rhs.m[0][0] + lhs->m[1][2] * rhs.m[0][1] + lhs->m[2][2] * rhs.m[0][2] + lhs->m[3][2] * rhs.m[0][3];
	const float m21 = lhs->m[0][2] * rhs.m[1][0] + lhs->m[1][2] * rhs.m[1][1] + lhs->m[2][2] * rhs.m[1][2] + lhs->m[3][2] * rhs.m[1][3];
	const float m22 = lhs->m[0][2] * rhs.m[2][0] + lhs->m[1][2] * rhs.m[2][1] + lhs->m[2][2] * rhs.m[2][2] + lhs->m[3][2] * rhs.m[2][3];
	const float m23 = lhs->m[0][2] * rhs.m[3][0] + lhs->m[1][2] * rhs.m[2][1] + lhs->m[2][2] * rhs.m[3][2] + lhs->m[3][2] * rhs.m[3][3];

	const float m30 = lhs->m[0][3] * rhs.m[0][0] + lhs->m[1][3] * rhs.m[0][1] + lhs->m[2][3] * rhs.m[0][2] + lhs->m[3][3] * rhs.m[0][3];
	const float m31 = lhs->m[0][3] * rhs.m[1][0] + lhs->m[1][3] * rhs.m[1][1] + lhs->m[2][3] * rhs.m[1][2] + lhs->m[3][3] * rhs.m[1][3];
	const float m32 = lhs->m[0][3] * rhs.m[2][0] + lhs->m[1][3] * rhs.m[2][1] + lhs->m[2][3] * rhs.m[2][2] + lhs->m[3][3] * rhs.m[2][3];
	const float m33 = lhs->m[0][3] * rhs.m[3][0] + lhs->m[1][3] * rhs.m[2][1] + lhs->m[2][3] * rhs.m[3][2] + lhs->m[3][3] * rhs.m[3][3];

	lhs->m[0][0] = m00;
	lhs->m[0][1] = m01;
	lhs->m[0][2] = m02;
	lhs->m[0][3] = m03;

	lhs->m[1][0] = m10;
	lhs->m[1][1] = m11;
	lhs->m[1][2] = m12;
	lhs->m[1][3] = m13;

	lhs->m[2][0] = m20;
	lhs->m[2][1] = m21;
	lhs->m[2][2] = m22;
	lhs->m[2][3] = m23;

	lhs->m[3][0] = m30;
	lhs->m[3][1] = m31;
	lhs->m[3][2] = m32;
	lhs->m[3][3] = m33;

	return lhs;
}
bool MatrixEquals(const Matrix lhs, const Matrix rhs) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (lhs.m[i][j] != rhs.m[i][j]) return false;
		}
	}
	return true;
}
bool MatrixEqualsMargin(const Matrix lhs, const Matrix rhs, const float margin) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (!FloatEquals(lhs.m[i][j], rhs.m[i][j], margin)) return false;
		}
	}
	return true;
}

Matrix MatrixCreateTranslation(const Vector vec) {
	return (Matrix){
		.m = {
			{ 1, 0, 0, vec.x },
			{ 0, 0, 0, vec.y },
			{ 0, 0, 0, vec.z },
			{ 0, 0, 0, 1 },
		}
	};
}
Matrix MatrixCreateScale(const Vector vec) {
	return (Matrix){
		.m = {
			{ vec.x, 0, 0, 0 },
			{ 0, vec.y, 0, 0 },
			{ 0, 0, vec.z, 0 },
			{ 0, 0, 0, 1 }
		}
	};
}
Matrix MatrixCreateRotation(const Rotation rot) {
	Matrix matrix = MatrixIdentity;

	const float p = DegToRad(rot.pitch);
	const float y = DegToRad(rot.yaw);
	const float r = DegToRad(rot.roll);

	const float cp = cosf(p);
	const float cy = cosf(y);
	const float cr = cosf(r);
	const float sp = sinf(p);
	const float sy = sinf(y);
	const float sr = sinf(r);

	matrix.m[0][0] = cy * cr;
	matrix.m[0][1] = -cy * sr;
	matrix.m[0][2] = sy;
	matrix.m[0][3] = 0.0f;

	matrix.m[1][0] = sp * sy * cr + cp * sr;
	matrix.m[1][1] = -sp * sy * sr + cp * cr;
	matrix.m[1][2] = -sp * cy;
	matrix.m[1][3] = 0.0f;

	matrix.m[2][0] = -cp * sy * cr + sp * sr;
	matrix.m[2][1] = cp * sy * sr + sp * cr;
	matrix.m[2][2] = cp * cy;
	matrix.m[2][3] = 0.0f;

	return matrix;
}
Matrix MatrixCreateOrtho(float left, float right, float top, float bottom, float nearZ, float farZ) {
	return (Matrix){
		.m = {
			{ 2.0f / (right - left),					0,									0,					0 },
			{ 0,										2.0f / (top - bottom),				0,					0 },
			{ 0,										0,									1,					0 },
			{ -((right + left) / (right - left)),		-((top + bottom) / (top - bottom)),	0,					1 }
		}
	};
}
Matrix MatrixCreatePerspective(float fov, float aspect, float nearZ, float farZ) {
	const float t = tanf(DegToRad(fov * 0.5f));
	const float top = nearZ * t;
	const float right = top * aspect;
	const float fn = farZ - nearZ;

	Matrix mat = MatrixIdentity;
	mat.m[0][0]  = nearZ / right;
	mat.m[1][1]  = nearZ / top;
	mat.m[2][2] = -(farZ + nearZ) / fn;
	mat.m[2][3] = -1.0f;
	mat.m[3][2] = -(2 * farZ * nearZ) / fn;
	mat.m[3][3] = 0.0f;
	return mat;
}

Matrix TransformToMatrix(Transform transform) {
	Matrix mat = MatrixMul(MatrixIdentity, MatrixCreateTranslation(transform.position));
	MatrixMulAssign(&mat, MatrixCreateTranslation(transform.origin));
	MatrixMulAssign(&mat, MatrixCreateRotation(transform.angle));
	MatrixMulAssign(&mat, MatrixCreateTranslation(VectorNeg(transform.origin)));
	MatrixMulAssign(&mat, MatrixCreateScale(transform.scale));
	return mat;
}
