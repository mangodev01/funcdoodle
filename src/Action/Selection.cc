#include "Action.h"

#include "Project/Project.h"

namespace FuncDoodle {
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

	void MoveSelectionAction::Undo() {
		auto proj = m_Ctx.Proj;
		if (!proj)
			return;
		Frame* frame = proj->AnimFrames()->Get(m_Ctx.FrameIndex);
		if (!frame)
			return;
		*frame = m_FrameBeforeMove;
	}

	void MoveSelectionAction::Redo() {
		auto proj = m_Ctx.Proj;
		if (!proj)
			return;
		Frame* frame = proj->AnimFrames()->Get(m_Ctx.FrameIndex);
		if (!frame)
			return;
		frame->MoveSelection(m_Ctx.Sel, m_Ctx.MoveDir, proj->BgCol());
	}
}  // namespace FuncDoodle
