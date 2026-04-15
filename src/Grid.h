#pragma once

#include "Gui.h"

#include "Frame.h"

namespace FuncDoodle {
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
