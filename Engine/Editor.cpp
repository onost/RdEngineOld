#include "Editor.h"
#include "Actor/Actor.h"
#include "Core/GraphicsEngine.h"
#include "Helper/MyAssert.h"
#include "Window.h"
#include "Graphics/Texture.h"
#include "Graphics/Renderer.h"
#include "Input/InputSystem.h"
#include "Scene/SceneManager.h"
#include "RdEngine.h"

namespace Console
{
	std::vector<std::string> gMessages;
	void Log(const std::string& message)
	{
		gMessages.emplace_back(message);
	}
	// コンソール画面を表示
	void ShowConsole()
	{
		if (Editor::IsShowEditor())
		{
			ImGui::Begin("Console", nullptr, ImGuiWindowFlags_NoMove);
			for (auto& message : gMessages)
			{
				ImGui::Text(message.c_str());
			}
			ImGui::End();
		}
	}
}

namespace Editor
{
	//EngineState gEngineState;
	bool gIsGame = false;
	EditorState gEditorState;
	bool mIsMaximum = false;
	std::unique_ptr<Texture> mStartTex;
	std::unique_ptr<Texture> mStopTex;
	std::unique_ptr<Texture> mPauseTex;
	std::unique_ptr<Texture> mStepTex;
	std::shared_ptr<DebugCamera> mDebugCamera;
	bool mIsDebugCamera = true;

