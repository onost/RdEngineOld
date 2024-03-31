#include "Renderer.h"
#include "Component/CameraComponent.h"
#include "Component/SpriteRenderer.h"
#include "Component/MeshParticleRenderer.h"
#include "Component/MeshRenderer.h"
#include "Component/ParticleRenderer.h"
#include "Component/SkinnedMeshRenderer.h"
#include "Editor.h"
#include <ImGui/imgui_internal.h>
//#include "Loader/ObjLoader.h"
#include "Loader/ModelLoader.h"
#include "Model/ModelCommon.h"
#include "Particle/ParticleCommon.h"
#include "RdEngine.h"
#include "Sprite/SpriteCommon.h"
#include "Window.h"

void Renderer::Load()
{
	GetModel("Plane.obj");
}

void Renderer::Initialize()
{
	// スプライト
	SpriteCommon::Initialize(this);
	// モデル
	ModelCommon::Initialize(this);
	// パーティクル
	ParticleCommon::Initialize(this);
	// ライト管理
	mLightManager = std::make_unique<LightManager>();
	mLightManager->Initialize();
	// プリミティブ
	mPrimitive = std::make_unique<Primitive>();
	mPrimitive->Initialize(this);

	// メインレンダーターゲット
	mMainRt = std::make_unique<RenderTarget>();
	mMainRt->Create();
	// 最終レンダーターゲット
	mFinalRt = std::make_unique<RenderTarget>();
	mFinalRt->Create();
	mFinalSprite = std::make_unique<Sprite>();
	mFinalSprite->Create();

	// ガウシアンブラー
	mGaussianBlur = std::make_unique<GaussianBlur>();
	mGaussianBlur->Initialize(mMainRt->GetRenderTarget().get(), this);

	// モデル用関数テーブル
	//mModelFuncs["obj"] = &Renderer::GetModelFromObj;// .obj
	//mModelFuncs["fbx"] = &Renderer::GetModelFromFbx;// .fbx

	ModelLoader::SetRenderer(this);

	Load();
}

/*void Renderer::Terminate()
{

}*/

// シーン描画前
void Renderer::PreRendering(ID3D12GraphicsCommandList* cmdList)
{
	mMainRt->PreRender(cmdList);
}

