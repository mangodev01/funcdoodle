/**
 * @file FrameRenderer.h
 * @brief Responsible for rendering individual animation frames and canvas UI.
 *
 * This file defines the FrameRenderer class, which handles:
 * - Rendering the pixel canvas for a single frame
 * - Drawing status bar information for the frame view
 * - Forwarding user input and painting actions to EditorController
 * - Managing a CanvasContext that bundles rendering and input state
 *
 * FrameRenderer acts as the bridge between the UI layer (ImGui rendering)
 * and the editor logic (EditorController).
 *
 * @see EditorController
 */

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
	/**
	 * @class FrameRenderer
	 * @brief Responible for rendering a single frame, status bar and dispatching painting to EditorController
	 *
	 * Handles drawing the frame canvas, status bar, and forwarding painting
	 * operations to the EditorController through a CanvasContext.
	 *
	 * @see EditorController
	 */
	class FrameRenderer {
		public:
		/**
		 * @fn FrameRenderer
		 * @brief Creates a renderer for a single frame view.
		 *
		 * @param frame Frame to render.
		 * @param index Timeline index of the frame.
		 * @param manager Tool manager used for painting and UI state.
		 * @param player Animation player attached to the project.
		 * @param editorController Shared editor controller used for painting.
		 * @param settings Application settings used for rendering behavior.
		 */
		FrameRenderer(Frame* frame, unsigned long index, ToolManager* manager,
			AnimationPlayer* player,
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
		/**
		 * @fn RenderFrame
		 * @brief Renders the complete frame view.
		 */
		void RenderFrame();
		/**
		 * @fn RenderStatusBar
		 * @brief Renders status information associated with the frame view.
		 */
		void RenderStatusBar() const;
		/**
		 * @fn InitPixels
		 * @brief Initializes pixel-related state before rendering.
		 */
		void InitPixels();

		/**
		 * @fn GetCtx
		 * @brief Returns the mutable canvas context used by this renderer.
		 *
		 * @return Pointer to the canvas context.
		 */
		EditorController::CanvasContext* GetCtx() { return &m_Ctx; }
		/**
		 * @fn SetUndoByStroke
		 * @brief Forwards undo-by-stroke preference to the editor controller.
		 *
		 * @param undoByStroke Whether strokes should be grouped into one undo step.
		 */
		void SetUndoByStroke(bool undoByStroke) {
			if (m_EditorController) {
				m_EditorController->SetUndoByStroke(undoByStroke, m_Ctx.Player);
			}
		}
		/**
		 * @fn RenderFramePixels
		 * @brief Draws frame pixels into a draw list.
		 *
		 * @param startX Canvas start X coordinate.
		 * @param startY Canvas start Y coordinate.
		 * @param drawList Draw list receiving pixel primitives.
		 * @param usePrevPxScale Whether to reuse cached pixel scale values.
		 * @param renderPreview Whether to render previous-frame preview pixels.
		 */
		void RenderFramePixels(int startX, int startY, ImDrawList* drawList,
			bool usePrevPxScale = true, bool renderPreview = true) const;

		private:
		EditorController::CanvasContext m_Ctx;
		SharedPtr<EditorController> m_EditorController;
		// ImVec2 m_LastHoverMousePos = ImVec2(-1, -1);
	};
}  // namespace FuncDoodle
