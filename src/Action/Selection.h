#pragma once

#include "Common.h"
#include "../Selection.h"
#include "Project.h"
#include "Ptr.h"
#include <cstdint>

namespace FuncDoodle {
	class RotateSelectionAction : public Action {
		public:
		RotateSelectionAction(unsigned long frameIndex, WeakPtr<Selection> sel,
			int32_t deg, const SharedPtr<ProjectFile>& proj)
			: m_FrameIndex(frameIndex), m_Sel(std::move(sel)), m_Proj(proj),
			  m_Deg(deg) {}

		void Undo() override;
		void Redo() override;

		[[nodiscard]] inline int32_t Deg() const { return m_Deg; };
		[[nodiscard]] inline WeakPtr<Selection> Sel() const { return m_Sel; };

		private:
		unsigned long m_FrameIndex;
		WeakPtr<Selection> m_Sel;
		int32_t m_Deg;
		WeakPtr<ProjectFile> m_Proj;
	};

	class DeleteSelectionAction : public Action {
		public:
		DeleteSelectionAction(unsigned long frameIndex, WeakPtr<Selection> sel,
			std::vector<Col> prevPixels, const SharedPtr<ProjectFile>& proj)
			: m_FrameIndex(frameIndex), m_Sel(std::move(sel)),
			  m_PrevPixels(std::move(prevPixels)), m_Proj(proj) {}

		void Undo() override;
		void Redo() override;

		[[nodiscard]] inline WeakPtr<Selection> Sel() const { return m_Sel; };

		private:
		unsigned long m_FrameIndex;
		WeakPtr<Selection> m_Sel;
		std::vector<Col> m_PrevPixels;
		WeakPtr<ProjectFile> m_Proj;
	};

	class MoveSelectionAction : public Action {
		public:
		MoveSelectionAction(Frame frame, MoveSelectionActionContext ctx)
			: m_FrameBeforeMove(frame), m_Ctx(ctx) {}

		void Undo() override;
		void Redo() override;

		[[nodiscard]] inline WeakPtr<Selection> Sel() const { return m_Ctx.sel; };
		[[nodiscard]] inline Direction Dir() const { return m_Ctx.moveDir; };
		[[nodiscard]] inline SharedPtr<ProjectFile> Proj() const { return m_Ctx.proj; };
		[[nodiscard]] inline unsigned long FrameIndex() const { return m_Ctx.frameIndex; };
		[[nodiscard]] inline MoveSelectionActionContext Ctx() const { return m_Ctx; };

		private:
		MoveSelectionActionContext m_Ctx;
		Frame m_FrameBeforeMove;
	};
}
