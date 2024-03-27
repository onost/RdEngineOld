#pragma once
#include "Matrix4.h"
#include "Quaternion.h"
#include "Vector3.h"
#include <map>
#include <optional>
#include <string>
#include <vector>

class Animation;

// クォータニオンを使ったトランスフォーム
struct QuaternionTransform
{
	Vector3 mScale;
	Quaternion mRotate;
	Vector3 mTranslate;
};


// ジョイント
struct Joint
{
	std::string mName;
	// ジョイントのトランスフォーム
	QuaternionTransform mTransform;
	Matrix4 mLocal;
	Matrix4 mSkelSpaceMat;// スケルトンスペースでの行列
	// インデックス
	int32_t mIndex;
	std::optional<int32_t> mParent;// 親
	std::vector<int32_t> mChildren;// 子
};

// スケルトン
class Skeleton
{
	friend class ModelLoader;
public:
	// 頂点
	struct WeightData
	{
		// 重み
		float mWeight;
		// 頂点のインデックス
		uint32_t mVertexIndex;
	};
	// ジョイント
	struct JointData
	{
		// ジョイントの逆バインドポーズ行列
		Matrix4 mInvBindPose;
		// 影響を与える頂点
		std::vector<WeightData> mWeightData;
	};

	void Update();
	//void ApplyAnimation(const Animation* anim, float time);

	const std::vector<Joint>& GetJoints() const { return mJoints; }
	const std::map<std::string, int32_t>& GetJointMap() const { return mJointMap; }
	const std::map<std::string, JointData>& GetForSkinCluster() const { return mForSkinCluster; }
	void SetName(const std::string& name) { mName = name; }

private:
	std::string mName;
	// ルートボーンのインデックス
	int32_t mRoot;
	// ジョイント
	std::vector<Joint> mJoints;

	// string: ジョイント名
	// int32_t: ジョイントのインデックス
	std::map<std::string, int32_t> mJointMap;
	// string: ジョイント名
	// JointData: Skinning
	std::map<std::string, JointData> mForSkinCluster;
};