	void Initialize(Window* window)
	{
		MY_ASSERT(window);

		if (!ImGui::CreateContext())
		{
			MY_ASSERT(false);
		}
		// フォント
		std::string fontPath = "Assets/Font/Roboto-Regular.ttf";
		float fontSize = 14.0f;
		auto& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF(fontPath.c_str(), fontSize, nullptr, io.Fonts->GetGlyphRangesJapanese());
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		if (!ImGui_ImplWin32_Init(window->GetHWnd()))
		{
			MY_ASSERT(false);
		}
		auto& srvHeap = gGraphicsEngine->GetSrvHeap();
		auto handle = srvHeap.Alloc();
		if (!ImGui_ImplDX12_Init(
			gGraphicsEngine->GetDevice(), 2, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
			srvHeap.GetHeap(), handle->mCpuHandle, handle->mGpuHandle))
		{
			MY_ASSERT(false);
		}

		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 2.0f;
		style.FramePadding = ImVec2(4.0f, 2.0f);
		style.ItemSpacing = ImVec2(8.0f, 5.0f);
		style.WindowBorderSize = 0.0f;
		style.GrabRounding = 2.0f;
		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.5f, 0.43f, 0.43f, 0.5f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.125f, 0.125f, 0.125f, 1.0f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.375f, 0.375f, 0.375f, 1.0f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.0f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.04f, 0.04f, 0.04f, 1.0f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.125f, 0.125f, 0.125f, 1.0f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.0f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5625f, 0.5625f, 0.5625f, 1.0f);
		colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.375f, 0.375f, 0.375f, 1.0f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.375f, 0.375f, 0.375f, 1.0f);
		colors[ImGuiCol_Button] = ImVec4(0.375f, 0.375f, 0.375f, 0.5f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.375f, 0.375f, 0.375f, 1.0f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.625f, 0.625f, 0.625f, 1.0f);
		colors[ImGuiCol_Header] = ImVec4(1.0f, 0.5f, 0.5f, 0.25f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(1.0f, 0.5f, 0.5f, 0.75f);
		colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
		colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.5f, 0.43f, 0.43f, 0.75f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.5f, 0.43f, 0.43f, 1.0f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.0f, 0.5f, 0.5f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.0f, 0.5f, 0.5f, 0.75f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
		colors[ImGuiCol_Tab] = ImVec4(1.0f, 0.5f, 0.5f, 0.25f);
		colors[ImGuiCol_TabHovered] = ImVec4(1.0f, 0.5f, 0.5f, 0.75f);
		colors[ImGuiCol_TabActive] = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.0f, 0.0f, 0.0f, 0.52f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);

		//Editor::gEngineState = Editor::EngineState::kEditor;
		//mState = State::kGame;
		Editor::gEditorState = Editor::EditorState::kEdit;

		mStartTex = std::make_unique<Texture>();
		mStopTex = std::make_unique<Texture>();
		mPauseTex = std::make_unique<Texture>();
		mStepTex = std::make_unique<Texture>();

		mStartTex->Create("Assets/Texture/Start.png");
		mStopTex->Create("Assets/Texture/Stop.png");
		mPauseTex->Create("Assets/Texture/Pause.png");
		mStepTex->Create("Assets/Texture/Step.png");
		// デバッグカメラ
		mDebugCamera = std::make_unique<DebugCamera>();
	}

	void Terminate()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}


	void Input(const InputSystem::State& input)
	{
		if (!gIsGame && mIsDebugCamera)
		{
			mDebugCamera->Input(input);
		}
	}

	void Update(float deltaTime)
	{
		if (!gIsGame && mIsDebugCamera)
		{
			mDebugCamera->Update(deltaTime);
		}
	}


	void PreProcess()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void PostProcess()
	{
		ImGui::Render();
	}

	void Draw(ID3D12GraphicsCommandList* cmdList)
	{
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);
	}

	// アクターツリーを表示
	void ActorTree(const std::vector<Actor*>& actors, Actor*& actorForDev)
	{
		for (uint32_t i = 0; i < actors.size(); ++i)
		{
			auto& actor = actors[i];
			auto& children = actor->GetChildren();
			if (children.size() > 0)// 子がいる
			{
				ImGuiTreeNodeFlags flags =
					ImGuiTreeNodeFlags_OpenOnArrow |
					ImGuiTreeNodeFlags_OpenOnDoubleClick |
					ImGuiTreeNodeFlags_SpanAvailWidth;
				if (actorForDev == actor)
				{
					flags |= ImGuiTreeNodeFlags_Selected;
				}
				auto name = actor->GetName();
				auto isOpen = ImGui::TreeNodeEx((void*)(intptr_t)i, flags, name.c_str());
				// ドラッグ
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					ImGui::SetDragDropPayload("ACTOR_PAYLOAD", &actor, sizeof(actor));
					ImGui::Text(name.c_str());
					ImGui::EndDragDropSource();
				}
				// ドロップ
				if (ImGui::BeginDragDropTarget())
				{
					if (auto payload = ImGui::AcceptDragDropPayload("ACTOR_PAYLOAD"))
					{
						// ドロップされたら親
						auto a = (Actor**)(payload->Data);
						(*a)->SetParent(actor);
					}
					ImGui::EndDragDropTarget();
				}
				// クリックされた
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				{
					actorForDev = actor;
				}
				if (isOpen)
				{
					// 子アクター（再帰）
					Editor::ActorTree(actor->GetChildren(), actorForDev);
					ImGui::TreePop();
				}
			}
			else// 子がいない
			{
				ImGuiTreeNodeFlags flags =
					ImGuiTreeNodeFlags_Leaf |
					ImGuiTreeNodeFlags_NoTreePushOnOpen;
				if (actorForDev == actor)
				{
					flags |= ImGuiTreeNodeFlags_Selected;
				}
				auto name = actor->GetName();
				ImGui::TreeNodeEx((void*)(intptr_t)i, flags, name.c_str());
				// ドラッグ
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					ImGui::SetDragDropPayload("ACTOR_PAYLOAD", &actor, sizeof(actor));
					ImGui::Text(name.c_str());
					ImGui::EndDragDropSource();
				}
				// ドロップ
				if (ImGui::BeginDragDropTarget())
				{
					if (auto payload = ImGui::AcceptDragDropPayload("ACTOR_PAYLOAD"))
					{
						// ドロップされたら親
						auto a = (Actor**)(payload->Data);
						(*a)->SetParent(actor);
					}
					ImGui::EndDragDropTarget();
				}
				// クリックされた
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				{
					actorForDev = actor;
				}
			}
		}
	}

	void ShowEditor(RdEngine* engine)
	{
		if (Editor::gEditorState == Editor::EditorState::kEdit || !Editor::mIsMaximum)//
		{
			ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoMove);

			ShowState(engine->GetRenderer().get(), engine->GetSceneManager().get());

			// デバッグカメラ
			//bool isDebugCamera = renderer->GetIsDebugCamera();
			if (ImGui::Checkbox("Is Debug Camera", &mIsDebugCamera))
			{
				//renderer->SetIsDebugCamera(isDebugCamera);
			}
			ImGui::Checkbox("Is Maximum", &mIsMaximum);

			ImGui::End();

			engine->GetRenderer()->UpdateForDev();
			engine->GetSceneManager()->UpdateForDev();


			Console::ShowConsole();
		}
		else
		{
			ImGui::Begin("State");
			Editor::ShowState(engine->GetRenderer().get(), engine->GetSceneManager().get());
			ImGui::End();
		}
	}

	void ShowState(Renderer*, SceneManager* sceneManager)
	{
		if (Editor::gEditorState == Editor::EditorState::kStep)
		{
			Editor::gEditorState = Editor::EditorState::kStop;
		}
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(-1.0f, -1.0f));
		switch (Editor::gEditorState)
		{
		case Editor::EditorState::kEdit:
			if (ImGui::ImageButton((void*)(intptr_t)mStartTex->GetDescHandle()->mGpuHandle.ptr, ImVec2(24.0f, 24.0f)))
			{
				Editor::gEditorState = Editor::EditorState::kPlay;
				sceneManager->InitActor();
				mIsDebugCamera = false;
			}
			ImGui::SameLine();
			if (ImGui::ImageButton((void*)(intptr_t)mPauseTex->GetDescHandle()->mGpuHandle.ptr, ImVec2(24.0f, 24.0f)))
			{
				Editor::gEditorState = Editor::EditorState::kStop;
				sceneManager->InitActor();
				mIsDebugCamera = false;
			}
			ImGui::SameLine();
			if (ImGui::ImageButton((void*)(intptr_t)mStepTex->GetDescHandle()->mGpuHandle.ptr, ImVec2(24.0f, 24.0f)))
			{
				Editor::gEditorState = Editor::EditorState::kStep;
				sceneManager->InitActor();
				mIsDebugCamera = false;
			}
			break;
		case Editor::EditorState::kPlay:
			if (ImGui::ImageButton((void*)(intptr_t)mStopTex->GetDescHandle()->mGpuHandle.ptr, ImVec2(24.0f, 24.0f)))
			{
				Editor::gEditorState = Editor::EditorState::kEdit;
				sceneManager->Reset();
				mIsDebugCamera = true;
			}
			ImGui::SameLine();
			if (ImGui::ImageButton((void*)(intptr_t)mPauseTex->GetDescHandle()->mGpuHandle.ptr, ImVec2(24.0f, 24.0f)))
			{
				Editor::gEditorState = Editor::EditorState::kStop;
			}
			ImGui::SameLine();
			if (ImGui::ImageButton((void*)(intptr_t)mStepTex->GetDescHandle()->mGpuHandle.ptr, ImVec2(24.0f, 24.0f)))
			{
				Editor::gEditorState = Editor::EditorState::kStep;
			}
			break;
		case Editor::EditorState::kStop:
			if (ImGui::ImageButton((void*)(intptr_t)mStopTex->GetDescHandle()->mGpuHandle.ptr, ImVec2(24.0f, 24.0f)))
			{
				Editor::gEditorState = Editor::EditorState::kEdit;
				sceneManager->Reset();
				mIsDebugCamera = true;
			}
			ImGui::SameLine();
			if (ImGui::ImageButton((void*)(intptr_t)mStartTex->GetDescHandle()->mGpuHandle.ptr, ImVec2(24.0f, 24.0f)))
			{
				Editor::gEditorState = Editor::EditorState::kPlay;
			}
			ImGui::SameLine();
			if (ImGui::ImageButton((void*)(intptr_t)mStepTex->GetDescHandle()->mGpuHandle.ptr, ImVec2(24.0f, 24.0f)))
			{
				Editor::gEditorState = Editor::EditorState::kStep;
			}
			break;
		}
		ImGui::PopStyleVar();
	}
}
