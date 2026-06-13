#include "KeyHandler.h"

#include "Core/App.h"
#include "Keybinds/Keybinds.h"
#include "UI/Gui.h"

namespace FuncDoodle {
	void KeyHandler::RegisterKeybinds(KeybindsRegistry& keybinds) {
		keybinds.Register("prev_frame", "Previous frame",
			"Go to the previous frame",
			{false, false, false, ImGuiKey_LeftBracket});
		keybinds.Register("next_frame", "Next frame", "Go to the next frame",
			{false, false, false, ImGuiKey_RightBracket});

		keybinds.Register("undo", "Undo", "Undo the last action",
			{false, false, false, ImGuiKey_E});
		keybinds.Register("redo", "Redo", "Redo the last undone action",
			{false, false, false, ImGuiKey_R});

		keybinds.Register("insert_after", "Insert after",
			"Insert a new frame after the current",
			{false, false, false, ImGuiKey_P});
		keybinds.Register("insert_before", "Insert before",
			"Insert a new frame before the current",
			{false, false, false, ImGuiKey_O});
		keybinds.Register("delete_frame", "Delete frame",
			"Delete the current frame",
			{false, false, false, ImGuiKey_Backslash});
		keybinds.Register("copy_frame", "Copy frame", "Copy the current frame",
			{false, false, false, ImGuiKey_Comma});
		keybinds.Register("paste_frame_after", "Paste after",
			"Paste the copied frame after the current",
			{false, false, false, ImGuiKey_Slash});
		keybinds.Register("paste_frame_before", "Paste before",
			"Paste the copied frame before the current",
			{false, false, false, ImGuiKey_Period});
		keybinds.Register("duplicate_frame_after", "Duplicate after",
			"Duplicate the current frame after it",
			{false, false, false, ImGuiKey_M});
		keybinds.Register("duplicate_frame_before", "Duplicate before",
			"Duplicate the current frame before it",
			{false, false, false, ImGuiKey_N});
		keybinds.Register("move_forward", "Move forward",
			"Move the current frame forward",
			{false, false, false, ImGuiKey_I});
		keybinds.Register("move_backward", "Move backward",
			"Move the current frame backward",
			{false, false, false, ImGuiKey_U});

		keybinds.Register("move_selection_left", "Move selection left",
			"Move the selection left",
			{false, false, false, ImGuiKey_LeftArrow});
		keybinds.Register("move_selection_right", "Move selection right",
			"Move the selection right",
			{false, false, false, ImGuiKey_RightArrow});
		keybinds.Register("move_selection_up", "Move selection up",
			"Move the selection up", {false, false, false, ImGuiKey_UpArrow});
		keybinds.Register("move_selection_down", "Move selection down",
			"Move the selection down",
			{false, false, false, ImGuiKey_DownArrow});

		keybinds.Register("toggle_grid", "Toggle grid", "Show or hide the grid",
			{false, false, false, ImGuiKey_G});
		keybinds.Register("increase_grid", "Increase grid size",
			"Increase the grid cell size", {false, false, false, ImGuiKey_Y});
		keybinds.Register("decrease_grid", "Decrease grid size",
			"Decrease the grid cell size", {false, false, false, ImGuiKey_T});

		keybinds.Register("zoom_in", "Zoom in", "Zoom in the viewport",
			{false, false, false, ImGuiKey_Equal});
		keybinds.Register("zoom_out", "Zoom out", "Zoom out the viewport",
			{false, false, false, ImGuiKey_Minus});
		keybinds.Register("reset_zoom", "Reset zoom",
			"Reset the viewport zoom to default",
			{false, false, false, ImGuiKey_0});

		keybinds.Register("export_frame", "Export frame",
			"Export the current frame to a file",
			{false, false, false, ImGuiKey_X});
	}

