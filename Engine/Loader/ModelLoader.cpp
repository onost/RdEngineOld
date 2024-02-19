#include "ModelLoader.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Model/ModelCommon.h"
#include <format>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model* ModelLoader::Load(const std::string& modelName)
{
	Helper::WriteToConsole(std::format("Create: \"{}\"\n", modelName.c_str()));

	std::string filePath =
		ModelCommon::kModelPath + Helper::RemoveExtension(modelName) + "/";
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile((filePath + modelName).c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	//aiProcess_Triangulate: 三角形化
	MyAssert(scene->HasMeshes());// メッシュなし

	Model* model = new Model();
	std::vector<std::string> matNames;

	// マテリアル
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex)
	{
		Material* myMaterial = new Material();
		aiMaterial* material = scene->mMaterials[materialIndex];
		// 名前
		aiString name;
		material->Get(AI_MATKEY_NAME, name);
		myMaterial->mName = name.C_Str();
		// ベースカラー
		aiColor4D baseColor;
		material->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor);
		myMaterial->mBaseColor = Color(baseColor.r, baseColor.g, baseColor.b, baseColor.a);
		// テクスチャ
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0)
		{
			aiString texturePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
			//myMaterial->mTexturePath = texturePath.C_Str();
			myMaterial->mTexturePath = filePath + Helper::GetFileName(texturePath.C_Str());
		}
		// 追加
		model->mMaterials.emplace(myMaterial->GetName(), myMaterial);
		matNames.emplace_back(myMaterial->GetName());
	}

	// メッシュ
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
	{
		Mesh* myMesh = new Mesh();
		uint16_t index = 0;

		aiMesh* mesh = scene->mMeshes[meshIndex];
		MyAssert(mesh->HasNormals());// 法線なし
		MyAssert(mesh->HasTextureCoords(0));// UV座標なし

		// 面
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
		{
			aiFace& face = mesh->mFaces[faceIndex];
			MyAssert(face.mNumIndices <= 4);

			uint32_t vertCount = 0;
			// 頂点
			for (uint32_t element = 0; element < face.mNumIndices; ++element)
			{
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& uv = mesh->mTextureCoords[0][vertexIndex];

				// 頂点
				Mesh::Vertex vertex = {};
				vertex.mPos = Vector3(position.x, position.y, position.z);
				vertex.mNormal = Vector3(normal.x, normal.y, normal.z);
				vertex.mUv = Vector2(uv.x, uv.y);
				vertex.mPos.x *= -1.0f;
				vertex.mNormal.x *= -1.0f;
				myMesh->mVertices.emplace_back(vertex);

				// インデックス
				if (vertCount >= 3)
				{
					myMesh->mIndices.emplace_back(static_cast<uint16_t>(index - 1));
					myMesh->mIndices.emplace_back(static_cast<uint16_t>(index));
					myMesh->mIndices.emplace_back(static_cast<uint16_t>(index - 3));
				}
				else
				{
					myMesh->mIndices.emplace_back(static_cast<uint16_t>(index));
				}
				++vertCount;
				++index;

				// マテリアル
				auto matIndex = mesh->mMaterialIndex;
				auto name = matNames[matIndex];
				myMesh->mMaterial = model->mMaterials[name];
			}
		}
		// 追加
		model->mMeshes.emplace_back(myMesh);
	}
	model->Create(modelName);
	return model;
}
