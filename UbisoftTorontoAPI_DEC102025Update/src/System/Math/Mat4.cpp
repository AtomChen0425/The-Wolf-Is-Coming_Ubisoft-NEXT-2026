#include "Mat4.h"
Vec4 RotateByY(float angleRad, Vec4 translation)
{
	Mat4 rotY;
	float cosAngle = std::cos(angleRad);
	float sinAngle = std::sin(angleRad);
	rotY.m[0][0] = cosAngle; rotY.m[0][1] = 0.0f;    rotY.m[0][2] = sinAngle; rotY.m[0][3] = 0.0f;
	rotY.m[1][0] = 0.0f;     rotY.m[1][1] = 1.0f;    rotY.m[1][2] = 0.0f;     rotY.m[1][3] = 0.0f;
	rotY.m[2][0] = -sinAngle; rotY.m[2][1] = 0.0f;    rotY.m[2][2] = cosAngle; rotY.m[2][3] = 0.0f;
	rotY.m[3][0] = 0.0f;     rotY.m[3][1] = 0.0f;    rotY.m[3][2] = 0.0f;     rotY.m[3][3] = 1.0f;
	return rotY * translation;
}