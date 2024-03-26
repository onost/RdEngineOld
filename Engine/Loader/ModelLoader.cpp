#include "ModelLoader.h"
#include "Graphics/Model/Animation.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Model/ModelCommon.h"
#include "Graphics/Renderer.h"
#include <format>

Renderer* ModelLoader::mRenderer = nullptr;

// モデル
Model* ModelLoader::LoadModel(const std::string& modelName)
{
	//Helper::WriteToConsole(std::format("Create: \"{}\"\n", modelName.c_str()));

	std::string filePath =
		ModelCommon::kModelPath + Helper::ExcludeExtension(modelName) + "/";
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile((filePath + modelName).c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	//aiProcess_Triangulate: 三角形化
	//MyAssert(scene->HasMeshes());

	Model* model = new Model();
	std::vector<std::string> materialNames;
	// ノードを解析
	auto node = ReadNode(scene->mRootNode);

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
			myMaterial->mTexturePath = filePath + Helper::ExtractFileName(texturePath.C_Str());
		}
		// 追加
		model->mMaterials.emplace(myMaterial->GetName(), myMaterial);
		materialNames.emplace_back(myMaterial->GetName());
	}

	// メッシュ
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
	{
		Mesh* myMesh = new Mesh();
		aiMesh* mesh = scene->mMeshes[meshIndex];
		MY_ASSERT(mesh->HasNormals());
		MY_ASSERT(mesh->HasTextureCoords(0));

		// 面
		myMesh->mVertices.resize(mesh->mNumVertices);
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
		{
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& uv = mesh->mTextureCoords[0][vertexIndex];
			// 頂点
			myMesh->mVertices[vertexIndex].mPos = Vector3(-position.x, position.y, position.z);
			myMesh->mVertices[vertexIndex].mNormal = Vector3(-normal.x, normal.y, normal.z);
			myMesh->mVertices[vertexIndex].mUv = Vector2(uv.x, uv.y);
		}

		// インデックス
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
		{
			aiFace& face = mesh->mFaces[faceIndex];
			MY_ASSERT(face.mNumIndices <= 4);

			for (uint32_t i = 0; i < face.mNumIndices; ++i)
			{
				if (i == 3)
				{
					myMesh->mIndices.emplace_back(face.mIndices[i - 1]);
					myMesh->mIndices.emplace_back(face.mIndices[i]);
					myMesh->mIndices.emplace_back(face.mIndices[i - 3]);
				}
				else
				{
					myMesh->mIndices.emplace_back(face.mIndices[i]);
				}
			}
		}

		// マテリアル
		auto materialIndex = mesh->mMaterialIndex;
		auto materialName = materialNames[materialIndex];
		myMesh->mMaterial = model->mMaterials[materialName];

		// トランスフォーム
		myMesh->mLocal = node.mLocal;

		// スケルトン
		Skeleton* skeleton = nullptr;
		if (mesh->mNumBones > 0)
		{
			if (scene->mNumAnimations > 0)// アニメーションがある
			{
				skeleton = CreateSkeleton(node);
				skeleton->SetName(modelName);// モデル名として
				//mRenderer->AddSkeleton(modelName, skeleton);
			}
		}
		myMesh->mSkeleton = skeleton;
		// ボーン（ジョイント）
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
		{
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			Skeleton::JointData& jointData = skeleton->mForSkinCluster[jointName];
			// 逆バインドポーズ行列を計算
			aiMatrix4x4 bindPose = bone->mOffsetMatrix.Inverse();
			aiVector3D scale;
			aiQuaternion rotate;
			aiVector3D translate;
			bindPose.Decompose(scale, rotate, translate);
			Matrix4 myBindPose = Matrix4::CreateAffine(
				Vector3(scale.x, scale.y, scale.z),
				Quaternion(rotate.w, rotate.x, -rotate.y, -rotate.z),
				Vector3(-translate.x, translate.y, translate.z));
			jointData.mInvBindPose = Inverse(myBindPose);
			// Weight
			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex)
			{
				Skeleton::WeightData weightData = {};
				weightData.mWeight = bone->mWeights[weightIndex].mWeight;
				weightData.mVertexIndex = bone->mWeights[weightIndex].mVertexId;
				jointData.mWeightData.emplace_back(weightData);
			}
		}

		// 追加
		model->mMeshes.emplace_back(myMesh);
	}
	model->Create(modelName);

	// アニメーション
	LoadAnimation(modelName);

	return model;
}

