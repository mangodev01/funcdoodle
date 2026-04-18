#include "ToolManager.h"

#include "App.h"
#include "Keybinds.h"
#include "Tool.h"

#include "MacroUtils.h"
#include "imgui.h"

namespace FuncDoodle {
	ToolManager::ToolManager(KeybindsRegistry& keybinds)
		: m_SelectedTool(ToolType::Pencil), m_Keybinds(keybinds) {}

	ToolManager::~ToolManager() {}

	void ToolManager::Buttons() {
		for (ToolType t : ToolTypes) {
			uint32_t btnTexId = ToolTexID(t);
			if (ImGui::ImageButton(ToolTypeName(t),
					(ImTextureID)(intptr_t)btnTexId, ImVec2(32, 32),
					ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0),
					m_SelectedTool == t ? ImVec4(1, 1, 1, 1)
										: ImVec4(0, 0, 0, 1))) {
				m_SelectedTool = t;
			}

			Tooltips(t);
		}
	}

	void ToolManager::RegisterKeybinds() {
		m_Keybinds.Register(
			"decrease_tool_size", {false, false, false, ImGuiKey_Semicolon});
		m_Keybinds.Register(
			"increase_tool_size", {false, false, false, ImGuiKey_Apostrophe});
	}

	void ToolManager::Widgets() {
		if (m_SelectedTool == ToolType::Pencil ||
			m_SelectedTool == ToolType::Eraser) {
			ImGui::SliderInt("##Size", &m_Size, 1, 100, "%dpx");
		}

		if (m_SelectedTool != ToolType::Eraser &&
			m_SelectedTool != ToolType::Select) {
			ImGui::PushItemWidth(250);
			ImGui::ColorPicker3(
				"Color", m_Col, ImGuiColorEditFlags_NoSidePreview);
			ImGui::PopItemWidth();
		}

		if (m_Size < 1)
			m_Size = 1;

		if (m_Keybinds.Get("decrease_tool_size").IsPressed()) {
			m_Size--;
		} else if (m_Keybinds.Get("increase_tool_size").IsPressed()) {
			m_Size++;
		}
	}

	void ToolManager::RenderTools() {
		ImGui::Begin("Tools");

		ToolKeybinds(&m_SelectedTool, m_Keybinds);
		Buttons();
		Widgets();

		Cursor();

		ImGui::End();
	}

	void ToolManager::UpdateCursor() {
		Application* app = Application::Get();
		ImVec2 pos = ImGui::GetMousePos();
		// sorry, selection tool, you just look pretty weird as a cursor  :(
		if (!app->IsPosInFrame(pos) || m_SelectedTool == ToolType::Select) {
			app->ShowCursor();
		} else {
			app->HideCursor();
		}
	}

	void ToolManager::Cursor() {
		Application* app = Application::Get();

		if (!app->GetWindow()->GetCursorHidden()) return;

		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

		ImDrawList* dl = ImGui::GetForegroundDrawList();
		int scale = app->GetManager()->GetFrameRenderer()->GetCtx()->PixelScale;
		int offsetFactor = scale / 2;

		ImVec2 cursorOff(-offsetFactor, -offsetFactor);
		ImVec2 posWithoutOff(ImGui::GetMousePos());
		ImVec2 pos = ImVec2(posWithoutOff.x + cursorOff.x, posWithoutOff.y + cursorOff.y);

		ImGui::SetMouseCursor(ImGuiMouseCursor_None);
		float off = 1.0f;
		// same as (ImTextureID)(intptr_t)ToolTexID(m_SelectedTool);
		ImTextureID texID = (intptr_t)ToolTexID(m_SelectedTool);

		// outline
		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				if (dx == 0 && dy == 0) continue;
				dl->AddImage(texID, 
						ImVec2(pos.x + dx*off, pos.y + dy*off),
						ImVec2(pos.x + 32 + dx*off, pos.y + 32 + dy*off),
						ImVec2(0,0), ImVec2(1,1), IM_COL32_BLACK);
			}
		}

		// main image
		dl->AddImage(texID, pos, ImVec2(pos.x + 32, pos.y + 32));
	}
}  // namespace FuncDoodle
