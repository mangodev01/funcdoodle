#pragma once

#include "AppSettings.h"
#include "Frame.h"

#include "Action/Action.h"
#include "Grid.h"
#include "ToolManager.h"

#include "Player.h"

#include "Ptr.h"

#include <memory>

#include "EditorController.h"
#include "imgui.h"

namespace FuncDoodle {
	class FrameRenderer {
		public:
			FrameRenderer(Frame* frame, unsigned long index,
				ToolManager* manager, AnimationPlayer* player,
				const SharedPtr<EditorController>& editorController,
				AppSettings& settings)
				: m_Ctx(settings) {
				m_Ctx.Frame = frame;
				m_Ctx.PreviousFrame = nullptr;
				m_Ctx.Index = index;
				m_Ctx.ToolManager = manager;
				m_Ctx.Player = player;
				m_Ctx.Grid = nullptr;
				m_Ctx.LastMousePos = ImVec2{-1, -1};
				m_Ctx.LastHoverMousePos = ImVec2{-1, -1};

				m_EditorController = editorController;
			}
			void RenderFrame();
			void RenderStatusBar();
			void InitPixels();

			inline EditorController::CanvasContext* GetCtx() { return &m_Ctx; }
			void SetUndoByStroke(bool undoByStroke) {
				if (m_EditorController) {
					m_EditorController->SetUndoByStroke(
						undoByStroke, m_Ctx.Player);
				}
			}
			void RenderFramePixels(int startX, int startY, ImDrawList* drawList,
				bool usePrevPxScale = true, bool renderPreview = true);

		private:
			EditorController::CanvasContext m_Ctx;
			SharedPtr<EditorController> m_EditorController;
			// ImVec2 m_LastHoverMousePos = ImVec2(-1, -1);
	};
}  // namespace FuncDoodle
