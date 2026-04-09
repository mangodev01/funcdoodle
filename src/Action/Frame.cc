#include "Action.h"

#include "Project.h"

namespace FuncDoodle {
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
}  // namespace FuncDoodle
