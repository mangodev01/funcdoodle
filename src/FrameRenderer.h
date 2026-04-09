#pragma once

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
				bool prevEnabled) {
				m_Ctx = EditorController::CanvasContext();
				m_Ctx.Frame = frame;
				m_Ctx.PreviousFrame = nullptr;
				m_Ctx.Index = index;
				m_Ctx.ToolManager = manager;
				m_Ctx.Player = player;
				m_Ctx.Grid = nullptr;

				// disgusting
				m_Ctx.LastMousePos = new ImVec2();
				m_Ctx.LastMousePos->x = -1;
				m_Ctx.LastMousePos->y = -1;

				// disgusting
				m_Ctx.LastHoverMousePos = new ImVec2();
				m_Ctx.LastHoverMousePos->x = -1;
				m_Ctx.LastHoverMousePos->y = -1;

				m_Ctx.PrevEnabled = prevEnabled;

				m_EditorController = editorController;
			}
			~FrameRenderer() {
				delete m_Ctx.LastMousePos;
				delete m_Ctx.LastHoverMousePos;
			}
			void RenderFrame();
			void RenderStatusBar();
			void InitPixels();

			inline EditorController::CanvasContext* Ctx() { return &m_Ctx; }
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
