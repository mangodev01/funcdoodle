#pragma once

#include "Project.h"
#include "Ptr.h"

#include <memory>
#include <optional>
#include <vector>

namespace FuncDoodle {
	class Action {
		public:
		virtual void Undo() = 0;
		virtual void Redo() = 0;
		virtual ~Action() = default;

		friend class DrawAction;
		friend class FillAction;
		friend class DeleteFrameAction;
	};

	class DrawAction : public Action {
		public:
		DrawAction(int x, int y, Col prev, Col next, unsigned long frameI,
			const SharedPtr<ProjectFile>& proj)
			: m_X(x), m_Y(y), m_Prev(prev), m_Next(next), m_FrameIndex(frameI),
			  m_Proj(proj) {};
		DrawAction(const DrawAction& other)
			: Action(other), m_X(other.m_X), m_Y(other.m_Y),
			  m_Prev(other.m_Prev), m_Next(other.m_Next),
			  m_FrameIndex(other.m_FrameIndex), m_Proj(other.m_Proj) {};
		~DrawAction() {}
		void Undo() override;
		void Redo() override;

		private:
		int m_X, m_Y;
		Col m_Prev;
		Col m_Next;
		unsigned long m_FrameIndex;
		WeakPtr<ProjectFile> m_Proj;
	};

	class FillAction : public Action {
		public:
		FillAction(Col prev, Col next, unsigned long frameI,
			const SharedPtr<ProjectFile>& proj,
			std::vector<std::pair<int, int>> affected)
			: m_Prev(prev), m_Next(next), m_FrameIndex(frameI), m_Proj(proj),
			  m_Pixels(affected) {};
		FillAction(const FillAction& other)
			: Action(other), m_Prev(other.m_Prev), m_Next(other.m_Next),
			  m_FrameIndex(other.m_FrameIndex), m_Proj(other.m_Proj),
			  m_Pixels(other.m_Pixels) {};
		~FillAction() {}
		void Undo() override;
		void Redo() override;

		private:
		Col m_Prev;
		Col m_Next;
		unsigned long m_FrameIndex;
		WeakPtr<ProjectFile> m_Proj;
		std::vector<std::pair<int, int>> m_Pixels;
	};

	class StrokeAction : public Action {
		public:
		struct PixelChange {
			int x;
			int y;
			Col prev;
			Col next;
		};
		StrokeAction(unsigned long frameI, const SharedPtr<ProjectFile>& proj,
			std::vector<PixelChange> changes)
			: m_FrameIndex(frameI), m_Proj(proj),
			  m_Changes(std::move(changes)) {}
		void Undo() override;
		void Redo() override;

		private:
		unsigned long m_FrameIndex;
		WeakPtr<ProjectFile> m_Proj;
		std::vector<PixelChange> m_Changes;
	};
}  // namespace FuncDoodle