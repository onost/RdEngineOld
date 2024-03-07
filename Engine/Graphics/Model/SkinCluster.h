#pragma once
#include "Core/StructuredBuffer.h"
#include "Core/VertexBuffer.h"
#include "Matrix4.h"
#include <array>
#include <memory>
#include <vector>

class Mesh;
class Skeleton;

const uint32_t kMaxInfluence = 4;
// Skinning
struct VertexInfluence
{
	std::array<float, kMaxInfluence> mWeights;
	std::array<int32_t, kMaxInfluence> mJointIndices;
};

struct Well
{
	// Matrix Palette
	Matrix4 mSkelSpaceMat;
};

class SkinCluster
{
public:
	void Create(Mesh* mesh, Skeleton* skeleton);
	void Update(const std::vector<Matrix4>& poses);
	void Bind(ID3D12GraphicsCommandList* cmdList, uint32_t rootParamIdx);
	const D3D12_VERTEX_BUFFER_VIEW& GetView() const { return mInfluence->GetView(); }

private:
	// 逆バインドポーズ行列
	std::vector<Matrix4> mInvBindPoses;
	// 頂点
	std::unique_ptr<VertexBuffer> mInfluence;
	// Matrix Palette
	std::unique_ptr<StructuredBuffer> mPalette;
};
