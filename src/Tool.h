#pragma once

#include "Frame.h"

#include "Keybinds.h"
#include "LoadedAssets.h"
#include "imgui.h"

#include "MacroUtils.h"

#include <functional>

namespace FuncDoodle {
	enum class ToolType {
		Pencil,
		Eraser,
		Bucket,
		Picker,
		Select,
	};
	constexpr std::array<ToolType, 5> ToolTypes{
		ToolType::Pencil,
		ToolType::Eraser,
		ToolType::Bucket,
		ToolType::Picker,
		ToolType::Select,
	};
	constexpr const char* ToolTypeName(ToolType t) {
		switch (t) {
			case ToolType::Pencil:
				return "Pencil";
			case ToolType::Eraser:
				return "Eraser";
			case ToolType::Bucket:
				return "Bucket";
			case ToolType::Picker:
				return "Picker";
			case ToolType::Select:
				return "Select";
		}
		return "Unknown";
	}
	constexpr uint32_t ToolTexID(ToolType tool) {
		switch (tool) {
			case ToolType::Pencil:
				return s_PencilTexId;
			case ToolType::Eraser:
				return s_EraserTexId;
			case ToolType::Bucket:
				return s_BucketTexId;
			case ToolType::Picker:
				return s_PickerTexId;
			case ToolType::Select:
				return s_SelectTexId;
		}
		// case ToolType::Text:
		// btnTexId = s_TextTexId;
		// break;
	}

	// hehe get it - tool, tips
	// lol
	static void Tooltips(ToolType tool) {
		switch (tool) {
			case ToolType::Pencil:
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("Pencil (1)");
					ImGui::EndTooltip();
				}
				break;
			case ToolType::Eraser:
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("Eraser (2)");
					ImGui::EndTooltip();
				}
				break;
			case ToolType::Bucket:
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("Bucket (3)");
					ImGui::EndTooltip();
				}
				break;
			case ToolType::Picker:
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("Picker (4)");
					ImGui::EndTooltip();
				}
				break;
			case ToolType::Select:
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("Select (5)");
					ImGui::EndTooltip();
				}
				break;
		}
	}

	static void ToolKeybindsRegister(KeybindsRegistry& keybinds) {
		keybinds.Register("pencil", {false, false, false, ImGuiKey_1});
		keybinds.Register("eraser", {false, false, false, ImGuiKey_2});
		keybinds.Register("bucket", {false, false, false, ImGuiKey_3});
		keybinds.Register("picker", {false, false, false, ImGuiKey_4});
		keybinds.Register("select", {false, false, false, ImGuiKey_5});
	}

	static void ToolKeybinds(ToolType* tool, KeybindsRegistry& keybinds) {
		if (keybinds.Get("pencil").IsPressed()) {
			*tool = ToolType::Pencil;
		}
		if (keybinds.Get("eraser").IsPressed()) {
			*tool = ToolType::Eraser;
		}
		if (keybinds.Get("bucket").IsPressed()) {
			*tool = ToolType::Bucket;
		}
		if (keybinds.Get("picker").IsPressed()) {
			*tool = ToolType::Picker;
		}
		if (keybinds.Get("select").IsPressed()) {
			*tool = ToolType::Select;
		}
	}
}  // namespace FuncDoodle
