#pragma once
#include "Graphics/Model/Skeleton.h"
#include "Matrix4.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <optional>
#include <string>
#include <vector>

class Animation;
class Model;
class Renderer;
class Skeleton;

// glTFでモデルを読み込む
class ModelLoader
{
public:
	struct Node
	{
		QuaternionTransform mTransform;
		std::string mName;
		std::vector<Node> mChildren;
		Matrix4 mLocal;
	};

	// モデル
	static Model* LoadModel(const std::string& modelName);
	// アニメーション
	static void LoadAnimation(const std::string& modelName);

	static void SetRenderer(Renderer* renderer) { mRenderer = renderer; }

private:
	// ノードを解析
	static Node ReadNode(aiNode* node);

	// スケルトン
	static Skeleton* CreateSkeleton(const Node& rootNode);
	// ジョイント
	static int32_t CreateJoint(
		const Node& node, const std::optional<int32_t>& parent,
		std::vector<Joint>& joints);

private:
	static Renderer* mRenderer;
};
