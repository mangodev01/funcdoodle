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
		/** @brief Creates a grid overlay with the given cell size and contrast color. */
		Grid(int gridWidth, int gridHeight, Col opposite);
		~Grid();
		/** @brief Draws the grid into the provided ImGui draw list. */
		void RenderWithDrawList(
			ImDrawList* drawList, ImVec2 startPos, ImVec2 endPos);
		/** @brief Makes the grid visible. */
		inline const void ShowGrid() { m_ShowGrid = true; };
		/** @brief Hides the grid. */
		inline const void HideGrid() { m_ShowGrid = false; };
		/** @brief Toggles grid visibility. */
		inline const void ToggleGrid() { m_ShowGrid = !m_ShowGrid; };
		/** @brief Returns whether the grid is currently visible. */
		inline const bool GridVisibility() { return m_ShowGrid; };
		/** @brief Updates the horizontal grid spacing. */
		inline const void SetGridWidth(int gridWidth) {
			m_GridWidth = gridWidth;
		}
		/** @brief Updates the vertical grid spacing. */
		inline const void SetGridHeight(int gridHeight) {
			m_GridHeight = gridHeight;
		}
		/** @brief Returns the horizontal grid spacing. */
		inline const int GridWidth() { return m_GridWidth; }
		/** @brief Returns the vertical grid spacing. */
		inline const int GridHeight() { return m_GridHeight; }

		private:
		int m_GridWidth;
		int m_GridHeight;
		bool m_ShowGrid;
		Col m_OppositeCol;
	};
}  // namespace FuncDoodle
