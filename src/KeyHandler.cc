#include "KeyHandler.h"

#include "Keybinds.h"
#include "imgui.h"

namespace FuncDoodle {
	void KeyHandler::RegisterKeybinds(KeybindsRegistry& keybinds) {
		keybinds.Register(
			"prev_frame", {false, false, false, ImGuiKey_LeftBracket});
		keybinds.Register(
			"next_frame", {false, false, false, ImGuiKey_RightBracket});
		keybinds.Register("undo", {false, false, false, ImGuiKey_E});
		keybinds.Register("redo", {false, false, false, ImGuiKey_R});
		keybinds.Register("insert_after", {false, false, false, ImGuiKey_P});
		keybinds.Register("insert_before", {false, false, false, ImGuiKey_O});
		keybinds.Register("move_forward", {false, false, false, ImGuiKey_I});
		keybinds.Register("move_backward", {false, false, false, ImGuiKey_U});

		keybinds.Register(
			"move_selection_left", {false, false, false, ImGuiKey_LeftArrow});
		keybinds.Register(
			"move_selection_right", {false, false, false, ImGuiKey_RightArrow});
		keybinds.Register(
			"move_selection_up", {false, false, false, ImGuiKey_UpArrow});
		keybinds.Register(
			"move_selection_down", {false, false, false, ImGuiKey_DownArrow});

		keybinds.Register(
			"delete_frame", {false, false, false, ImGuiKey_Backslash});
		keybinds.Register("copy_frame", {false, false, false, ImGuiKey_Comma});
		keybinds.Register(
			"paste_frame_before", {false, false, false, ImGuiKey_Period});
		keybinds.Register(
			"paste_frame_after", {false, false, false, ImGuiKey_Slash});
		keybinds.Register(
			"duplicate_frame_after", {false, false, false, ImGuiKey_M});
		keybinds.Register(
			"duplicate_frame_before", {false, false, false, ImGuiKey_N});
	}

	void KeyHandler::HandleTimelineShortcuts(
		TimelineKeyContext& context, KeybindsRegistry& keybinds) {
		if (!context.Proj || !context.SelectedFrame) {
			return;
		}
		if (ImGui::IsAnyItemActive()) {
			return;
		}

		unsigned long& selectedFrame = *context.SelectedFrame;

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
			Frame* frame =
				new Frame(*context.Proj->AnimFrames()->Get(selectedFrame));
			context.Proj->AnimFrames()->InsertAfter(selectedFrame, frame);
			InsertFrameAction action =
				InsertFrameAction(selectedFrame + 1, context.Proj);
			context.Proj->PushUndoable(action);
		}
		if (keybinds.Get("duplicate_frame_before").IsPressed()) {
			Frame* frame =
				new Frame(*context.Proj->AnimFrames()->Get(selectedFrame));
			context.Proj->AnimFrames()->InsertBefore(selectedFrame, frame);
			selectedFrame++;
			InsertFrameAction action =
				InsertFrameAction(selectedFrame - 1, context.Proj);
			context.Proj->PushUndoable(action);
		}
	}
}  // namespace FuncDoodle
