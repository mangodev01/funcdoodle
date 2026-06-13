#include "ToolManager.h"

#include <algorithm>

#include "Asset/LoadedAssets.h"
#include "Core/App.h"
#include "Keybinds/Keybinds.h"
#include "Tool/Tool.h"

#include "Conf/FuncPCH.h"

#include "UI/Gui.h"
#include "imgui.h"

namespace FuncDoodle {
	ToolManager::ToolManager(KeybindsRegistry& keybinds)
		: m_Keybinds(keybinds) {}

	ToolManager::~ToolManager() = default;

	void ToolManager::Buttons() {
		float avail = ImGui::GetContentRegionAvail().x;
		float btnSize = 36.0f;
		float spacing = ImGui::GetStyle().ItemSpacing.x;

		auto columns = (int)(avail / (btnSize + spacing));
		columns = std::max(columns, 1);

		int i = 0;
		for (ToolType t : ToolTypes) {
			// ImGui::PushID(i);

			uint32_t btnTexId = ToolTexID(t);
			if (ImGui::ImageButton(ToolTypeName(t),
					(ImTextureID)(intptr_t)btnTexId, ImVec2(32, 32),
					ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0),
					m_SelectedTool == t ? ImVec4(1, 1, 1, 1)
										: ImVec4(0, 0, 0, 1))) {
				m_SelectedTool = t;
			}

			if ((i + 1) % columns != 0 && i != (int)ToolTypes.size() - 1)
				ImGui::SameLine();

			Tooltips(t, m_Keybinds);

			// ImGui::PopID();
			i++;
		}
	}

	void ToolManager::RegisterKeybinds() {
		m_Keybinds.Register("decrease_tool_size", "Decrease tool size",
			"Decrease the current tool size",
			{false, false, false, ImGuiKey_Semicolon});
		m_Keybinds.Register("increase_tool_size", "Increase tool size",
			"Increase the current tool size",
			{false, false, false, ImGuiKey_Apostrophe});

		m_Keybinds.Register("swap_colors", "Swap",
			"Swap between first/secondary colors",
			{false, false, false, ImGuiKey_V});
	}

	void ToolManager::Widgets() {
		bool showColorPredicate = m_SelectedTool != ToolType::Eraser &&
								  m_SelectedTool != ToolType::Select;

		bool showSizePredicate = m_SelectedTool == ToolType::Pencil ||
								 m_SelectedTool == ToolType::Eraser;

		bool showTolerancePredicate = m_SelectedTool == ToolType::Bucket;

		if (showSizePredicate) {
			ImGui::SliderInt("##size", &m_Size, 1, 100, "%dpx");

			if (showColorPredicate)
				ImGui::SameLine();
		}

		if (showTolerancePredicate) {
			static constexpr uint8_t minTolerance = 1;
			static constexpr uint8_t maxTolerance = 255;

			ImGui::SliderScalar("##tolerance", ImGuiDataType_U8, &m_Tolerance,
				&minTolerance, &maxTolerance);

			if (showColorPredicate)
				ImGui::SameLine();
		}

		if (m_Keybinds.Get("decrease_tool_size").IsPressed()) {
			m_Size--;
		} else if (m_Keybinds.Get("increase_tool_size").IsPressed()) {
			m_Size++;
		}

		m_Size = std::max(m_Size, 1);

		float* col = GetCol();
		float* secondaryCol = GetSecCol();

		if (showColorPredicate) {
			auto drawColorButton = [](const char* id, const float* color,
									   const char* popupName) {
				if (ImGui::ColorButton(
						id, ImVec4(color[0], color[1], color[2], 1.0f))) {
					ImGui::OpenPopup(popupName);
				}
			};

			const bool firstIsPrimary = (m_CurrentColor == ColorChoice::First);

			drawColorButton(
				firstIsPrimary ? "##first_color" : "##secondary_color",
				firstIsPrimary ? col : secondaryCol,
				firstIsPrimary ? "ColorPickerFirst" : "ColorPickerSecondary");

			ImGui::SameLine();

			float prevCurX = ImGui::GetCursorPosX();

			drawColorButton(
				firstIsPrimary ? "##secondary_color" : "##first_color",
				firstIsPrimary ? secondaryCol : col,
				firstIsPrimary ? "ColorPickerSecondary" : "ColorPickerFirst");

			ImGui::SetCursorPosX(prevCurX - 23);

			if (ImGui::ImageButton("##swap", (ImTextureID)(intptr_t)s_SwapTexId,
					ImVec2(20, 20)) ||
				m_Keybinds.Get("swap_colors").IsPressed()) {
				if (m_CurrentColor == ColorChoice::Secondary) {
					m_CurrentColor = ColorChoice::First;
				} else if (m_CurrentColor == ColorChoice::First) {
					m_CurrentColor = ColorChoice::Secondary;
				}
			}
		}

		if (ImGui::BeginPopup("ColorPickerFirst")) {
			ImGui::PushItemWidth(250);
			ImGui::ColorPicker3(
				"##color", col, ImGuiColorEditFlags_NoSidePreview);
			ImGui::PopItemWidth();

			ImGui::EndPopup();
		}

		if (ImGui::BeginPopup("ColorPickerSecondary")) {
			ImGui::PushItemWidth(250);
			ImGui::ColorPicker3("##secondary_color", secondaryCol,
				ImGuiColorEditFlags_NoSidePreview);
			ImGui::PopItemWidth();

			ImGui::EndPopup();
		}
	}

	void ToolManager::RenderTools() {
		ImGui::Begin("Tools");

		ToolKeybinds(&m_SelectedTool, m_Keybinds);
		Buttons();

		ImGui::Separator();
		Widgets();

		Cursor();

		ImGui::End();
	}

	void ToolManager::UpdateCursor() {
		Application* app = Application::Get();
		ImVec2 pos = ImGui::GetMousePos();
		// sorry, selection tool, you just look pretty weird as a cursor  :(
		if (!Application::IsPosInFrame(pos) ||
			m_SelectedTool == ToolType::Select) {
			app->ShowCursor();
		} else {
			app->HideCursor();
		}
	}

	void ToolManager::Cursor() {
		Application* app = Application::Get();

		if (!app->GetWindow()->GetCursorHidden())
			return;

		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

		ImDrawList* dl = ImGui::GetForegroundDrawList();
		auto scale =
			(float)app->GetManager()->GetFrameRenderer()->GetCtx()->PixelScale;
		float offsetFactor = scale / 2.0f;

		ImVec2 posWithoutOff(ImGui::GetMousePos());
		ImVec2 pos(posWithoutOff.x, posWithoutOff.y - 32 + (scale / 2.0f));

		ImGui::SetMouseCursor(ImGuiMouseCursor_None);
		float off = 1.5f;

		// same as (ImTextureID)(intptr_t)ToolTexID(m_SelectedTool);
		ImTextureID texID = (intptr_t)ToolTexID(m_SelectedTool);

		// outline
		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				if (dx == 0 && dy == 0)
					continue;
				dl->AddImage(texID,
					ImVec2(pos.x + (dx * off), pos.y + (dy * off)),
					ImVec2(pos.x + 32 + (dx * off), pos.y + 32 + (dy * off)),
					ImVec2(0, 0), ImVec2(1, 1), IM_COL32_BLACK);
			}
		}

		// main image
		dl->AddImage(texID, pos, ImVec2(pos.x + 32, pos.y + 32));
	}
}  // namespace FuncDoodle
