#include <ranges>

#include "Action.h"

#include "Project/Project.h"

#include <tuple>

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
			for (const std::tuple<int, int, Col>& xyc : m_Pixels) {
				proj->AnimFrames()
					->Get(m_FrameIndex)
					->SetPixel(std::get<0>(xyc), std::get<1>(xyc), std::get<2>(xyc));
			}
		}
	}
	void FillAction::Redo() {
		if (auto proj = m_Proj.lock()) {
			for (const std::tuple<int, int, Col>& xyc : m_Pixels) {
				proj->AnimFrames()
					->Get(m_FrameIndex)
					->SetPixel(std::get<0>(xyc), std::get<1>(xyc), m_Next);
			}
		}
	}
	void StrokeAction::Undo() {
		if (auto proj = m_Proj.lock()) {
			Frame* frame = proj->AnimFrames()->Get(m_FrameIndex);
			if (!frame)
				return;
			for (auto& m_Change : std::ranges::reverse_view(m_Changes)) {
				frame->SetPixel(m_Change.x, m_Change.y, m_Change.prev);
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
