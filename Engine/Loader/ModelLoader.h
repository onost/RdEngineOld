#pragma once
#include <string>

class Model;

// glTFでモデルを読み込む
class ModelLoader
{
public:
	static Model* Load(const std::string& modelName);
};
