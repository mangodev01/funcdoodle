#include "FrameRenderer.h"

#include "Gui.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "Frame.h"

#include <iostream>

#include <memory>
#include <utility>

#include "MacroUtils.h"

#include "Player.h"

#include <cmath>

namespace FuncDoodle {
	const float c_StatusBarHeight = 24.0f;

	void FrameRenderer::RenderFrame() {
		ImGui::SetNextWindowPos(ImVec2(0, 32), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(1073, 862), ImGuiCond_FirstUseEver);
		ImGui::Begin("Frame", 0,
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		if (!m_Ctx.Frame || !m_Ctx.ToolManager) {
			ImGui::End();
			return;
		}

		if (!m_Ctx.Grid) {
			m_Ctx.Grid = std::make_unique<Grid>(m_Ctx.Frame->Width(),
				m_Ctx.Frame->Height(), m_Ctx.Player->Proj()->BgCol());
		}

		if (ImGui::BeginPopupContextWindow()) {
			if (ImGui::MenuItem("Zoom out", "-")) {
				m_Ctx.PixelScale = std::max(1, m_Ctx.PixelScale - 1);
			}
			if (ImGui::MenuItem("Zoom equal", "0")) {
				m_Ctx.PixelScale = 1;
			}
			if (ImGui::MenuItem("Zoom in", "=")) {
				m_Ctx.PixelScale++;
			}
			if (ImGui::MenuItem("Toggle Grid", "G")) {
				if (m_Ctx.Grid->GridVisibility())
					m_Ctx.Grid->HideGrid();
				else
					m_Ctx.Grid->ShowGrid();
			}
			if (ImGui::MenuItem("Increase grid size", "Y")) {
				m_Ctx.Grid->SetGridWidth(m_Ctx.Grid->GridWidth() + 1);
				m_Ctx.Grid->SetGridHeight(m_Ctx.Grid->GridHeight() + 1);
			}
			if (ImGui::MenuItem("Decrease grid size", "T")) {
				if (m_Ctx.Grid->GridWidth() > 1)
					m_Ctx.Grid->SetGridWidth(m_Ctx.Grid->GridWidth() - 1);
				if (m_Ctx.Grid->GridHeight() > 1)
					m_Ctx.Grid->SetGridHeight(m_Ctx.Grid->GridHeight() - 1);
			}
			ImGui::EndPopup();
		}
		InitPixels();

		RenderStatusBar();

		ImGui::End();
	}

	void FrameRenderer::RenderStatusBar() {
		float availY = ImGui::GetContentRegionAvail().y;
		ImGui::SetCursorPosY(
			ImGui::GetCursorPosY() + availY - c_StatusBarHeight);

		ImGui::Separator();

		ImGui::Text("Frame %lu | %dx%d | Zoom: %dx | X: %.0f, Y: %.0f | Draw "
					"X: %.0f, Draw Y: %.0f",
			m_Ctx.Index, m_Ctx.Frame->Width(), m_Ctx.Frame->Height(),
			m_Ctx.PixelScale,
			m_Ctx.LastHoverMousePos ? m_Ctx.LastHoverMousePos->x : 0,
			m_Ctx.LastHoverMousePos ? m_Ctx.LastHoverMousePos->y : 0,
			m_Ctx.LastMousePos ? m_Ctx.LastMousePos->x : 0,
			m_Ctx.LastMousePos ? m_Ctx.LastMousePos->y : 0);
	}

	void FrameRenderer::InitPixels() {
		if (!m_EditorController) {
			return;
		}

		m_EditorController->RenderCanvas(m_Ctx);
	}

	void FrameRenderer::RenderFramePixels(int startX, int startY,
		ImDrawList* drawList, bool usePrevPxScale, bool renderPreview) {
		// Use the appropriate frame for rendering
		const ImageArray* pixels = m_Ctx.Frame->Pixels();

		for (int y = 0; y < pixels->Height(); y++) {
			for (int x = 0; x < pixels->Width(); x++) {
				Col col = pixels->Get(x, y);

				ImVec2 topLeft, bottomRight;
				if (usePrevPxScale) {
					topLeft = ImVec2(startX + x * m_Ctx.PixelScale,
						startY + y * m_Ctx.PixelScale);
					bottomRight = ImVec2(startX + (x + 1) * m_Ctx.PixelScale,
						startY + (y + 1) * m_Ctx.PixelScale);
				} else {
					topLeft = ImVec2(startX + x, startY + y);
					bottomRight = ImVec2(startX + x + 1, startY + y + 1);
				}

				drawList->AddRectFilled(
					topLeft, bottomRight, IM_COL32(col.r, col.g, col.b, 255));
			}
		}
	}
}  // namespace FuncDoodle
