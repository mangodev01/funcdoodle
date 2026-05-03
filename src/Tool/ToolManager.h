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

#include "Project/Frame.h"

#include "Asset/AssetLoader.h"

#include "Keybinds/Keybinds.h"
#include "Util/MacroUtils.h"

#include "Tool/Tool.h"

namespace FuncDoodle {
	/**
	 * @class ToolManager
	 * @brief Manages tools in FuncDoodle.
	 *
	 * Does various stuff like rendering tool buttons, handling tool keybinds
	 */
	class ToolManager {
		public:
		/**
		 * @fn ToolManager
		 * @brief Creates a tool manager bound to the application keybind
		 * registry.
		 *
		 * @param keybinds Keybind registry used for tool shortcuts.
		 */
		ToolManager(KeybindsRegistry& keybinds);
		~ToolManager();
		/**
		 * @fn RegisterKeybinds
		 * @brief Registers tool-selection keybinds.
		 */
		void RegisterKeybinds();
		/**
		 * @fn RenderTools
		 * @brief Renders the complete tool UI.
		 */
		void RenderTools();
		/**
		 * @fn Buttons
		 * @brief Renders tool-selection buttons.
		 */
		void Buttons();
		/**
		 * @fn Widgets
		 * @brief Renders tool configuration widgets.
		 */
		void Widgets();
		/**
		 * @fn UpdateCursor
		 * @brief Updates cursor state to match the active tool.
		 */
		void UpdateCursor();
		/**
		 * @fn Cursor
		 * @brief Renders or updates the tool cursor overlay.
		 */
		void Cursor();
		/**
		 * @fn GetSelectedTool
		 * @brief Returns the currently selected tool.
		 *
		 * @return Active tool type.
		 */
		ToolType GetSelectedTool() { return m_SelectedTool; }
		/**
		 * @fn GetCol
		 * @brief Returns the current tool color as normalized RGB values.
		 *
		 * @return Pointer to a 3-float RGB array.
		 */
		const float* GetCol() { return m_Col; }
		/**
		 * @fn SetCol
		 * @brief Sets the current tool color from an 8-bit RGB color.
		 *
		 * @param col New tool color.
		 */
		void SetCol(struct Col col) {
			unsigned char colArr[3] = {col.r, col.g, col.b};
			for (int j = 0; j < 3; j++) {
				m_Col[j] = static_cast<float>(colArr[j]) / 255.0f;
			}
		}
		/**
		 * @fn GetSize
		 * @brief Returns the current tool size.
		 *
		 * @return Brush or tool size.
		 */
		[[nodiscard]] int GetSize() const { return m_Size; }
		/**
		 * @fn SetSize
		 * @brief Sets the current tool size.
		 *
		 * @param size New brush or tool size.
		 */
		void SetSize(int size) { m_Size = size; }

		private:
		ToolType m_SelectedTool = ToolType::Pencil;
		float m_Col[3] = {0.0f, 0.0f, 0.0f};
		int m_Size = 1;
		KeybindsRegistry& m_Keybinds;
	};
}  // namespace FuncDoodle
