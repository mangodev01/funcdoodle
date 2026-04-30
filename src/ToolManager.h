/**
 * @file ToolManager.h
 * @brief Manages tools, tool state, and tool-related UI in FuncDoodle.
 *
 * This file defines ToolManager, responsible for:
 * - Tracking the currently selected drawing tool
 * - Managing tool parameters (color, brush size, etc.)
 * - Rendering tool UI elements
 * - Handling tool-related keybinds and input updates
 * - Updating cursor behavior based on active tool
 *
 * ToolManager acts as the central bridge between user input,
 * UI rendering, and editing tools such as pencil, fill, erase, etc.
 */

#pragma once

/**
 * @file ToolManager.h
 * @brief Manages tools, tool state, and tool-related UI in FuncDoodle.
 *
 * This file defines ToolManager, responsible for:
 * - Tool selection and registration
 * - Tool rendering in the UI
 * - Cursor updates based on selected tool
 * - Tool keybind registration
 */

#include "Frame.h"

#include "AssetLoader.h"

#include "Keybinds.h"
#include "MacroUtils.h"

#include "Tool.h"

namespace FuncDoodle {
	/**
	 * @class ToolManager
	 * @brief Manages tools in FuncDoodle.
	 *
	 * Does various stuff like rendering tool buttons, handling tool keybinds
	 */
	class ToolManager {
		public:
		ToolManager(KeybindsRegistry& keybinds);
		~ToolManager();
		void RegisterKeybinds();
		void RenderTools();
		void Buttons();
		void Widgets();
		void UpdateCursor();
		void Cursor();
		const ToolType GetSelectedTool() { return m_SelectedTool; }
		const float* GetCol() { return m_Col; }
		void SetCol(struct Col col) {
			unsigned char colArr[3] = {col.r, col.g, col.b};
			for (int j = 0; j < 3; j++) {
				m_Col[j] = static_cast<float>(colArr[j]) / 255.0f;
			}
		}
		const int GetSize() const { return m_Size; }
		void SetSize(int size) { m_Size = size; }

		private:
		ToolType m_SelectedTool = ToolType::Pencil;
		float m_Col[3] = {0.0f, 0.0f, 0.0f};
		int m_Size = 1;
		KeybindsRegistry& m_Keybinds;
	};
}  // namespace FuncDoodle
