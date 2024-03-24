#include "ObjLoader.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Model/ModelCommon.h"
#include <fstream>
#include <format>
#include <sstream>

Model* ObjLoader::Load(const std::string& modelName)
{
	//Helper::WriteToConsole(std::format("Create: \"{}\"\n", modelName.c_str()));

	std::string filePath =
		ModelCommon::kModelPath + Helper::ExcludeExtension(modelName) + "/";
	std::ifstream fstream;
	fstream.open(filePath + modelName);
	if (fstream.fail())
	{
		return nullptr;
	}
	Model* model = new Model();
	Mesh* mesh = nullptr;
	std::vector<Vector3> positions;
	std::vector<Vector2> uvs;
	std::vector<Vector3> normals;
	uint16_t index = 0;
	std::string line;
	while (std::getline(fstream, line))
	{
		std::istringstream sstream(line);
		std::string key;
		std::getline(sstream, key, ' ');
		if (key == "mtllib")
		{
			std::string mtlFileName;
			sstream >> mtlFileName;
			if (!LoadMtl(model, filePath, mtlFileName))
			{
				delete model;
				return nullptr;
			}
		}
		if (key == "v")
		{
			Vector3 position;
			sstream >> position.x;
			sstream >> position.y;
			sstream >> position.z;
			position.x *= -1.0f;
			positions.emplace_back(position);
		}
		if (key == "vt")
		{
			Vector2 uv;
			sstream >> uv.x;
			sstream >> uv.y;
			uv.y = 1.0f - uv.y;
			uvs.emplace_back(uv);
		}
		if (key == "vn")
		{
			Vector3 normal;
			sstream >> normal.x;
			sstream >> normal.y;
			sstream >> normal.z;
			normal.x *= -1.0f;
			normals.emplace_back(normal);
		}
		if (key == "f")
		{
			uint32_t vertCount = 0;
			std::vector<uint16_t> indices;
			while (std::getline(sstream, line, ' '))
			{
				std::istringstream idxStream(line);
				Mesh::Vertex vertex = {};
				uint16_t idx;
				idxStream >> idx;
				vertex.mPos = positions[idx - 1];
				idxStream.seekg(1, std::ios_base::cur);
				idxStream >> idx;
				vertex.mUv = uvs[idx - 1];
				idxStream.seekg(1, std::ios_base::cur);
				idxStream >> idx;
				vertex.mNormal = normals[idx - 1];
				mesh->mVertices.emplace_back(vertex);
				if (vertCount >= 3)
				{
					//mesh->mIndices.emplace_back(static_cast<uint16_t>(index - 1));
					//mesh->mIndices.emplace_back(static_cast<uint16_t>(index));
					//mesh->mIndices.emplace_back(static_cast<uint16_t>(index - 3));
					indices.emplace_back(static_cast<uint16_t>(index - 1));
					indices.emplace_back(static_cast<uint16_t>(index));
					indices.emplace_back(static_cast<uint16_t>(index - 3));
				}
				else
				{
					//mesh->mIndices.emplace_back(static_cast<uint16_t>(index));
					indices.emplace_back(static_cast<uint16_t>(index));
				}
				++vertCount;
				++index;
			}
			for (int32_t i = int32_t(indices.size()) - 1; i >= 0; --i)
			{
				mesh->mIndices.emplace_back(indices[i]);
			}
		}
		if (key == "usemtl")
		{
			//auto m = std::make_unique<Mesh>();
			//mesh = m.get();
			mesh = new Mesh();
			model->mMeshes.emplace_back(mesh);
			index = 0;
			std::string name;
			sstream >> name;
			auto it = model->mMaterials.find(name);
			if (it != model->mMaterials.end())
			{
				mesh->mMaterial = it->second;
			}
		}
	}
	fstream.close();
	model->Create(modelName);
	return model;
}

// .mtl
bool ObjLoader::LoadMtl(
	Model* model, const std::string& filePath, const std::string& fileName)
{
	std::ifstream fstream;
	fstream.open(filePath + fileName);
	if (fstream.fail())
	{
		return false;
	}
	Material* material = nullptr;
	std::string line;
	while (std::getline(fstream, line))
	{
		std::istringstream sstream(line);
		std::string key;
		std::getline(sstream, key, ' ');
		if (key == "newmtl")
		{
			//auto m = std::make_unique<Material>();
			//material = m.get();
			material = new Material();
			std::string name;
			sstream >> name;
			model->mMaterials.emplace(name, material);
			material->mName = name;
		}
		if (key == "Kd")
		{
			Color baseColor;
			sstream >> baseColor.r;
			sstream >> baseColor.g;
			sstream >> baseColor.b;
			baseColor.a = 1.0f;
			material->mBaseColor = baseColor;
		}
		if (key == "map_Kd")
		{
			std::string texturePath;
			sstream >> texturePath;
			material->mTexturePath = filePath + Helper::ExtractFileName(texturePath);
		}
	}
	fstream.close();
	return true;
}
