#pragma once

#include "Action/Action.h"
#include "AppSettings.h"
#include "Frame.h"
#include "Ptr.h"
#include "Selection.h"
#include "imgui.h"

#include <unordered_map>
#include <vector>

namespace FuncDoodle {
	class ToolManager;
	class AnimationPlayer;
	class Grid;

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
