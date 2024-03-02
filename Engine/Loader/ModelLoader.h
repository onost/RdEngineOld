#pragma once
#include "Matrix4.h"
#include "QuaternionTransform.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>
#include "Graphics/Model/Skeleton.h"

struct Animation;
class Model;
class Renderer;
struct Skeleton;

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
	static Animation* LoadAnimation(const std::string& modelName);

	static void SetRenderer(Renderer* renderer) { mRenderer = renderer; }

private:
	// ノードを解析
	static Node ReadNode(aiNode* node);

	// スケルトンを作成
	static Skeleton* CreateSkeleton(const Node& rootNode);
	// ジョイントを作成
	static int32_t CreateJoint(
		const Node& node,
		const std::optional<int32_t>& parent,
		std::vector<Joint>& joints);

private:
	static Renderer* mRenderer;
};
