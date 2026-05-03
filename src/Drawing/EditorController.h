/**
 * @file EditorController.h
 * @brief Core editor logic handling canvas interaction, painting tools, and
 * stroke management.
 *
 * This file defines EditorController, which is responsible for:
 * - Processing user input on the canvas (mouse/keyboard interaction)
 * - Dispatching tool behavior (pencil, eraser, bucket, picker, selection)
 * - Rendering the canvas using ImGui draw lists
 * - Managing stroke-based undo/redo recording
 * - Coordinating with AnimationPlayer, ToolManager, and Selection systems
 *
 * It also contains CanvasContext, a per-frame runtime structure used to
 * pass rendering and input state into canvas operations.
 *
 * The controller acts as the central bridge between UI input and frame editing
 * logic.
 */

#pragma once

/**
 * @file EditorController.h
 * @brief Core editor logic handling canvas interaction, painting tools, and
 * stroke management.
 *
 * This file defines EditorController, which is responsible for:
 * - Processing mouse input on the canvas
 * - Managing active painting strokes
 * - Handling tool behavior (pencil, eraser, etc.)
 * - Selection manipulation and rendering
 */

#include "Action/Action.h"
#include "Core/AppSettings.h"
#include "Core/Grid.h"
#include "Core/Player.h"
#include "Project/Frame.h"
#include "Selection/Selection.h"
#include "Tool/ToolManager.h"
#include "Util/Ptr.h"
#include "imgui.h"

#include <unordered_map>
#include <vector>

namespace FuncDoodle {
	/**
	 * @class EditorController
	 * @brief Handles canvas editing, painting tools, and stroke recording.
	 *
	 * Bridges canvas input, tool execution, and frame mutation logic.
	 */
	class EditorController {
		public:
		/**
		 * @struct CanvasContext
		 * @brief Bundles per-frame canvas state used during rendering and input
		 * handling.
		 */
		struct CanvasContext {
			class Frame* Frame =
				nullptr;  ///< Frame currently being rendered and edited.
			class Frame* PreviousFrame =
				nullptr;  ///< Previous frame used for preview rendering.
			unsigned long Index = 0;  ///< Timeline index of the active frame.
			ToolManager* ToolManager =
				nullptr;  ///< Tool state used for painting.
			AnimationPlayer* Player =
				nullptr;  ///< Player attached to the current project.
			UniquePtr<Grid> Grid =
				nullptr;		 ///< Optional grid overlay for the canvas.
			int PixelScale = 8;	 ///< Screen-space size of one pixel.
			ImVec2 LastMousePos = {-1,
				-1};  ///< Last mouse position used for stroke interpolation.
			ImVec2 LastHoverMousePos = {
				-1, -1};  ///< Last hovered mouse position over the canvas.
			AppSettings&
				Settings;  ///< Application settings affecting canvas behavior.

			/**
			 * @fn CanvasContext
			 * @brief Creates a canvas context bound to application settings.
			 *
			 * @param settings Settings reference used while editing.
			 */
			CanvasContext(AppSettings& settings) : Settings(settings) {}
		};

		/**
		 * @fn EditorController
		 * @brief Creates an editor controller.
		 */
		EditorController();

		/**
		 * @fn Paint
		 * @brief Applies the active tool to a frame at a specific pixel.
		 *
		 * @param frame Frame being edited.
		 * @param frameI Frame index within the project.
		 * @param toolManager Tool state and color source.
		 * @param player Animation player for project access.
		 * @param pixelX Target pixel X coordinate.
		 * @param pixelY Target pixel Y coordinate.
		 * @param mouseDown Whether the primary mouse button is held.
		 * @param mouseClicked Whether the primary mouse button was clicked this
		 * frame.
		 * @return Whether the frame was modified.
		 */
		bool Paint(Frame* frame, unsigned long frameI, ToolManager* toolManager,
			AnimationPlayer* player, int pixelX, int pixelY, bool mouseDown,
			bool mouseClicked);
		/**
		 * @fn SetUndoByStroke
		 * @brief Enables or disables stroke-grouped undo behavior.
		 *
		 * @param undoByStroke Whether strokes should become single undo
		 * entries.
		 * @param player Player providing project access for flushing active
		 * strokes.
		 */
		void SetUndoByStroke(bool undoByStroke, AnimationPlayer* player);
		/**
		 * @fn RenderCanvas
		 * @brief Renders the frame canvas and handles its live interaction.
		 *
		 * @param context Canvas state for the frame being rendered.
		 */
		void RenderCanvas(CanvasContext& context);
		/**
		 * @fn EndStroke
		 * @brief Finalizes the active stroke and pushes its undo record.
		 *
		 * @param player Player providing access to the current project.
		 */
		void EndStroke(AnimationPlayer* player);
		/**
		 * @fn ResetState
		 * @brief Clears transient editor state tied to the active project.
		 */
		void ResetState();
		/**
		 * @fn Sel
		 * @brief Returns the active selection object.
		 *
		 * @return Shared selection pointer.
		 */
		SharedPtr<Selection> Sel() { return m_Sel; }

		private:
		bool PaintPencil(Frame* frame, unsigned long frameI,
			ToolManager* toolManager, AnimationPlayer* player, int pixelX,
			int pixelY, bool mouseDown);
		bool PaintEraser(Frame* frame, unsigned long frameI,
			ToolManager* toolManager, AnimationPlayer* player, int pixelX,
			int pixelY, bool mouseDown);
		bool PaintBucket(Frame* frame, unsigned long frameI,
			ToolManager* toolManager, AnimationPlayer* player, int pixelX,
			int pixelY, bool mouseClicked);
		static bool PaintPicker(
			Frame* frame, ToolManager* toolManager, int pixelX, int pixelY);
		bool PaintSelect(
			Frame* frame, ToolManager* toolManager, int pixelX, int pixelY);
		static void FloodFill(int x, int y, Col targetCol, Col fillCol,
			Frame* targetFrame, std::vector<std::pair<int, int>>& changed);
		void RecordStrokeChange(unsigned long frameI, int x, int y,
			const Col& prev, const Col& next);
		static void HandleCanvasInput(CanvasContext& context);
		void ApplyToolAt(CanvasContext& context, const ImageArray* pixels,
			float startX, float startY, float frameWidth, float frameHeight);
		void DrawCanvas(CanvasContext& context, const ImageArray* pixels,
			ImDrawList* drawList, float startX, float startY, float frameWidth,
			float frameHeight);
		void FinalizeStroke(AnimationPlayer* player);

		bool m_UndoByStroke = false;
		bool m_StrokeActive = false;
		unsigned long m_StrokeFrameI = 0;
		std::vector<StrokeAction::PixelChange> m_StrokeChanges;
		std::unordered_map<unsigned long long, size_t> m_StrokeIndexByKey;
		SharedPtr<Selection> m_Sel;
		SquareSelection m_SquareSel;
	};
}  // namespace FuncDoodle
