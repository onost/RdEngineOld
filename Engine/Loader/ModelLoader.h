#pragma once
#include "Matrix4.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>

class Model;

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

	static Model* Load(const std::string& modelName);

private:
	// ノードを解析
	static Node ReadNode(aiNode* node);
};