// アニメーション
void ModelLoader::LoadAnimation(const std::string& modelName)
{
	//Helper::WriteToConsole(std::format("Create: \"{}\"\n", modelName.c_str()));

	std::string filePath =
		ModelCommon::kModelPath + Helper::ExcludeExtension(modelName) + "/";
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile((filePath + modelName).c_str(), 0);
	//MyAssert(scene->mNumAnimations != 0);

	for (uint32_t animIndex = 0; animIndex != scene->mNumAnimations; ++animIndex)
	{
		aiAnimation* anim = scene->mAnimations[animIndex];//
		std::shared_ptr<Animation> myAnim = std::make_shared<Animation>();
		myAnim->mName = modelName + "/" + anim->mName.C_Str();// 名前
		myAnim->mDuration = float(anim->mDuration / anim->mTicksPerSecond);// 秒へ
		for (uint32_t channelIndex = 0; channelIndex < anim->mNumChannels; ++channelIndex)
		{
			aiNodeAnim* nodeAnim = anim->mChannels[channelIndex];
			NodeAnimation& myNodeAnim = myAnim->mNodeAnimations[nodeAnim->mNodeName.C_Str()];
			// Scale
			for (uint32_t keyIndex = 0; keyIndex < nodeAnim->mNumScalingKeys; ++keyIndex)
			{
				aiVectorKey& key = nodeAnim->mScalingKeys[keyIndex];
				Keyframe<Vector3> myKey;
				myKey.mTime = float(key.mTime / anim->mTicksPerSecond);
				myKey.mValue = Vector3(key.mValue.x, key.mValue.y, key.mValue.z);
				myNodeAnim.mScale.emplace_back(myKey);
			}
			// Rotation
			for (uint32_t keyIndex = 0; keyIndex < nodeAnim->mNumRotationKeys; ++keyIndex)
			{
				aiQuatKey& key = nodeAnim->mRotationKeys[keyIndex];
				Keyframe<Quaternion> myKey;
				myKey.mTime = float(key.mTime / anim->mTicksPerSecond);
				myKey.mValue = Quaternion(key.mValue.w, key.mValue.x, -key.mValue.y, -key.mValue.z);
				myNodeAnim.mRotate.emplace_back(myKey);
			}
			// Position
			for (uint32_t keyIndex = 0; keyIndex < nodeAnim->mNumPositionKeys; ++keyIndex)
			{
				aiVectorKey& key = nodeAnim->mPositionKeys[keyIndex];
				Keyframe<Vector3> myKey;
				myKey.mTime = float(key.mTime / anim->mTicksPerSecond);
				myKey.mValue = Vector3(-key.mValue.x, key.mValue.y, key.mValue.z);
				myNodeAnim.mTranslate.emplace_back(myKey);
			}
		}
		mRenderer->AddAnimation(myAnim->GetName(), myAnim);
	}
}

// ノードを解析
ModelLoader::Node ModelLoader::ReadNode(aiNode* node)
{
	Node result = {};
	aiVector3D scale;
	aiQuaternion rotate;
	aiVector3D translate;
	node->mTransformation.Decompose(scale, rotate, translate);
	result.mTransform.mScale = Vector3(scale.x, scale.y, scale.z);
	result.mTransform.mRotate = Quaternion(rotate.w, rotate.x, -rotate.y, -rotate.z);
	result.mTransform.mTranslate = Vector3(-translate.x, translate.y, translate.z);
	// local
	//aiMatrix4x4 local = node->mTransformation;
	result.mLocal = Matrix4::CreateAffine(
		result.mTransform.mScale,
		result.mTransform.mRotate,
		result.mTransform.mTranslate);
	//local.Transpose();
	/*result.mLocal.m[0][0] = local[0][0];
	result.mLocal.m[0][1] = local[1][0];
	result.mLocal.m[0][2] = local[2][0];
	result.mLocal.m[0][3] = local[3][0];
	result.mLocal.m[1][0] = local[0][1];
	result.mLocal.m[1][1] = local[1][1];
	result.mLocal.m[1][2] = local[2][1];
	result.mLocal.m[1][3] = local[3][1];
	result.mLocal.m[2][0] = local[0][2];
	result.mLocal.m[2][1] = local[1][2];
	result.mLocal.m[2][2] = local[2][2];
	result.mLocal.m[2][3] = local[3][2];
	result.mLocal.m[3][0] = local[0][3];
	result.mLocal.m[3][1] = local[1][3];
	result.mLocal.m[3][2] = local[2][3];
	result.mLocal.m[3][3] = local[3][3];*/
	// name
	result.mName = node->mName.C_Str();
	// children
	result.mChildren.resize(node->mNumChildren);
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
	{
		// 再帰
		result.mChildren[childIndex] = ReadNode(node->mChildren[childIndex]);
	}
	return result;
}

// スケルトンを作成
Skeleton* ModelLoader::CreateSkeleton(const Node& rootNode)
{
	Skeleton* skeleton = new Skeleton();
	skeleton->mRoot = CreateJoint(rootNode, {}, skeleton->mJoints);
	for (const Joint& joint : skeleton->mJoints)
	{
		skeleton->mJointMap.emplace(joint.mName, joint.mIndex);
	}
	skeleton->Update();
	return skeleton;
}

// ジョイントを作成
int32_t ModelLoader::CreateJoint(
	const Node& node, const std::optional<int32_t>& parent,
	std::vector<Joint>& joints)
{
	Joint joint = {};
	joint.mName = node.mName;
	joint.mTransform = node.mTransform;
	joint.mLocal = node.mLocal;
	joint.mSkelSpaceMat = Matrix4::kIdentity;
	joint.mIndex = int32_t(joints.size());
	joint.mParent = parent;
	joints.emplace_back(joint);
	for (const Node& child : node.mChildren)
	{
		int32_t childIndex = CreateJoint(child, joint.mIndex, joints);
		joints[joint.mIndex].mChildren.emplace_back(childIndex);
	}
	return joint.mIndex;
}
