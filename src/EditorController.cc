#include "EditorController.h"

#include "Grid.h"
#include "Player.h"
#include "Tool.h"
#include "ToolManager.h"
#include "imgui.h"
#include "imgui_internal.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <stack>

namespace FuncDoodle {
	const float c_StatusBarHeight = 20.0f;

	EditorController::EditorController() {}

	bool EditorController::Paint(Frame* frame, unsigned long frameI,
		ToolManager* toolManager, AnimationPlayer* player, int pixelX,
		int pixelY, bool mouseDown, bool mouseClicked) {
		if (!frame || !frame->Pixels() || !toolManager || !player ||
			!player->Proj()) {
			return false;
		}

		switch (toolManager->SelectedTool()) {
			case ToolType::Pencil:
				return PaintPencil(frame, frameI, toolManager, player, pixelX,
					pixelY, mouseDown);
			case ToolType::Eraser:
				return PaintEraser(frame, frameI, toolManager, player, pixelX,
					pixelY, mouseDown);
			case ToolType::Bucket:
				return PaintBucket(frame, frameI, toolManager, player, pixelX,
					pixelY, mouseClicked);
			case ToolType::Picker:
				return PaintPicker(frame, toolManager, pixelX, pixelY);
			case ToolType::Select:
				return PaintSelect(frame, toolManager, pixelX, pixelY);
			default:
				return false;
		}
	}

	void EditorController::SetUndoByStroke(
		bool undoByStroke, AnimationPlayer* player) {
		if (m_UndoByStroke == undoByStroke) {
			return;
		}
		if (!undoByStroke && m_StrokeActive) {
			FinalizeStroke(player);
		}
		m_UndoByStroke = undoByStroke;
	}

	void EditorController::EndStroke(AnimationPlayer* player) {
		FinalizeStroke(player);
	}

	bool EditorController::PaintPencil(Frame* frame, unsigned long frameI,
		ToolManager* toolManager, AnimationPlayer* player, int pixelX,
		int pixelY, bool mouseDown) {
		if (!mouseDown) {
			return false;
		}
		const float* colOld = toolManager->Col();
		Col newColor = {static_cast<unsigned char>(colOld[0] * 255.0f + 0.5f),
			static_cast<unsigned char>(colOld[1] * 255.0f + 0.5f),
			static_cast<unsigned char>(colOld[2] * 255.0f + 0.5f)};

		int size = toolManager->Size();
		bool actionPerformed = false;
		int startOffset = -(size / 2);
		int endOffset = startOffset + size - 1;

		for (int offsetY = startOffset; offsetY <= endOffset; offsetY++) {
			for (int offsetX = startOffset; offsetX <= endOffset; offsetX++) {
				int newX = pixelX + offsetX;
				int newY = pixelY + offsetY;

				if (newX < 0 || newX >= frame->Pixels()->Width() || newY < 0 ||
					newY >= frame->Pixels()->Height()) {
					continue;
				}

				Col prevColor = frame->Pixels()->Get(newX, newY);
				frame->SetPixel(newX, newY, newColor);

				if (prevColor == newColor) {
					continue;
				}

				if (m_UndoByStroke) {
					RecordStrokeChange(frameI, newX, newY, prevColor, newColor);
				} else {
					DrawAction action(newX, newY, prevColor, newColor, frameI,
						player->Proj());
					player->Proj()->PushUndoable(action);
				}
				actionPerformed = true;
			}
		}
		return actionPerformed;
	}

	bool EditorController::PaintEraser(Frame* frame, unsigned long frameI,
		ToolManager* toolManager, AnimationPlayer* player, int pixelX,
		int pixelY, bool mouseDown) {
		if (!mouseDown) {
			return false;
		}
		int size = toolManager->Size();
		bool actionPerformed = false;
		int startOffset = -(size / 2);
		int endOffset = startOffset + size - 1;
		Col bgColor = player->Proj()->BgCol();

		for (int offsetY = startOffset; offsetY <= endOffset; offsetY++) {
			for (int offsetX = startOffset; offsetX <= endOffset; offsetX++) {
				int newX = pixelX + offsetX;
				int newY = pixelY + offsetY;

				if (newX < 0 || newX >= frame->Pixels()->Width() || newY < 0 ||
					newY >= frame->Pixels()->Height()) {
					continue;
				}

				Col prevColor = frame->Pixels()->Get(newX, newY);
				frame->SetPixel(newX, newY, bgColor);

				if (prevColor == bgColor) {
					continue;
				}

				if (m_UndoByStroke) {
					RecordStrokeChange(frameI, newX, newY, prevColor, bgColor);
				} else {
					DrawAction action(
						newX, newY, prevColor, bgColor, frameI, player->Proj());
					player->Proj()->PushUndoable(action);
				}
				actionPerformed = true;
			}
		}

		return actionPerformed;
	}