// シーン描画後
void Renderer::PostRendering(ID3D12GraphicsCommandList* cmdList)
{
	//Editor::PostProcess();///
	//Editor::Draw(cmdList);///

	mMainRt->PostRender();

	//Editor::PreProcess();

	// ==================================================
	// ポストエフェクト
	// ==================================================

	SpriteCommon::PreRendering(cmdList);
	// ガウシアンブラー
	Texture* texture = nullptr;
	if (mIsGaussianBlur)
	{
		mGaussianBlur->Execute(cmdList, mGaussianPower);
		texture = mGaussianBlur->GetTexture();
	}
	else
	{
		texture = mMainRt->GetRenderTarget().get();
	}
	SpriteCommon::PostRendering();



	if (Editor::IsEditor() && (Editor::gEditorState == Editor::EditorState::kEdit || !Editor::mIsMaximum))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, -1.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(-1.0f, -1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(-1.0f, -1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(-1.0f, -1.0f));
		// ImGuiへ描画
		//static ImGuiWindowFlags gizmoWindowFlags = 0;

		ImGui::Begin("Game Screen", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		ImVec2 size = ImGui::GetWindowSize();
		//ImGui::SetWindowSize(ImVec2(size.x, Window::kHeight * size.x / Window::kWidth));
		//ImVec2 size = ImGui::GetWindowSize();
		//ImGui::Image(
		//	ImTextureID(texture->GetDescHandle().ptr),
		//	ImVec2(size.x, Window::kHeight * size.x / Window::kWidth));
		//// モデルのドラッグアンドドロップでメッシュレンダラー付のアクターを作成
		//if (ImGui::BeginDragDropTarget())
		//{
		//	if (auto payload = ImGui::AcceptDragDropPayload("MODEL_PAYLOAD"))
		//	{
		//		auto model = *(Model**)(payload->Data);
		//		auto actor = new Actor(gEngine->GetSceneManager()->GetCurrScene());
		//		auto mr = new MeshRenderer(actor);
		//		mr->SetModel(model);
		//		actor->SetName(Helper::RemoveExtension(model->GetName()));
		//	}
		//	ImGui::EndDragDropTarget();
		//}

		auto scene = gEngine->GetSceneManager()->GetCurrScene();
		if (scene)
		{
			auto actor = scene->GetActorForDev();
			if (actor)
			{

				ImGui::Image(
					ImTextureID(texture->GetDescHandle()->mGpuHandle.ptr),
					ImVec2(size.x, Window::kHeight * size.x / Window::kWidth));
				// モデルのドラッグアンドドロップでメッシュレンダラー付のアクターを作成
				if (ImGui::BeginDragDropTarget())
				{
					if (auto payload = ImGui::AcceptDragDropPayload("MODEL_PAYLOAD"))
					{
						auto model = *(Model**)(payload->Data);
						auto actor2 = new Actor(gEngine->GetSceneManager()->GetCurrScene());
						auto mr = new MeshRenderer(actor2);
						mr->SetModel(model);
						actor2->SetName(Helper::ExcludeExtension(model->GetName()));
					}
					ImGui::EndDragDropTarget();
				}

				//if (!gEngine->GetIsMaximum())
				{

					//auto min = ImGui::GetItemRectMin();
					//auto max = ImGui::GetItemRectMax();
					//auto w = max.x - min.x;
					//auto h = max.y - min.y;
					//ImGuizmo::SetRect(min.x, min.y, w, h);
					//float windowWidth = (float)ImGui::GetWindowWidth();
					//float windowHeight = (float)ImGui::GetWindowHeight();
					ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, size.x, Window::kHeight * size.x / Window::kWidth);
					//float viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
					//float viewManipulateTop = ImGui::GetWindowPos().y;
					/*ImGuiWindow* window = ImGui::GetCurrentWindow();
					gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;*/



					ImGuizmo::SetDrawlist();
					auto world = actor->mTransform->GetWorld();
					auto view = mCurrCamera->GetView();
					ImGuizmo::Manipulate(&view.m[0][0], &mCurrCamera->GetProj().m[0][0], ImGuizmo::TRANSLATE, ImGuizmo::WORLD, &world.m[0][0]);
					/*auto dist = Length(actor->mTransform->GetWorld().GetTranslation() - mCurrCamera->GetPosition());
					ImGuizmo::ViewManipulate(&view.m[0][0], dist, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);*/

					auto p = actor->GetParent();
					Matrix4 pm = Matrix4::kIdentity;
					if (p)
					{
						pm = p->mTransform->GetWorld();
					}

					world = Inverse(pm) * world;
					actor->mTransform->mScale = world.GetScale();
					actor->mTransform->mRotation = world.GetRotation();
					actor->mTransform->mPosition = world.GetTranslation();
				}
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
	}
	else
	{
		// スプライトへテクスチャをセット
		mFinalSprite->SetTexture(texture);
	}
}

// シーン描画
void Renderer::RenderScene(ID3D12GraphicsCommandList* cmdList)
{
	// ==================================================
	// 前処理
	// ==================================================

	// カメラ
	if (Editor::mIsDebugCamera &&
		Editor::IsEditor() &&
		(Editor::gEditorState == Editor::EditorState::kEdit || !Editor::mIsMaximum))
	{
		mCurrCamera = Editor::mDebugCamera->GetCamera();
	}
	else if (mGameCamera)
	{
		mCurrCamera = mGameCamera->GetCamera();
	}
	if (mCurrCamera)
	{
		// ビュープロジェクション行列を更新
		mCurrCamera->UpdateViewProj();
	}

	// ライト管理
	//mLightManager->Update();

	// ==================================================
	// レンダリング
	// ==================================================

	// 背景スプライト
	SpriteCommon::PreRendering(cmdList);
	for (auto& renderer : mBackground)
	{
		renderer->Draw();
	}
	SpriteCommon::PostRendering();

	// メッシュ
	ModelCommon::PreRendering(cmdList);
	for (auto& renderer : mMeshes)
	{
		renderer->Draw();
	}
	for (auto& renderer : mSkinnedMeshes)
	{
		renderer->Draw();
	}
	ModelCommon::PostRendering();

	// パーティクル
	ParticleCommon::PreRendering(cmdList);
	for (auto& renderer : mMeshParticles)
	{
		renderer->Draw();
	}
	// 深度を書かない
	for (auto& renderer : mParticles)
	{
		renderer->Draw();
	}
	ParticleCommon::PostRendering();

	// 前景スプライト
	SpriteCommon::PreRendering(cmdList);
	for (auto& renderer : mForeground)
	{
		renderer->Draw();
	}
	SpriteCommon::PostRendering();
}

void Renderer::RenderFinal(ID3D12GraphicsCommandList* cmdList)
{
	if (Editor::IsEditor() &&
		(Editor::gEditorState == Editor::EditorState::kEdit || !Editor::mIsMaximum))
	{
		// エディタ上に表示
		Editor::Draw(cmdList);
	}
	else
	{
		SpriteCommon::PreRendering(cmdList);
		// ビューポートをセットし直す
		D3D12_VIEWPORT viewport = {};
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = FLOAT(Window::kWidth);
		viewport.Height = FLOAT(Window::kHeight);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		cmdList->RSSetViewports(1, &viewport);
		mFinalSprite->Draw(Vector2(float(Window::kWidth), float(Window::kHeight)));
		SpriteCommon::PostRendering();
		Editor::Draw(cmdList);
	}
}

// スプライトをソート
void Renderer::SortSprites(SpriteRenderer* sprite)
{
	uint32_t order = sprite->GetDrawOrder();
	auto it = std::find(mForeground.begin(), mForeground.end(), sprite);
	if (it != mForeground.end())
	{
		if (order < 100)
		{
			mForeground.erase(it);
			AddSprite(sprite);
		}
		else
		{
			std::sort(mForeground.begin(), mForeground.end(),
				[](SpriteRenderer* a, SpriteRenderer* b)
				{
					return a->GetDrawOrder() < b->GetDrawOrder();
				}
			);
		}
		return;
	}
	it = std::find(mBackground.begin(), mBackground.end(), sprite);
	if (it != mBackground.end())
	{
		if (order >= 100)
		{
			mBackground.erase(it);
			AddSprite(sprite);
		}
		else
		{
			std::sort(mBackground.begin(), mBackground.end(),
				[](SpriteRenderer* a, SpriteRenderer* b)
				{
					return a->GetDrawOrder() < b->GetDrawOrder();
				}
			);
		}
	}
}

// ==================================================
// アセット
// ==================================================

// テクスチャ
Texture* Renderer::GetTexture(const std::string& filePath)
{
	std::shared_ptr<Texture> texture = mTextures.Get(filePath);
	if (!texture)
	{
		texture = std::make_shared<Texture>();
		texture->Create(filePath);
		mTextures.Add(filePath, texture);
	}
	return texture.get();
}

// モデル
Model* Renderer::GetModel(const std::string& filePath)
{
	std::shared_ptr<Model> model = mModels.Get(filePath);
	if (!model)
	{
		//model = ObjLoader::Load(filePath);
		model.reset(ModelLoader::LoadModel(filePath));
		mModels.Add(filePath, model);
	}
	return model.get();
}

// 頂点シェーダ
Shader* Renderer::GetVs(const std::string& filePath)
{
	std::shared_ptr<Shader> shader = mShaders.Get(filePath);
	if (!shader)
	{
		shader = std::make_shared<Shader>();
		shader->CompileVs(filePath);
		mShaders.Add(filePath, shader);
	}
	return shader.get();
}

// ジオメトリシェーダ
Shader* Renderer::GetGs(const std::string& filePath)
{
	std::shared_ptr<Shader> shader = mShaders.Get(filePath);
	if (!shader)
	{
		shader = std::make_shared<Shader>();
		shader->CompileGs(filePath);
		mShaders.Add(filePath, shader);
	}
	return shader.get();
}

// ピクセルシェーダ
Shader* Renderer::GetPs(const std::string& filePath)
{
	std::shared_ptr<Shader> shader = mShaders.Get(filePath);
	if (!shader)
	{
		shader = std::make_shared<Shader>();
		shader->CompilePs(filePath);
		mShaders.Add(filePath, shader);
	}
	return shader.get();
}

// アニメーション
void Renderer::AddAnimation(const std::string& filePath, std::shared_ptr<Animation> animation)
{
	mAnimations.Add(filePath, animation);
}
Animation* Renderer::GetAnimation(const std::string& filePath)
{
	return mAnimations.Get(filePath).get();
}

// スケルトン
void Renderer::AddSkeleton(const std::string& filePath, std::shared_ptr<Skeleton> skeleton)
{
	mSkeletons.Add(filePath, skeleton);
}
Skeleton* Renderer::GetSkeleton(const std::string& filePath)
{
	return mSkeletons.Get(filePath).get();
}

// ==================================================
// レンダラーコンポーネント
// ==================================================

// Add
void Renderer::AddSprite(SpriteRenderer* sprite)
{
	uint32_t order = sprite->GetDrawOrder();
	if (order < 100)
	{
		auto it = mBackground.begin();
		for (; it != mBackground.end(); ++it)
		{
			if (order < (*it)->GetDrawOrder())
			{
				break;
			}
		}
		mBackground.insert(it, sprite);
	}
	else
	{
		auto it = mForeground.begin();
		for (; it != mForeground.end(); ++it)
		{
			if (order < (*it)->GetDrawOrder())
			{
				break;
			}
		}
		mForeground.insert(it, sprite);
	}
}

void Renderer::AddMesh(MeshRenderer* mesh)
{
	mMeshes.emplace_back(mesh);
}

void Renderer::AddSkinnedMesh(SkinnedMeshRenderer* skinnedMesh)
{
	mSkinnedMeshes.emplace_back(skinnedMesh);
}

void Renderer::AddParticle(ParticleRenderer* particle)
{
	mParticles.emplace_back(particle);
}

void Renderer::AddMeshParticle(MeshParticleRenderer* meshParticle)
{
	mMeshParticles.emplace_back(meshParticle);
}

// Remove
void Renderer::RemoveSprite(SpriteRenderer* sprite)
{
	auto it = std::find(mBackground.begin(), mBackground.end(), sprite);
	if (it != mBackground.end())
	{
		mBackground.erase(it);
		return;
	}
	it = std::find(mForeground.begin(), mForeground.end(), sprite);
	if (it != mForeground.end())
	{
		mForeground.erase(it);
	}
}

void Renderer::RemoveMesh(MeshRenderer* mesh)
{
	auto it = std::find(mMeshes.begin(), mMeshes.end(), mesh);
	if (it != mMeshes.end())
	{
		mMeshes.erase(it);
	}
}

void Renderer::RemoveSkinnedMesh(SkinnedMeshRenderer* skinnedMesh)
{
	auto it = std::find(mSkinnedMeshes.begin(), mSkinnedMeshes.end(), skinnedMesh);
	if (it != mSkinnedMeshes.end())
	{
		mSkinnedMeshes.erase(it);
	}
}

void Renderer::RemoveParticle(ParticleRenderer* particle)
{
	auto it = std::find(mParticles.begin(), mParticles.end(), particle);
	if (it != mParticles.end())
	{
		mParticles.erase(it);
	}
}

void Renderer::RemoveMeshParticle(MeshParticleRenderer* meshParticle)
{
	auto it = std::find(mMeshParticles.begin(), mMeshParticles.end(), meshParticle);
	if (it != mMeshParticles.end())
	{
		mMeshParticles.erase(it);
	}
}

void Renderer::LoadFile(const nlohmann::json& json)
{
	auto texs = json["Textures"];
	for (uint32_t i = 0; i < texs.size(); ++i)
	{
		if (texs[i].is_string())
		{
			GetTexture(texs[i].get<std::string>());
		}
	}

	auto models = json["Model"];
	for (uint32_t i = 0; i < models.size(); ++i)
	{
		if (models[i].is_string())
		{
			GetModel(models[i].get<std::string>());
		}
	}

	auto anims = json["Animations"];
	for (uint32_t i = 0; i < anims.size(); ++i)
	{
		if (anims[i].is_string())
		{
			auto name = anims[i].get<std::string>();
			auto pos = name.rfind('/');
			ModelLoader::LoadAnimation(name.substr(0, pos));
		}
	}
}

void Renderer::SaveFile(nlohmann::json& json)
{
	auto& texs = json["Textures"];
	for (auto tex : mTextures.GetResources())
	{
		texs.push_back(tex.second->GetFilePath());
	}

	auto& models = json["Model"];
	for (auto model : mModels.GetResources())
	{
		models.push_back(model.second->GetName());
	}

	auto& anims = json["Animations"];
	for (auto anim : mAnimations.GetResources())
	{
		anims.push_back(anim.second->GetName());
	}
}

// ==================================================
// 開発用
// ==================================================

void Renderer::UpdateForDev()
{
	ImGui::Begin("Renderer", nullptr, ImGuiWindowFlags_NoMove);
	mLightManager->UpdateForDev();
	// ガウシアンブラー
	ImGui::Checkbox("Is Gaussian", &mIsGaussianBlur);
	ImGui::DragFloat("Power", &mGaussianPower, 0.01f, 0.01f, 100.0f);
	ImGui::End();

	// ==================================================
	// アセットウィンドウ
	// ==================================================
	auto fileTex = GetTexture("Assets/Texture/File.png");
	auto size = ImVec2(82.0f / 2.0f, 111.0f / 2.0f);

	// テクスチャ
	ImGui::Begin("Texture", nullptr, ImGuiWindowFlags_NoMove);
	auto currWndSize = ImGui::GetWindowSize();
	float currWidth = 0.0f;
	uint32_t i = 0;
	for (auto& texture : mTextures.GetResources())
	{
		Texture* t = texture.second.get();
		if (t)
		{
			currWidth += 60.0f;
			if (i > 0 && currWidth < currWndSize.x)
			{
				ImGui::SameLine(0.0f, 20.0f);
			}
			else
			{
				currWidth = 60.0f;
			}
			// Group
			ImGui::BeginGroup();
			ImGui::Image((void*)(intptr_t)t->GetDescHandle()->mGpuHandle.ptr, ImVec2(40.0f, 40.0f));
			auto texName = Helper::ExtractFileName(t->GetFilePath());
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))// ドラッグ
			{
				ImGui::SetDragDropPayload("TEXTURE_PAYLOAD", &t, sizeof(t));
				ImGui::Text(texName.c_str());
				ImGui::EndDragDropSource();
			}
			// 文字列が長い
			if (texName.length() >= 7)
			{
				texName[4] = texName[5] = texName[6] = '.';
				texName[7] = '\0';
			}
			ImGui::Text(texName.c_str());
			ImGui::EndGroup();
			++i;
		}
	}
	ImGui::End();

	// モデル
	currWidth = 0.0f;
	ImGui::Begin("Model", nullptr, ImGuiWindowFlags_NoMove);
	i = 0;
	for (auto& model : mModels.GetResources())
	{
		Model* m = model.second.get();
		if (m)
		{
			currWidth += size.x + 20.0f;
			if (i > 0 && currWidth < currWndSize.x)
			{
				ImGui::SameLine(0.0f, 20.0f);
			}
			else
			{
				currWidth = 60.0f;
			}
			// Group
			ImGui::BeginGroup();
			ImGui::Image((void*)(intptr_t)fileTex->GetDescHandle()->mGpuHandle.ptr, size);
			auto modelName = m->GetName();
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))// ドラッグ
			{
				ImGui::SetDragDropPayload("MODEL_PAYLOAD", &m, sizeof(m));
				ImGui::Text(modelName.c_str());
				ImGui::EndDragDropSource();
			}
			if (modelName.length() >= 7)
			{
				modelName[4] = modelName[5] = modelName[6] = '.';
				modelName[7] = '\0';
			}
			ImGui::Text(modelName.c_str());
			ImGui::EndGroup();
			++i;
		}
	}
	ImGui::End();

	// モデル
	currWidth = 0.0f;
	ImGui::Begin("Animation" , nullptr, ImGuiWindowFlags_NoMove);
	ImGui::InputText("Name", &mAnimName);
	if (ImGui::Button("Create"))
	{
		/*mSceneNames.push_back(
			std::format("Scene {}", mSceneNames.size()));*/// デフォルト名
		ModelLoader::LoadAnimation(mAnimName);
	}

	i = 0;
	for (auto& animation : mAnimations.GetResources())
	{
		Animation* anim = animation.second.get();
		if (anim)
		{
			currWidth += size.x + 20.0f;
			if (i > 0 && currWidth < currWndSize.x)
			{
				ImGui::SameLine(0.0f, 20.0f);
			}
			else
			{
				currWidth = 60.0f;
			}
			// Group
			ImGui::BeginGroup();
			ImGui::Image((void*)(intptr_t)fileTex->GetDescHandle()->mGpuHandle.ptr, size);
			auto animName = anim->GetName();
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))// ドラッグ
			{
				ImGui::SetDragDropPayload("ANIM_PAYLOAD", &anim, sizeof(anim));
				ImGui::Text(animName.c_str());
				ImGui::EndDragDropSource();
			}
			if (animName.length() >= 7)
			{
				animName[4] = animName[5] = animName[6] = '.';
				animName[7] = '\0';
			}
			ImGui::Text(animName.c_str());
			ImGui::EndGroup();
			++i;
		}
	}
	ImGui::End();
}

void Renderer::RenderDebug(ID3D12GraphicsCommandList* cmdList)
{
	if (Editor::gEditorState == Editor::EditorState::kEdit || !Editor::mIsMaximum)//
	{
		Primitive* prim = GetPrimitive();
		prim->PreRendering(cmdList);
		mPrimitive->DrawGrid();
		gEngine->GetSceneManager()->RenderForDev(prim);
		prim->PostRendering();
	}
}

// ==================================================
// ヘルパー関数
// ==================================================

void Renderer::SetGameCamera(CameraComponent* camera)
{
	if (!camera)
	{
		return;
	}
	if (mGameCamera)
	{
		mGameCamera->SetIsMain(false);
	}
	mGameCamera = camera;
	mGameCamera->SetIsMain(true);
}


void Renderer::Render(ID3D12GraphicsCommandList* cmdList)
{
	PreRendering(cmdList);
	RenderScene(cmdList);
	if (Editor::IsEditor())
	{
		RenderDebug(cmdList);
	}
	PostRendering(cmdList);
}