	void KeyHandler::HandleTimelineShortcuts(
		TimelineKeyContext& context, KeybindsRegistry& keybinds) {
		if (!context.Proj || !context.SelectedFrame) {
			return;
		}
		if (ImGui::IsAnyItemActive()) {
			return;
		}

		uint64_t& selectedFrame = *context.SelectedFrame;

		if (keybinds.Get("prev_frame").IsPressed()) {
			if (selectedFrame > 0) {
				Frame* prevFrame =
					context.Proj->AnimFrames()->Get(selectedFrame);
				selectedFrame--;

				Frame* frame = context.Proj->AnimFrames()->Get(selectedFrame);
				if (context.FrameRenderer) {
					context.FrameRenderer->GetCtx()->PreviousFrame = prevFrame;
					context.FrameRenderer->GetCtx()->Frame = frame;
				}
			}
		}
		if (keybinds.Get("next_frame").IsPressed()) {
			if (selectedFrame < context.Proj->AnimFrameCount() - 1) {
				Frame* prevFrame =
					context.Proj->AnimFrames()->Get(selectedFrame);
				selectedFrame++;

				Frame* frame = context.Proj->AnimFrames()->Get(selectedFrame);
				if (context.FrameRenderer) {
					context.FrameRenderer->GetCtx()->PreviousFrame = prevFrame;
					context.FrameRenderer->GetCtx()->Frame = frame;
				}
			}
		}

		const int X1 = 3;
		const int X2 = 4;
		if (keybinds.Get("undo").IsPressed() || ImGui::IsMouseClicked(X1)) {
			SharedPtr<ProjectFile> proj =
				context.Player ? context.Player->Proj() : context.Proj;
			if (proj) {
				proj->Undo();
			}
		}
		if (keybinds.Get("redo").IsPressed() || ImGui::IsMouseClicked(X2)) {
			SharedPtr<ProjectFile> proj =
				context.Player ? context.Player->Proj() : context.Proj;
			if (proj) {
				proj->Redo();
			}
		}
		if (keybinds.Get("insert_after").IsPressed()) {
			context.Proj->AnimFrames()->InsertAfterEmpty(selectedFrame);
			InsertFrameAction action =
				InsertFrameAction(selectedFrame + 1, context.Proj);
			context.Proj->PushUndoable(action);
		}
		if (keybinds.Get("insert_before").IsPressed()) {
			context.Proj->AnimFrames()->InsertBeforeEmpty(selectedFrame);
			selectedFrame++;
			InsertFrameAction action =
				InsertFrameAction(selectedFrame - 1, context.Proj);
			context.Proj->PushUndoable(action);
		}
		if (keybinds.Get("move_forward").IsPressed()) {
			context.Proj->AnimFrames()->MoveForward(selectedFrame);
			selectedFrame++;
			MoveFrameRightAction action =
				MoveFrameRightAction(selectedFrame, context.Proj);
			context.Proj->PushUndoable(action);
		}
		if (keybinds.Get("move_backward").IsPressed()) {
			if (selectedFrame != 0) {
				context.Proj->AnimFrames()->MoveBackward(selectedFrame);
				MoveFrameLeftAction action =
					MoveFrameLeftAction(selectedFrame, context.Proj);
				selectedFrame--;
				context.Proj->PushUndoable(action);
			}
		}
		if (keybinds.Get("delete_frame").IsPressed()) {
			if (context.Proj->AnimFrameCount() != 1) {
				Frame deletedFrame =
					*context.Proj->AnimFrames()->Get(selectedFrame);
				context.Proj->AnimFrames()->Remove(selectedFrame);
				DeleteFrameAction action = DeleteFrameAction(
					selectedFrame, &deletedFrame, context.Proj);
				context.Proj->PushUndoable(action);
			}
		}
		if (keybinds.Get("copy_frame").IsPressed()) {
			context.Proj->AnimFrames()->Get(selectedFrame)->CopyToClipboard();
		}
		if (keybinds.Get("paste_frame_before").IsPressed()) {
			Frame* frame = Frame::PastedFrame();
			context.Proj->AnimFrames()->InsertBefore(selectedFrame, frame);
			selectedFrame++;
		}
		if (keybinds.Get("paste_frame_after").IsPressed()) {
			Frame* frame = Frame::PastedFrame();
			context.Proj->AnimFrames()->InsertAfter(selectedFrame, frame);
		}
		if (keybinds.Get("duplicate_frame_after").IsPressed()) {
			auto* frame =
				new Frame(*context.Proj->AnimFrames()->Get(selectedFrame));
			context.Proj->AnimFrames()->InsertAfter(selectedFrame, frame);
			InsertFrameAction action =
				InsertFrameAction(selectedFrame + 1, context.Proj);
			context.Proj->PushUndoable(action);
		}
		if (keybinds.Get("duplicate_frame_before").IsPressed()) {
			auto* frame =
				new Frame(*context.Proj->AnimFrames()->Get(selectedFrame));
			context.Proj->AnimFrames()->InsertBefore(selectedFrame, frame);
			selectedFrame++;
			InsertFrameAction action =
				InsertFrameAction(selectedFrame - 1, context.Proj);
			context.Proj->PushUndoable(action);
		}
		if (keybinds.Get("import_before").IsPressed()) {
			Application::Get()->Import(Application::Where::Before);
		}
		if (keybinds.Get("import_after").IsPressed()) {
			Application::Get()->Import(Application::Where::After);
		}
	}
}  // namespace FuncDoodle