	bool EditorController::PaintBucket(Frame* frame, unsigned long frameI,
		ToolManager* toolManager, AnimationPlayer* player, int pixelX,
		int pixelY, bool mouseClicked) {
		if (!mouseClicked || pixelX < 0 || pixelX >= frame->Pixels()->Width() ||
			pixelY < 0 || pixelY >= frame->Pixels()->Height()) {
			return false;
		}

		const float* colOld = toolManager->Col();
		Col fillColor = {static_cast<unsigned char>(colOld[0] * 255.0f + 0.5f),
			static_cast<unsigned char>(colOld[1] * 255.0f + 0.5f),
			static_cast<unsigned char>(colOld[2] * 255.0f + 0.5f)};
		Col curPixelCol = frame->Pixels()->Get(pixelX, pixelY);

		if (curPixelCol == fillColor) {
			return false;
		}

		std::vector<std::pair<int, int>> pixelsChangedByBucketTool;
		FloodFill(pixelX, pixelY, curPixelCol, fillColor, frame,
			pixelsChangedByBucketTool);
		if (pixelsChangedByBucketTool.empty()) {
			return false;
		}

		FillAction action(curPixelCol, fillColor, frameI, player->Proj(),
			pixelsChangedByBucketTool);
		player->Proj()->PushUndoable(action);
		return true;
	}

	bool EditorController::PaintPicker(
		Frame* frame, ToolManager* toolManager, int pixelX, int pixelY) {
		if (!frame || !frame->Pixels() || !toolManager || pixelX < 0 ||
			pixelX >= frame->Pixels()->Width() || pixelY < 0 ||
			pixelY >= frame->Pixels()->Height()) {
			return false;
		}

		toolManager->SetCol(frame->Pixels()->Get(pixelX, pixelY));
		return true;
	}

	bool EditorController::PaintSelect(
		Frame* frame, ToolManager* toolManager, int pixelX, int pixelY) {
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			// im a squaresel
			// m_SquareSel
			m_SquareSel.Active = true;
			m_SquareSel.Min = ImVec2i(pixelX, pixelY);
			m_SquareSel.Max = m_SquareSel.Min;
		}

		if (m_SquareSel.Active) {
			m_SquareSel.Max = ImVec2i(pixelX, pixelY);

			// if (m_SquareSel.Min.x == m_SquareSel.Max.x && m_SquareSel.Min.y
			// == m_SquareSel.Max.y) { m_Sel = nullptr;
			// }
		}

