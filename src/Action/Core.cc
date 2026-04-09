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
}  // namespace FuncDoodle
