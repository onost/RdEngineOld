#pragma once
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

// コンソール
namespace Console
{
	void Log(const std::string& message);
	// コンソール画面を表示
	void ShowConsole();
}

namespace Editor
{
	void Initialize(Window* window);
	void Terminate();

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
}
