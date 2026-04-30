/**
 * @file Grid.h
 * @brief Renders and manages the editor grid overlay.
 *
 * This file defines the Grid class, which is responsible for drawing
 * a visual grid over the canvas to assist with pixel alignment and editing.
 *
 * Features:
 * - Configurable grid width and height
 * - Toggleable visibility state
 * - Rendering via ImGui draw lists
 * - Color inversion support for contrast (m_OppositeCol)
 *
 * The grid is intended to be a single shared instance to avoid visual desync
 * across different editor views.
 *
 * @note Only one Grid instance should typically exist during runtime.
 */

#pragma once

#include "Gui.h"

#include "Frame.h"

namespace FuncDoodle {
	/**
	 * @class Grid
	 * @brief Represents a singular grid.
	 *
	 * @note Should only be instantiated once so no grid desync happens.
	 */
	class Grid {
		public:
		Grid(int gridWidth, int gridHeight, Col opposite);
		~Grid();
		void RenderWithDrawList(
			ImDrawList* drawList, ImVec2 startPos, ImVec2 endPos);
		inline const void ShowGrid() { m_ShowGrid = true; };
		inline const void HideGrid() { m_ShowGrid = false; };
		inline const void ToggleGrid() { m_ShowGrid = !m_ShowGrid; };
		inline const bool GridVisibility() { return m_ShowGrid; };
		inline const void SetGridWidth(int gridWidth) {
			m_GridWidth = gridWidth;
		}
		inline const void SetGridHeight(int gridHeight) {
			m_GridHeight = gridHeight;
		}
		inline const int GridWidth() { return m_GridWidth; }
		inline const int GridHeight() { return m_GridHeight; }

		private:
		int m_GridWidth;
		int m_GridHeight;
		bool m_ShowGrid;
		Col m_OppositeCol;
	};
}  // namespace FuncDoodle
