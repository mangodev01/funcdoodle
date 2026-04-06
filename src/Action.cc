#include "Action.h"

#include "Project.h"

namespace FuncDoodle {
	void DrawAction::Undo() {
		if (auto proj = m_Proj.lock()) {
			proj->AnimFrames()->Get(m_FrameIndex)->SetPixel(m_X, m_Y, m_Prev);
		}
	}
	void DrawAction::Redo() {
		if (auto proj = m_Proj.lock()) {
			proj->AnimFrames()->Get(m_FrameIndex)->SetPixel(m_X, m_Y, m_Next);
		}
	}

	void FillAction::Undo() {
		if (auto proj = m_Proj.lock()) {
			for (const std::pair<int, int>& xy : m_Pixels) {
				proj->AnimFrames()
					->Get(m_FrameIndex)
					->SetPixel(xy.first, xy.second, m_Prev);
			}
		}
	}
	void FillAction::Redo() {
		if (auto proj = m_Proj.lock()) {
			for (const std::pair<int, int>& xy : m_Pixels) {
				proj->AnimFrames()
					->Get(m_FrameIndex)
					->SetPixel(xy.first, xy.second, m_Next);
			}
		}
	}
	void StrokeAction::Undo() {
		if (auto proj = m_Proj.lock()) {
			Frame* frame = proj->AnimFrames()->Get(m_FrameIndex);
			if (!frame)
				return;
			for (auto it = m_Changes.rbegin(); it != m_Changes.rend(); ++it) {
				frame->SetPixel(it->x, it->y, it->prev);
			}
		}
	}
	void StrokeAction::Redo() {
		if (auto proj = m_Proj.lock()) {
			Frame* frame = proj->AnimFrames()->Get(m_FrameIndex);
			if (!frame)
				return;
			for (const auto& change : m_Changes) {
				frame->SetPixel(change.x, change.y, change.next);
			}
		}
	}

	void DeleteFrameAction::Undo() {
		if (auto proj = m_Proj.lock()) {
			if (m_Empty) {
				if (m_FrameIndex > 1)
					proj->AnimFrames()->InsertBeforeEmpty(m_FrameIndex);
				else
					proj->AnimFrames()->PushBackEmpty();
			} else if (m_Frame.has_value()) {
				if (m_FrameIndex > 1)
					proj->AnimFrames()->InsertBefore(m_FrameIndex, *m_Frame);
				else
					proj->AnimFrames()->PushBack(&(*m_Frame));
			}
		}
	}
	void DeleteFrameAction::Redo() {
		if (auto proj = m_Proj.lock())
			proj->AnimFrames()->Remove(m_FrameIndex);
	}

	void InsertFrameAction::Undo() {
		if (auto proj = m_Proj.lock())
			proj->AnimFrames()->Remove(m_FrameIndex);
	}
	void InsertFrameAction::Redo() {
		if (auto proj = m_Proj.lock()) {
			if (m_Empty) {
				if (m_FrameIndex > 1)
					proj->AnimFrames()->InsertBeforeEmpty(m_FrameIndex);
				else
					proj->AnimFrames()->PushBackEmpty();
			} else if (m_Frame.has_value()) {
				if (m_FrameIndex > 1)
					proj->AnimFrames()->InsertBefore(m_FrameIndex, *m_Frame);
				else
					proj->AnimFrames()->PushBack(&(*m_Frame));
			}
		}
	}
	void MoveFrameLeftAction::Undo() {
		if (auto proj = m_Proj.lock()) {
			proj->AnimFrames()->MoveForward(m_FrameIndex - 1);
		}
	}
	void MoveFrameLeftAction::Redo() {
		if (auto proj = m_Proj.lock()) {
			proj->AnimFrames()->MoveBackward(m_FrameIndex);
		}
	}
	void MoveFrameRightAction::Undo() {
		if (auto proj = m_Proj.lock()) {
			proj->AnimFrames()->MoveBackward(m_FrameIndex);
		}
	}
	void MoveFrameRightAction::Redo() {
		if (auto proj = m_Proj.lock()) {
			proj->AnimFrames()->MoveForward(m_FrameIndex);
		}
	}

	void RotateFrameAction::Undo() {
		if (auto proj = m_Proj.lock()) {
			proj->AnimFrames()->Get(m_FrameIndex)->Rotate(-m_Deg);
		}
	}

	void RotateFrameAction::Redo() {
		if (auto proj = m_Proj.lock()) {
			proj->AnimFrames()->Get(m_FrameIndex)->Rotate(m_Deg);
		}
	}

	void RotateSelectionAction::Undo() {
		if (auto proj = m_Proj.lock()) {
			proj->AnimFrames()
				->Get(m_FrameIndex)
				->RotateSelection(m_Sel, -m_Deg);
		}
	}

	void RotateSelectionAction::Redo() {
		if (auto proj = m_Proj.lock()) {
			proj->AnimFrames()
				->Get(m_FrameIndex)
				->RotateSelection(m_Sel, m_Deg);
		}
	}

	void DeleteSelectionAction::Undo() {
		if (auto proj = m_Proj.lock()) {
			if (auto sel = m_Sel.lock()) {
				Frame* frame = proj->AnimFrames()->Get(m_FrameIndex);
				if (!frame)
					return;

				auto pixels = sel->All();
				for (size_t i = 0; i < pixels.size(); i++) {
					frame->SetPixel(pixels[i].x, pixels[i].y, m_PrevPixels[i]);
				}
			}
		}
	}

	void DeleteSelectionAction::Redo() {
		if (auto proj = m_Proj.lock()) {
			proj->AnimFrames()
				->Get(m_FrameIndex)
				->DeleteSelection(m_Sel, Col{});
		}
	}
}  // namespace FuncDoodle
