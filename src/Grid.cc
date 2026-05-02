#include "Grid.h"

#include "Gui.h"

#include <iostream>

#include "MacroUtils.h"

#include <cmath>

#include "Frame.h"

namespace FuncDoodle {
	void Grid::RenderWithDrawList(
		ImDrawList* drawList, ImVec2 startPos, ImVec2 endPos) const {
		if (!m_ShowGrid)
			return;

		// Calculate cell sizes with rounding to avoid floating point
		// imprecision
		float cellWidth = std::round((endPos.x - startPos.x) / m_GridWidth);
		float cellHeight = std::round((endPos.y - startPos.y) / m_GridHeight);

		// Draw vertical lines with rounded positions
		for (int x = 0; x <= m_GridWidth; x++) {
			float xPos = std::round(startPos.x + (x * cellWidth)) - 0.7f;
			drawList->AddLine(ImVec2(xPos, startPos.y), ImVec2(xPos, endPos.y),
				INVERTED_IMCOL(m_OppositeCol.r, m_OppositeCol.g,
					m_OppositeCol.b)  // Gray color
			);
		}

		// Draw horizontal lines with rounded positions
		for (int y = 0; y <= m_GridHeight; y++) {
			float yPos = std::round(startPos.y + (y * cellHeight)) - 0.7f;
			drawList->AddLine(ImVec2(startPos.x, yPos), ImVec2(endPos.x, yPos),
				INVERTED_IMCOL(m_OppositeCol.r, m_OppositeCol.g,
					m_OppositeCol.b)  // Gray color
			);
		}
	}
	Grid::Grid(int gridWidth, int gridHeight, Col opposite)
		: m_GridWidth(gridWidth), m_GridHeight(gridHeight), 
		  m_OppositeCol(opposite) {}
	Grid::~Grid() = default;
}  // namespace FuncDoodle
