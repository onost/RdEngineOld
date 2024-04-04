#include "Matrix4.h"

// 単位行列
static float sIdent4[4][4] =
{
	{ 1.0f,0.0f,0.0f,0.0f },
	{ 0.0f,1.0f,0.0f,0.0f },
	{ 0.0f,0.0f,1.0f,0.0f },
	{ 0.0f,0.0f,0.0f,1.0f }
};
const Matrix4 Matrix4::kIdentity(sIdent4);
