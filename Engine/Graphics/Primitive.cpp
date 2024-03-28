#include "Primitive.h"
#include "Camera.h"
#include "Core/GraphicsCommon.h"
#include "Matrix3.h"
#include "Renderer.h"
#include "Window.h"

void Primitive::Initialize(Renderer* renderer)
{
	MY_ASSERT(renderer);
	mRenderer = renderer;

	// ルートシグネチャ
	//mRootSignature.Initialize(1, 1);
	mRootSignature = std::make_unique<RootSignature>(1, 1);
	mRootSignature->RootParameters(0).InitConstant(0);
	mRootSignature->Samplers(0) = GraphicsCommon::gSamplerLinearWrap;
	mRootSignature->Create();

	// シェーダ
	Shader* vs = renderer->GetVs("Assets/Shader/Primitive/PrimVs.hlsl");
	Shader* ps = renderer->GetPs("Assets/Shader/Primitive/PrimPs.hlsl");
	// パイプラインステート
	mLinePso2.SetRootSignature(mRootSignature.get());
	mLinePso2.SetVertexShader(vs);
	mLinePso2.SetPixelShader(ps);
	mLinePso2.SetBlendState(GraphicsCommon::gBlendNormal);
	mLinePso2.SetRasterizerState(GraphicsCommon::gRasterizerCullModeNone);
	mLinePso2.SetDepthStencilState(GraphicsCommon::gDepthDisable);
	D3D12_INPUT_ELEMENT_DESC inputLayouts[2] = {};
	inputLayouts[0].SemanticName = "POSITION";
	inputLayouts[0].SemanticIndex = 0;
	inputLayouts[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayouts[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputLayouts[1].SemanticName = "COLOR";
	inputLayouts[1].SemanticIndex = 0;
	inputLayouts[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputLayouts[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	mLinePso2.SetInputLayout(_countof(inputLayouts), inputLayouts);
	mLinePso2.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
	mLinePso2.Create();
	mLinePso3 = mLinePso2;
	//mLinePso3.SetDepthStencilDesc(GraphicsCommon::gDepthEnable);
	mLinePso3.Create();
	mPrimPso2 = mLinePso2;
	mPrimPso2.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	mPrimPso2.Create();
	mPrimPso3 = mLinePso3;
	mPrimPso3.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	mPrimPso3.Create();

	mGridPso = mLinePso3;
	mGridPso.SetDepthStencilState(GraphicsCommon::gDepthEnable);
	mGridPso.Create();

	// 頂点バッファ
	mVBuff = std::make_unique<VertexBuffer>();
	mVBuff->Create(sizeof(Vertex) * kMaxVertex, sizeof(Vertex));
	// 定数バッファ
	mCBuff2 = std::make_unique<ConstantBuffer>();
	Matrix4 initData = Matrix4::CreateOrthographic(
		0.0f, 0.0f, float(Window::kWidth), float(Window::kHeight), 0.0f, 100.0f);
	mCBuff2->Create(sizeof(Matrix4), &initData);
	mCBuff3 = std::make_unique<ConstantBuffer>();
	mCBuff3->Create(sizeof(Matrix4));
}

// レンダリング前
void Primitive::PreRendering(ID3D12GraphicsCommandList* cmdList)
{
	MY_ASSERT(cmdList);
	mCmdList = cmdList;
	mIndex = 0;
	// カメラ
	Camera* camera = mRenderer->GetCurrCamera();
	if (camera)
	{
		auto vp = camera->GetViewProj();
		mCBuff3->Copy(vp);
	}
}

// レンダリング後
void Primitive::PostRendering()
{
	mCmdList = nullptr;
}

// Line 2D
void Primitive::DrawLine2(
	const Vector2& p1, const Vector2& p2, const Color& col)
{
	std::vector<Vertex> v(2);
	v[0] = { ToVector3(p1),col };
	v[1] = { ToVector3(p2),col };
	LineList2(v);
}

// Line 3D
void Primitive::DrawLine3(
	const Vector3& p1, const Vector3& p2, const Color& col)
{
	std::vector<Vertex> v(2);
	v[0] = { p1,col };
	v[1] = { p2,col };
	LineList3(v);
}

// Triangle
void Primitive::DrawTriangle(
	const Vector2& p1, const Vector2& p2, const Vector2& p3, const Color& col, bool isFill)
{
	if (isFill)
	{
		std::vector<Vertex> v(3);
		v[0] = { ToVector3(p1),col };
		v[1] = { ToVector3(p2),col };
		v[2] = { ToVector3(p3),col };
		TriList2(v);
	}
	else
	{
		// Line
		std::vector<Vertex> v(6);
		v[0] = { ToVector3(p1),col };
		v[1] = { ToVector3(p2),col };
		v[2] = v[1];
		v[3] = { ToVector3(p3),col };
		v[4] = v[3];
		v[5] = v[0];
		LineList2(v);
	}
}

// Box 2D
void Primitive::DrawBox2(
	const Vector2& pos, const Vector2& size, const Color& col, bool isFill)
{
	DrawBox2(pos, size, 0.0f, col, isFill);
}

// Box 2D
void Primitive::DrawBox2(
	const Vector2& pos, const Vector2& size, float r, const Color& col, bool isFill)
{
	Vector2 pivot = Vector2::kZero;
	Matrix3 m =
		Matrix3::CreateScale(size) *
		Matrix3::CreateRotate(r) *
		Matrix3::CreateTranslate(pos);
	Vector2 p[4] =
	{
		Vector2(0.0f - pivot.x,0.0f - pivot.y) * m,
		Vector2(1.0f + pivot.x,0.0f - pivot.y) * m,
		Vector2(1.0f + pivot.x,1.0f + pivot.y) * m,
		Vector2(0.0f - pivot.x,1.0f + pivot.y) * m
	};
	if (isFill)
	{
		std::vector<Vertex> v(6);
		v[0] = { ToVector3(p[0]),col };
		v[1] = { ToVector3(p[1]),col };
		v[2] = { ToVector3(p[3]),col };
		v[3] = v[1];
		v[4] = { ToVector3(p[2]),col };
		v[5] = v[2];
		TriList2(v);
	}
	else
	{
		// Line
		std::vector<Vertex> v(8);
		v[0] = { ToVector3(p[0]),col };
		v[1] = { ToVector3(p[1]),col };
		v[2] = v[1];
		v[3] = { ToVector3(p[2]),col };
		v[4] = v[3];
		v[5] = { ToVector3(p[3]),col };
		v[6] = v[5];
		v[7] = v[0];
		LineList2(v);
	}
}

// Box 3D
void Primitive::DrawBox3(
	const Vector3& pos, const Vector3& size, const Color& col)
{
	Matrix4 m =
		Matrix4::CreateScale(size) *
		//Matrix4::CreateRotate() *
		Matrix4::CreateTranslate(pos);
	Vector3 p[8] =
	{
		Vector3(-0.5f, +0.5f, +0.5f) * m,
		Vector3(-0.5f, -0.5f, +0.5f) * m,
		Vector3(-0.5f, -0.5f, -0.5f) * m,
		Vector3(-0.5f, +0.5f, -0.5f) * m,
		Vector3(+0.5f, +0.5f, +0.5f) * m,
		Vector3(+0.5f, -0.5f, +0.5f) * m,
		Vector3(+0.5f, -0.5f, -0.5f) * m,
		Vector3(+0.5f, +0.5f, -0.5f) * m
	};
	std::vector<Vertex> v(24);
	v[0] = { Vector3(p[0]),col };
	v[1] = { Vector3(p[1]),col };
	v[2] = v[1];
	v[3] = { Vector3(p[2]),col };
	v[4] = v[3];
	v[5] = { Vector3(p[3]),col };
	v[6] = v[5];
	v[7] = v[0];
	v[8] = { Vector3(p[4]),col };
	v[9] = { Vector3(p[5]),col };
	v[10] = v[9];
	v[11] = { Vector3(p[6]),col };
	v[12] = v[11];
	v[13] = { Vector3(p[7]),col };
	v[14] = v[13];
	v[15] = v[8];
	v[16] = v[0];
	v[17] = v[8];
	v[18] = v[1];
	v[19] = v[9];
	v[20] = v[3];
	v[21] = v[11];
	v[22] = v[5];
	v[23] = v[13];
	LineList3(v);
}

/*
// Box 3D
void Primitive::DrawBox3(
	const Vector3& min, const Vector3& max, const Color& col)
{
	Vector3 p[8] =
	{
		Vector3(min.x, max.y, max.z),
		Vector3(min.x, min.y, max.z),
		Vector3(min.x, min.y, min.z),
		Vector3(min.x, max.y, min.z),
		Vector3(max.x, max.y, max.z),
		Vector3(max.x, min.y, max.z),
		Vector3(max.x, min.y, min.z),
		Vector3(max.x, max.y, min.z)
	};
	std::vector<Vertex> v(24);
	v[0] = { Vector3(p[0]),col };
	v[1] = { Vector3(p[1]),col };
	v[2] = v[1];
	v[3] = { Vector3(p[2]),col };
	v[4] = v[3];
	v[5] = { Vector3(p[3]),col };
	v[6] = v[5];
	v[7] = v[0];
	v[8] = { Vector3(p[4]),col };
	v[9] = { Vector3(p[5]),col };
	v[10] = v[9];
	v[11] = { Vector3(p[6]),col };
	v[12] = v[11];
	v[13] = { Vector3(p[7]),col };
	v[14] = v[13];
	v[15] = v[8];
	v[16] = v[0];
	v[17] = v[8];
	v[18] = v[1];
	v[19] = v[9];
	v[20] = v[3];
	v[21] = v[11];
	v[22] = v[5];
	v[23] = v[13];
	LineList3(v);
}
*/

// AABB
void Primitive::DrawBox3(const AABB& aabb, const Color& col)
{
	DrawBox3(aabb.mMin, aabb.mMax, col);
}

// OBB
void Primitive::DrawBox3(const OBB& obb, const Color& col)
{
	Matrix4 r;
	r.m[0][0] = obb.mAxis[0].x;
	r.m[0][1] = obb.mAxis[0].y;
	r.m[0][2] = obb.mAxis[0].z;
	r.m[1][0] = obb.mAxis[1].x;
	r.m[1][1] = obb.mAxis[1].y;
	r.m[1][2] = obb.mAxis[1].z;
	r.m[2][0] = obb.mAxis[2].x;
	r.m[2][1] = obb.mAxis[2].y;
	r.m[2][2] = obb.mAxis[2].z;
	Matrix4 m =
		Matrix4::CreateScale(obb.mSize * 2.0f) *
		r *
		Matrix4::CreateTranslate(obb.mCenter);
	Vector3 p[8] =
	{
		Vector3(-0.5f, +0.5f, +0.5f) * m,
		Vector3(-0.5f, -0.5f, +0.5f) * m,
		Vector3(-0.5f, -0.5f, -0.5f) * m,
		Vector3(-0.5f, +0.5f, -0.5f) * m,
		Vector3(+0.5f, +0.5f, +0.5f) * m,
		Vector3(+0.5f, -0.5f, +0.5f) * m,
		Vector3(+0.5f, -0.5f, -0.5f) * m,
		Vector3(+0.5f, +0.5f, -0.5f) * m
	};
	std::vector<Vertex> v(24);
	v[0] = { Vector3(p[0]),col };
	v[1] = { Vector3(p[1]),col };
	v[2] = v[1];
	v[3] = { Vector3(p[2]),col };
	v[4] = v[3];
	v[5] = { Vector3(p[3]),col };
	v[6] = v[5];
	v[7] = v[0];
	v[8] = { Vector3(p[4]),col };
	v[9] = { Vector3(p[5]),col };
	v[10] = v[9];
	v[11] = { Vector3(p[6]),col };
	v[12] = v[11];
	v[13] = { Vector3(p[7]),col };
	v[14] = v[13];
	v[15] = v[8];
	v[16] = v[0];
	v[17] = v[8];
	v[18] = v[1];
	v[19] = v[9];
	v[20] = v[3];
	v[21] = v[11];
	v[22] = v[5];
	v[23] = v[13];
	LineList3(v);
}

// Circle
void Primitive::DrawCircle(
	const Vector2& pos, float rad, const Color& col, bool isFill)
{
	const uint32_t kDiv = 32;
	std::vector<Vector3> p(kDiv);
	for (uint32_t i = 0; i < kDiv; ++i)
	{
		float r = MyMath::k2Pi / kDiv * i;
		p[i] = Vector3(
			pos.x + cosf(r) * rad,
			pos.y + sinf(r) * rad,
			0.0f);
	}
	if (isFill)
	{
		std::vector<Vertex> v(kDiv * 3);
		for (uint32_t i = 0; i < kDiv; ++i)
		{
			v[i * 3 + 0] = { p[i],col };
			v[i * 3 + 1] = { p[(i + 1) % kDiv],col };
			v[i * 3 + 2] = { ToVector3(pos),col };
		}
		TriList2(v);
	}
	else
	{
		std::vector<Vertex> v(kDiv * 2);
		for (uint32_t i = 0; i < kDiv; ++i)
		{
			v[i * 2 + 0] = { p[i],col };
			v[i * 2 + 1] = { p[(i + 1) % kDiv],col };
		}
		LineList2(v);
	}
}

// Sphere
void Primitive::DrawSphere(
	const Vector3& pos, float rad, const Color& col)
{
	const uint32_t kDiv = 32;
	std::vector<Vector3> p(kDiv * 3);
	for (uint32_t i = 0; i < kDiv; ++i)
	{
		float r = MyMath::k2Pi / kDiv * i;
		float c = cosf(r) * rad;
		float s = sinf(r) * rad;
		p[kDiv * 0 + i] = pos + Vector3(c, s, 0.0f);
		p[kDiv * 1 + i] = pos + Vector3(0.0f, c, s);
		p[kDiv * 2 + i] = pos + Vector3(s, 0.0f, c);
	}
	std::vector<Vertex> v(kDiv * 2 * 3);
	v[0] = { p[0],col };
	v[1] = { p[1],col };
	for (uint32_t i = 1; i < kDiv * 3; ++i)
	{
		v[i * 2 + 0] = { p[i],col };
		if ((i + 1) % kDiv == 0)
		{
			v[i * 2 + 1] = { p[i + 1 - kDiv],col };
		}
		else
		{
			v[i * 2 + 1] = { p[i + 1],col };
		}
	}
	LineList3(v);
}

// Grid
void Primitive::DrawGrid(uint32_t gridNum)
{
	Color col = Color(1.0f, 1.0f, 1.0f, 0.1f);

	float size = (gridNum - 1) / 2.0f;
	std::vector<Vertex> v(2 * gridNum * 2);
	for (uint32_t i = 0; i < gridNum; ++i)
	{
		float z = size - i;
		v[i * 2 + 0] = { Vector3(-size,0.0f,z),col };
		v[i * 2 + 1] = { Vector3(size,0.0f,z),col };
		if (i == uint32_t((gridNum - 1) / 2.0f))
		{
			v[i * 2 + 0].mColor = Color(1.0f, 0.2f, 0.2f, 1.0f);
			v[i * 2 + 1].mColor = Color(1.0f, 0.2f, 0.2f, 1.0f);
		}
	}
	for (uint32_t i = gridNum; i < gridNum * 2; ++i)
	{
		float x = -size + i - gridNum;
		v[i * 2 + 0] = { Vector3(x,0.0f,size),col };
		v[i * 2 + 1] = { Vector3(x,0.0f,-size),col };
		if (i - gridNum == uint32_t((gridNum - 1) / 2.0f))
		{
			v[i * 2 + 0].mColor = Color(0.2f, 0.2f, 1.0f, 1.0f);
			v[i * 2 + 1].mColor = Color(0.2f, 0.2f, 1.0f, 1.0f);
		}
	}
	//LineList3(v);
	// グリッド用
	uint32_t vSize = uint32_t(v.size());
	MY_ASSERT(mIndex + vSize < kMaxVertex);
	auto data = static_cast<Vertex*>(mVBuff->GetData());
	std::copy(v.begin(), v.end(), &data[mIndex]);
	mRootSignature->Bind(mCmdList);
	mGridPso.Bind(mCmdList);
	mCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	mVBuff->Bind(mCmdList);
	mCBuff3->Bind(mCmdList, 0);
	mCmdList->DrawInstanced(vSize, 1, mIndex, 0);
	mIndex += vSize;
}

// Line List 2D
void Primitive::LineList2(const std::vector<Vertex>& v)
{
	MY_ASSERT(mCmdList);
	uint32_t size = uint32_t(v.size());
	MY_ASSERT(mIndex + size < kMaxVertex);
	auto data = static_cast<Vertex*>(mVBuff->GetData());
	std::copy(v.begin(), v.end(), &data[mIndex]);
	mRootSignature->Bind(mCmdList);
	mLinePso2.Bind(mCmdList);
	mCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	mVBuff->Bind(mCmdList);
	mCBuff2->Bind(mCmdList, 0);
	mCmdList->DrawInstanced(size, 1, mIndex, 0);
	mIndex += size;
}

// Line List 3D
void Primitive::LineList3(const std::vector<Vertex>& v)
{
	MY_ASSERT(mCmdList);
	uint32_t size = uint32_t(v.size());
	MY_ASSERT(mIndex + size < kMaxVertex);
	auto data = static_cast<Vertex*>(mVBuff->GetData());
	std::copy(v.begin(), v.end(), &data[mIndex]);
	mRootSignature->Bind(mCmdList);
	mLinePso3.Bind(mCmdList);
	mCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	mVBuff->Bind(mCmdList);
	mCBuff3->Bind(mCmdList, 0);
	mCmdList->DrawInstanced(size, 1, mIndex, 0);
	mIndex += size;
}

// Triangle List 2D
void Primitive::TriList2(const std::vector<Vertex>& v)
{
	MY_ASSERT(mCmdList);
	uint32_t size = uint32_t(v.size());
	MY_ASSERT(mIndex + size < kMaxVertex);
	auto data = static_cast<Vertex*>(mVBuff->GetData());
	std::copy(v.begin(), v.end(), &data[mIndex]);
	mRootSignature->Bind(mCmdList);
	mPrimPso3.Bind(mCmdList);
	mCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mVBuff->Bind(mCmdList);
	mCBuff2->Bind(mCmdList, 0);
	mCmdList->DrawInstanced(size, 1, mIndex, 0);
	mIndex += size;
}
