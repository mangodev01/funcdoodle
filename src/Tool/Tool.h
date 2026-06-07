/**
 * @file Tool.h
 * @brief Defines editor tools, tool utilities, and tool-related keybinding
 * logic.
 *
 * This file contains:
 * - ToolType enum defining all available editor tools
 * - Utility functions for mapping tools to names, textures, and tooltips
 * - Keybinding registration and input handling for tool switching
 *
 * It provides a lightweight functional layer for tool selection and UI
 * integration, primarily used by ToolManager and editor input systems.
 *
 * @note Tool utilities are implemented as constexpr/static helpers for
 * simplicity.
 */

#pragma once

#include "Project/Frame.h"

#include "Asset/LoadedAssets.h"
#include "Keybinds/Keybinds.h"
#include "UI/Gui.h"

#include "Conf/FuncPCH.h"

#include <functional>

namespace FuncDoodle {
	/**
	 * @enum ToolType
	 * @brief All available tools in the editor.
	 */
	enum class ToolType {
		Pencil,
		Eraser,
		Bucket,
		Picker,
		Select,
	};

	// i really don't like that i have to do this, but i do, because as it is
	// right now, there is no consistent way to iterate over an enum class,
	// without adding Start and End members which aren't that clean imo.
	/** @brief Ordered list of tools used for iteration and UI rendering. */
	constexpr std::array<ToolType, 5> ToolTypes{
		ToolType::Pencil,
		ToolType::Eraser,
		ToolType::Bucket,
		ToolType::Picker,
		ToolType::Select,
	};

	// don't like this either, but i have to do it..
	// actually i COULD use a constexpr std::unordered_map here but idk
	/** @brief Returns the user-facing name of a tool type. */
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

	// also could use a constexpr std::unordered_map here but idk
	// rather than having 2 unordered_maps it would be better to have one with a
	// struct that holds both the name and the texid for each tool.
	/** @brief Returns the toolbar texture ID associated with a tool. */
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
	// (could ALSO use an unordered_map)
	// (this is old code, ok?)
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

	// yea i really have nothing to say about this method idk why i put a
	// comment here
	static void ToolKeybindsRegister(KeybindsRegistry& keybinds) {
		keybinds.Register("pencil", {false, false, false, ImGuiKey_1});
		keybinds.Register("eraser", {false, false, false, ImGuiKey_2});
		keybinds.Register("bucket", {false, false, false, ImGuiKey_3});
		keybinds.Register("picker", {false, false, false, ImGuiKey_4});
		keybinds.Register("select", {false, false, false, ImGuiKey_5});
	}

	// perhaps use some sorta unordered_map too..?
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
