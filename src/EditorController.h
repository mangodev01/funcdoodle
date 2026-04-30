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
#include "AppSettings.h"
#include "Frame.h"
#include "Grid.h"
#include "Player.h"
#include "Ptr.h"
#include "Selection.h"
#include "ToolManager.h"
#include "imgui.h"

#include <unordered_map>
#include <vector>

namespace FuncDoodle {
	class EditorController {
		public:
		struct CanvasContext {
			class Frame* Frame = nullptr;
			class Frame* PreviousFrame = nullptr;
			unsigned long Index = 0;
			ToolManager* ToolManager = nullptr;
			AnimationPlayer* Player = nullptr;
			UniquePtr<Grid> Grid = nullptr;
			int PixelScale = 8;
			ImVec2 LastMousePos = {-1, -1};
			ImVec2 LastHoverMousePos = {-1, -1};
			AppSettings& Settings;

			CanvasContext(AppSettings& settings) : Settings(settings) {}
		};

		EditorController();

		bool Paint(Frame* frame, unsigned long frameI, ToolManager* toolManager,
			AnimationPlayer* player, int pixelX, int pixelY, bool mouseDown,
			bool mouseClicked);
		void SetUndoByStroke(bool undoByStroke, AnimationPlayer* player);
		void RenderCanvas(CanvasContext& context);
		void EndStroke(AnimationPlayer* player);
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
		bool PaintPicker(
			Frame* frame, ToolManager* toolManager, int pixelX, int pixelY);
		bool PaintSelect(
			Frame* frame, ToolManager* toolManager, int pixelX, int pixelY);
		void FloodFill(int x, int y, Col targetCol, Col fillCol,
			Frame* targetFrame, std::vector<std::pair<int, int>>& changed);
		void RecordStrokeChange(unsigned long frameI, int x, int y,
			const Col& prev, const Col& next);
		void HandleCanvasInput(CanvasContext& context);
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
