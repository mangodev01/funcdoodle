#include "ToolManager.h"

#include "Keybinds.h"
#include "Tool.h"

#include "MacroUtils.h"

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
			ImGui::SameLine();
			ImGui::InputInt("##SizeText", &m_Size, 1, 100, 0);
		}

		if (m_SelectedTool != ToolType::Eraser && m_SelectedTool != ToolType::Select) {
			ImGui::ColorPicker3("Col", m_Col);
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

		ImGui::End();
	}
}  // namespace FuncDoodle
