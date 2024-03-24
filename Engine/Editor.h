#pragma once
#include "Actor/DebugCamera.h"
#include "Input/InputSystem.h"
#include <d3d12.h>
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx12.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_stdlib.h>
#include <ImGui/ImGuizmo.h>
#include <magic_enum/magic_enum.hpp>
#include <string>
#include <vector>

class Actor;
class Window;
class Renderer;
class SceneManager;
class RdEngine;

// コンソール
namespace Console
{
	void Log(const std::string& message);
	// コンソール画面を表示
	void ShowConsole();
}

namespace Editor
{
	// エンジンの状態
	//enum class EngineState
	//{
	//	kEditor,// エディタ
	//	kGame// ゲーム
	//};

	// エディタの状態
	enum class EditorState
	{
		kEdit,
		kPlay,
		kStop,
		kStep
	};

	// エンジンの状態
	//extern EngineState gEngineState;
	extern bool gIsGame;
	// エディタの状態
	extern EditorState gEditorState;

	inline bool IsEditor() { return !gIsGame; }
	inline bool IsUpdate() { return gIsGame || (gEditorState == EditorState::kPlay) || (gEditorState == EditorState::kStep); }


	// デバッグ用カメラ
	extern std::shared_ptr<DebugCamera> mDebugCamera;
	extern bool mIsDebugCamera;

	extern bool mIsMaximum;

	inline bool IsShowEditor()
	{
		return
			IsEditor() &&
			(gEditorState == EditorState::kEdit || !mIsMaximum);
	}

	void Initialize(Window* window);
	void Terminate();

	void Input(const InputSystem::State& input);
	void Update(float deltaTime);

	void PreProcess();
	void PostProcess();
	void Draw(ID3D12GraphicsCommandList* cmdList);

	template <class T>
	bool Combo(const char* name, T& curr)
	{
		bool isChanged = false;
		if (ImGui::BeginCombo(name, magic_enum::enum_name(curr).data()))
		{
			for (size_t i = 0; i < magic_enum::enum_count<T>(); ++i)
			{
				bool isSelected = false;
				if (ImGui::Selectable(magic_enum::enum_name(T(i)).data(), isSelected))
				{
					curr = T(i);
					isChanged = true;
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		return isChanged;
	}

	// アクターツリーを表示
	void ActorTree(const std::vector<Actor*>& actors, Actor*& actorForDev);

	// 開発用
	void ShowEditor(RdEngine* engine);
	void ShowState(Renderer* renderer,SceneManager*sceneManager);
}