		return true;
	}

	void EditorController::FloodFill(int x, int y, Col targetCol, Col fillCol,
		Frame* targetFrame, std::vector<std::pair<int, int>>& changed) {
		if (!targetFrame || !targetFrame->Pixels()) {
			return;
		}

		std::stack<std::pair<int, int>> pixelStack;
		pixelStack.push({x, y});

		while (!pixelStack.empty()) {
			auto [currentX, currentY] = pixelStack.top();
			pixelStack.pop();

			if (currentX < 0 || currentX >= targetFrame->Pixels()->Width() ||
				currentY < 0 || currentY >= targetFrame->Pixels()->Height()) {
				continue;
			}

			Col currentCol = targetFrame->Pixels()->Get(currentX, currentY);
			if (currentCol != targetCol || currentCol == fillCol) {
				continue;
			}

			changed.emplace_back(currentX, currentY);
			targetFrame->SetPixel(currentX, currentY, fillCol);

			pixelStack.push({currentX + 1, currentY});
			pixelStack.push({currentX - 1, currentY});
			pixelStack.push({currentX, currentY + 1});
			pixelStack.push({currentX, currentY - 1});
		}
	}

	void EditorController::RecordStrokeChange(
		unsigned long frameI, int x, int y, const Col& prev, const Col& next) {
		if (prev == next) {
			return;
		}

		if (!m_StrokeActive || m_StrokeFrameI != frameI) {
			m_StrokeActive = true;
			m_StrokeFrameI = frameI;
			m_StrokeChanges.clear();
			m_StrokeIndexByKey.clear();
		}

		const unsigned long long key =
			(static_cast<unsigned long long>(static_cast<unsigned int>(y))
				<< 32U) |
			static_cast<unsigned int>(x);
		auto it = m_StrokeIndexByKey.find(key);
		if (it == m_StrokeIndexByKey.end()) {
			m_StrokeIndexByKey[key] = m_StrokeChanges.size();
			m_StrokeChanges.push_back(StrokeAction::PixelChange{
				.x = x, .y = y, .prev = prev, .next = next});
		} else {
			m_StrokeChanges[it->second].next = next;
		}
	}

	void EditorController::FinalizeStroke(AnimationPlayer* player) {
		if (!m_StrokeActive) {
			return;
		}

		m_StrokeActive = false;
		if (!m_StrokeChanges.empty() && player && player->Proj()) {
			StrokeAction action(
				m_StrokeFrameI, player->Proj(), std::move(m_StrokeChanges));
			player->Proj()->PushUndoable(action);
		}
		m_StrokeChanges.clear();
		m_StrokeIndexByKey.clear();
	}

	void EditorController::RenderCanvas(CanvasContext& context) {
		if (!context.Frame || !context.Player || !context.Player->Proj() ||
			!context.ToolManager || !context.Grid || !context.PixelScale ||
			!context.LastMousePos) {
			return;
		}
		const ImageArray* pixels = context.Frame->Pixels();
		if (!pixels) {
			return;
		}

		HandleCanvasInput(context);

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 contentRegion = ImGui::GetContentRegionAvail();
		const float frameWidth = pixels->Width() * context.PixelScale;
		const float frameHeight = pixels->Height() * context.PixelScale;
		const float startX =
			windowPos.x + (contentRegion.x - frameWidth) * 0.5f + 9;
		const float startY =
			windowPos.y +
			((contentRegion.y - c_StatusBarHeight) - frameHeight) * 0.5f + 41;
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ApplyToolAt(context, pixels, startX, startY, frameWidth, frameHeight);
		DrawCanvas(
			context, pixels, drawList, startX, startY, frameWidth, frameHeight);
	}

	void EditorController::HandleCanvasInput(CanvasContext& context) {
		if (!ImGui::IsAnyItemActive() && ImGui::IsWindowFocused()) {
			if (ImGui::IsKeyPressed(ImGuiKey_Equal)) {
				context.PixelScale += 1;
			}
			if (ImGui::IsKeyPressed(ImGuiKey_Minus)) {
				context.PixelScale = std::max(1, context.PixelScale - 1);
			}
			if (ImGui::IsKeyPressed(ImGuiKey_0)) {
				context.PixelScale = 1;
			}
			if (ImGui::IsKeyPressed(ImGuiKey_T)) {
				if (context.Grid->GridWidth() > 1)
					context.Grid->SetGridWidth(context.Grid->GridWidth() - 1);
				if (context.Grid->GridHeight() > 1)
					context.Grid->SetGridHeight(context.Grid->GridHeight() - 1);
			}
			if (ImGui::IsKeyPressed(ImGuiKey_Y)) {
				context.Grid->SetGridWidth(context.Grid->GridWidth() + 1);
				context.Grid->SetGridHeight(context.Grid->GridHeight() + 1);
			}
			if (ImGui::IsKeyPressed(ImGuiKey_G)) {
				if (context.Grid->GridVisibility())
					context.Grid->HideGrid();
				else
					context.Grid->ShowGrid();
			}
		}
	}

	void EditorController::ApplyToolAt(CanvasContext& context,
		const ImageArray* pixels, float startX, float startY, float frameWidth,
		float frameHeight) {
		const ImVec2 mousePos = ImGui::GetMousePos();
		const ImVec2 frameMin(startX, startY);
		const ImVec2 frameMax(startX + frameWidth, startY + frameHeight);
		const bool shouldDraw = ImGui::IsMouseDown(0);

		if (ImGui::IsMouseHoveringRect(frameMin, frameMax) && shouldDraw) {
			const ImVec2 currentPixel(
				(mousePos.x - startX) / context.PixelScale,
				(mousePos.y - startY) / context.PixelScale);
			if (currentPixel.x >= 0 && currentPixel.x < pixels->Width() &&
				currentPixel.y >= 0 && currentPixel.y < pixels->Height()) {
				const ToolType selectedTool =
					context.ToolManager->SelectedTool();
				ImGuiContext* ctx = ImGui::GetCurrentContext();
				ImGuiWindow* focusedWindow = ctx->NavWindow;
				const bool isFrameWindowFocused =
					focusedWindow &&
					std::strcmp(focusedWindow->Name, "Frame") == 0;

				if (isFrameWindowFocused) {
					if (context.LastMousePos->x >= 0 &&
						context.LastMousePos->y >= 0 &&
						(selectedTool == ToolType::Pencil ||
							selectedTool == ToolType::Eraser)) {
						const float dx =
							currentPixel.x - context.LastMousePos->x;
						const float dy =
							currentPixel.y - context.LastMousePos->y;
						const int steps =
							std::max(1, std::max(static_cast<int>(std::abs(dx)),
											static_cast<int>(std::abs(dy))));
						for (int i = 0; i <= steps; i++) {
							const float t = static_cast<float>(i) / steps;
							const ImVec2 interpPixel(
								context.LastMousePos->x + dx * t,
								context.LastMousePos->y + dy * t);

							Paint(context.Frame, context.Index,
								context.ToolManager, context.Player,
								static_cast<int>(interpPixel.x),
								static_cast<int>(interpPixel.y), true, false);
						}
					} else if (selectedTool == ToolType::Select) {
						Paint(context.Frame, context.Index, context.ToolManager,
							context.Player, static_cast<int>(currentPixel.x),
							static_cast<int>(currentPixel.y), shouldDraw,
							ImGui::IsMouseClicked(0));
					} else {
						Paint(context.Frame, context.Index, context.ToolManager,
							context.Player, static_cast<int>(currentPixel.x),
							static_cast<int>(currentPixel.y), shouldDraw,
							ImGui::IsMouseClicked(0));
					}
				}
				*context.LastMousePos = currentPixel;
				*context.LastHoverMousePos = currentPixel;
			}
		} else if (!ImGui::IsMouseDown(0) ||
				   !ImGui::IsMouseHoveringRect(frameMin, frameMax)) {
			const ImVec2 currentPixel(
				(mousePos.x - startX) / context.PixelScale,
				(mousePos.y - startY) / context.PixelScale);

			*context.LastMousePos = ImVec2(-1, -1);
			*context.LastHoverMousePos = currentPixel;
		}

		if (context.ToolManager->SelectedTool() == ToolType::Select &&
			m_SquareSel.Active &&
			ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
			m_SquareSel.Active = false;
			if (m_SquareSel.Min.x == m_SquareSel.Max.x &&
				m_SquareSel.Min.y == m_SquareSel.Max.y) {
				m_Sel.reset();
			} else {
				m_Sel = std::make_unique<SquareSelection>(m_SquareSel);
			}
			FUNC_INF("m_SquareSel min: " << m_SquareSel.Min.x << ","
										 << m_SquareSel.Min.y
										 << "; max: " << m_SquareSel.Max.x
										 << "," << m_SquareSel.Max.y);
		}

		if (ImGui::IsMouseHoveringRect(frameMin, frameMax) && !shouldDraw &&
			!ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId) &&
			!context.Player->Playing()) {
			if (context.PrevEnabled &&
				context.ToolManager->SelectedTool() == ToolType::Pencil) {
				ImVec2 pixel = *context.LastHoverMousePos;

				if (pixel.x >= 0 && pixel.y >= 0) {
					int px = (int)pixel.x;
					int py = (int)pixel.y;

					float brushSize = (float)context.ToolManager->Size();
					float scaledSize = brushSize * context.PixelScale;

					int startOffset = -(int)(brushSize / 2);

					float brushOriginX =
						startX + (px + startOffset) * context.PixelScale;
					float brushOriginY =
						startY + (py + startOffset) * context.PixelScale;

					// calc brush bounds in pixel coordinates
					int brushMinX = px + startOffset;
					int brushMinY = py + startOffset;
					int brushMaxX = brushMinX + (int)brushSize;
					int brushMaxY = brushMinY + (int)brushSize;

					// clip to frame bounds
					int frameWidth = pixels->Width();
					int frameHeight = pixels->Height();
					int clipMinX = std::max(0, brushMinX);
					int clipMinY = std::max(0, brushMinY);
					int clipMaxX = std::min(frameWidth, brushMaxX);
					int clipMaxY = std::min(frameHeight, brushMaxY);

					// only render if there's any visible area
					if (clipMaxX > clipMinX && clipMaxY > clipMinY) {
						float clipOriginX =
							startX + clipMinX * context.PixelScale;
						float clipOriginY =
							startY + clipMinY * context.PixelScale;
						float clipWidth =
							(clipMaxX - clipMinX) * context.PixelScale;
						float clipHeight =
							(clipMaxY - clipMinY) * context.PixelScale;

						ImVec2 min(clipOriginX, clipOriginY);
						ImVec2 max(
							clipOriginX + clipWidth, clipOriginY + clipHeight);

						Col col = Col::FromFloat3(context.ToolManager->Col());

						ImGui::GetForegroundDrawList()->AddRectFilled(
							min, max, IM_COL32(col.r, col.g, col.b, 255));
					}
				}
			}
		}

		if (!ImGui::IsMouseDown(0)) {
			EndStroke(context.Player);
		}
	}

	void EditorController::DrawCanvas(CanvasContext& context,
		const ImageArray* pixels, ImDrawList* drawList, float startX,
		float startY, float frameWidth, float frameHeight) {
		for (int y = 0; y < pixels->Height(); y++) {
			for (int x = 0; x < pixels->Width(); x++) {
				const Col col = pixels->Get(x, y);
				const ImVec2 topLeft(startX + x * context.PixelScale,
					startY + y * context.PixelScale);
				const ImVec2 bottomRight(startX + (x + 1) * context.PixelScale,
					startY + (y + 1) * context.PixelScale);
				drawList->AddRectFilled(
					topLeft, bottomRight, IM_COL32(col.r, col.g, col.b, 255));
			}
		}

		if (context.Index > 0 && context.PreviousFrame &&
			context.PreviousFrame->Pixels() && !context.Player->Playing()) {
			const ImageArray* prevPixels = context.PreviousFrame->Pixels();
			for (int y = 0; y < prevPixels->Height(); y++) {
				for (int x = 0; x < prevPixels->Width(); x++) {
					const Col col = prevPixels->Get(x, y);
					const Col curCol = pixels->Get(x, y);
					if (curCol.r == 255 && curCol.g == 255 && curCol.b == 255) {
						const ImVec2 topLeft(startX + x * context.PixelScale,
							startY + y * context.PixelScale);
						const ImVec2 bottomRight(
							startX + (x + 1) * context.PixelScale,
							startY + (y + 1) * context.PixelScale);
						drawList->AddRectFilled(topLeft, bottomRight,
							IM_COL32(col.r, col.g, col.b, 128));
					}
				}
			}
		}

		if (!m_Sel && !m_SquareSel.Active) {
			return;
		}

		context.Grid->RenderWithDrawList(drawList, ImVec2(startX, startY),
			ImVec2(startX + frameWidth, startY + frameHeight));

		float t = ImGui::GetTime();
		float dash = 6.0f;
		float gap = 6.0f;
		float offset = fmod(t * 60.0f, dash + gap);

		float screenMinX = startX + m_SquareSel.Min.x * context.PixelScale;
		float screenMinY = startY + m_SquareSel.Min.y * context.PixelScale;
		float screenMaxX =
			startX + (m_SquareSel.Max.x + 1) * context.PixelScale;
		float screenMaxY =
			startY + (m_SquareSel.Max.y + 1) * context.PixelScale;

		for (float x = screenMinX - offset; x < screenMaxX; x += dash + gap)
			drawList->AddLine({x, screenMinY},
				{ImMin(x + dash, screenMaxX), screenMinY},
				IM_COL32(150, 150, 150, 255));

		for (float x = screenMinX - offset; x < screenMaxX; x += dash + gap)
			drawList->AddLine({x, screenMaxY},
				{ImMin(x + dash, screenMaxX), screenMaxY},
				IM_COL32(150, 150, 150, 255));

		for (float y = screenMinY - offset; y < screenMaxY; y += dash + gap)
			drawList->AddLine({screenMinX, y},
				{screenMinX, ImMin(y + dash, screenMaxY)},
				IM_COL32(150, 150, 150, 255));

		for (float y = screenMinY - offset; y < screenMaxY; y += dash + gap)
			drawList->AddLine({screenMaxX, y},
				{screenMaxX, ImMin(y + dash, screenMaxY)},
				IM_COL32(150, 150, 150, 255));
	}

}  // namespace FuncDoodle
