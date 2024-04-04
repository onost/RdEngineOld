#include "Matrix3.h"

// 単位行列
static float sIdent3[3][3] =
{
	{ 1.0f,0.0f,0.0f },
	{ 0.0f,1.0f,0.0f },
	{ 0.0f,0.0f,1.0f }
};
const Matrix3 Matrix3::kIdentity(sIdent3);
