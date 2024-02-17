#pragma once
#include <string>

class Model;

// .obj
class ObjLoader
{
public:
	static Model* Load(const std::string& modelName);
private:
	// .mtl
	static bool LoadMtl(
		Model* model, const std::string& filePath, const std::string& fileName);
};
