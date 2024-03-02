#pragma once
#include "Matrix4.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>

class Model;
struct Animation;

// glTFでモデルを読み込む
class ModelLoader
{
public:
	struct Node
	{
		std::string mName;
		std::vector<Node> mChildren;
		Matrix4 mLocal;
	};

	// モデル
	static Model* Load(const std::string& modelName);
	// アニメーション
	static Animation* LoadAnimation(const std::string& modelName);

private:
	// ノードを解析
	static Node ReadNode(aiNode* node);
};
