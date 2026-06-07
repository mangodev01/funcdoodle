#include "FrameRenderer.h"

#include "Core/App.h"

#include <filesystem>
#include <imgui.h>

#include "Project/Frame.h"

#include <memory>

#include "Conf/Constants.h"

#include "Core/Player.h"
#include "UI/Gui.h"

namespace FuncDoodle {
	const float c_StatusBarHeight = 24.0f;

	void FrameRenderer::RenderFrame() {
		Application* app = Application::Get();
		KeybindsRegistry& keys = app->GetKeybinds();

		ImGui::Begin("Frame", nullptr,
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
			Shortcut zoomOut = keys.Get("zoom_out");
			Shortcut resetZoom = keys.Get("reset_zoom");
			Shortcut zoomIn = keys.Get("zoom_in");
			Shortcut toggleGrid = keys.Get("toggle_grid");
			Shortcut decreaseGrid = keys.Get("decrease_grid");
			Shortcut increaseGrid = keys.Get("increase_grid");
			Shortcut exportFrame = keys.Get("export_frame");

			if (ImGui::MenuItem("Zoom out", zoomOut)) {
				m_Ctx.PixelScale = std::max(1, m_Ctx.PixelScale - 1);
			}
			if (ImGui::MenuItem("Reset zoom", resetZoom)) {
				m_Ctx.PixelScale = 1;
			}
			if (ImGui::MenuItem("Zoom in", zoomIn)) {
				m_Ctx.PixelScale++;
			}
			if (ImGui::MenuItem("Toggle Grid", toggleGrid)) {
				if (m_Ctx.Grid->GridVisibility())
					m_Ctx.Grid->HideGrid();
				else
					m_Ctx.Grid->ShowGrid();
			}
			if (ImGui::MenuItem("Increase grid size", increaseGrid)) {
				m_Ctx.Grid->SetGridWidth(m_Ctx.Grid->GridWidth() + 1);
				m_Ctx.Grid->SetGridHeight(m_Ctx.Grid->GridHeight() + 1);
			}
			if (ImGui::MenuItem("Decrease grid size", decreaseGrid)) {
				if (m_Ctx.Grid->GridWidth() > 1)
					m_Ctx.Grid->SetGridWidth(m_Ctx.Grid->GridWidth() - 1);
				if (m_Ctx.Grid->GridHeight() > 1)
					m_Ctx.Grid->SetGridHeight(m_Ctx.Grid->GridHeight() - 1);
			}
			if (ImGui::MenuItem("Export", exportFrame)) {
				FileDialog diag(g_SupportedExtensionsForImporting);

				std::filesystem::path path = diag.Save();

				m_Ctx.Frame->Export(path.c_str());
			}

			ImGui::EndPopup();
		}
		InitPixels();

		RenderStatusBar();

		ImGui::End();
	}

	void FrameRenderer::RenderStatusBar() const {
		float availY = ImGui::GetContentRegionAvail().y;
		ImGui::SetCursorPosY(
			ImGui::GetCursorPosY() + availY - c_StatusBarHeight);

		ImGui::Separator();

		ImGui::Text(
			"%s%s | Frame %lu | %dx%d | Zoom: %dx | X: %.0f, Y: %.0f | Draw "
			"X: %.0f, Draw Y: %.0f",
			m_Ctx.Player->Proj()->AnimName(),
			m_Ctx.Player->Proj()->Saved() ? "" : "*", m_Ctx.Index,
			m_Ctx.Frame->Width(), m_Ctx.Frame->Height(), m_Ctx.PixelScale,
			m_Ctx.LastHoverMousePos.x, m_Ctx.LastHoverMousePos.y,
			m_Ctx.LastMousePos.x, m_Ctx.LastMousePos.y);
	}

	void FrameRenderer::InitPixels() {
		if (!m_EditorController) {
			return;
		}

		m_EditorController->RenderCanvas(m_Ctx);
	}

	void FrameRenderer::RenderFramePixels(int startX, int startY,
		ImDrawList* drawList, bool usePrevPxScale, bool renderPreview) const {
		// Use the appropriate frame for rendering
		const ImageArray* pixels = m_Ctx.Frame->Pixels();

		for (int y = 0; y < pixels->Height(); y++) {
			for (int x = 0; x < pixels->Width(); x++) {
				Col col = pixels->Get(x, y);

				ImVec2 topLeft;
				ImVec2 bottomRight;
				if (usePrevPxScale) {
					topLeft = ImVec2(startX + (x * m_Ctx.PixelScale),
						startY + (y * m_Ctx.PixelScale));
					bottomRight = ImVec2(startX + ((x + 1) * m_Ctx.PixelScale),
						startY + ((y + 1) * m_Ctx.PixelScale));
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
