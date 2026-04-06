#pragma once

#include "Frame.h"
#include "Ptr.h"
#include "Selection.h"

#include <memory>
#include <optional>
#include <vector>

namespace FuncDoodle {
	class ProjectFile;

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
				: m_X(x), m_Y(y), m_Prev(prev), m_Next(next),
				  m_FrameIndex(frameI), m_Proj(proj) {};
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
				: m_Prev(prev), m_Next(next), m_FrameIndex(frameI),
				  m_Proj(proj), m_Pixels(affected) {};
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
			StrokeAction(unsigned long frameI,
				const SharedPtr<ProjectFile>& proj,
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
	class DeleteFrameAction : public Action {
		public:
			// empty constructor
			DeleteFrameAction(
				unsigned long frameI, const SharedPtr<ProjectFile>& proj)
				: m_Proj(proj), m_FrameIndex(frameI), m_Empty(true),
				  m_Frame(std::nullopt) {}
			// good constructor
			DeleteFrameAction(unsigned long frameI, Frame* frame,
				const SharedPtr<ProjectFile>& proj)
				: m_Proj(proj), m_FrameIndex(frameI), m_Empty(frame == nullptr),
				  m_Frame(frame ? std::optional<Frame>(*frame) : std::nullopt) {
			}

			void Undo() override;
			void Redo() override;

		private:
			unsigned long m_FrameIndex;
			bool m_Empty;
			std::optional<Frame> m_Frame;
			WeakPtr<ProjectFile> m_Proj;
	};
	class InsertFrameAction : public Action {
		public:
			// empty constructor
			InsertFrameAction(
				unsigned long frameI, const SharedPtr<ProjectFile>& proj)
				: m_Proj(proj), m_FrameIndex(frameI), m_Empty(true),
				  m_Frame(std::nullopt) {}
			// good constructor
			InsertFrameAction(unsigned long frameI, Frame* frame,
				const SharedPtr<ProjectFile>& proj)
				: m_FrameIndex(frameI), m_Proj(proj), m_Empty(frame == nullptr),
				  m_Frame(frame ? std::optional<Frame>(*frame) : std::nullopt) {
			}

			void Undo() override;
			void Redo() override;

		private:
			unsigned long m_FrameIndex;
			bool m_Empty;
			std::optional<Frame> m_Frame;
			WeakPtr<ProjectFile> m_Proj;
	};
	class MoveFrameLeftAction : public Action {
		public:
			MoveFrameLeftAction(
				unsigned long frameI, const SharedPtr<ProjectFile>& proj)
				: m_Proj(proj), m_FrameIndex(frameI) {}
			void Undo() override;
			void Redo() override;

		private:
			unsigned long m_FrameIndex;
			WeakPtr<ProjectFile> m_Proj;
	};
	class MoveFrameRightAction : public Action {
		public:
			MoveFrameRightAction(
				unsigned long frameI, const SharedPtr<ProjectFile>& proj)
				: m_Proj(proj), m_FrameIndex(frameI) {}
			void Undo() override;
			void Redo() override;

		private:
			unsigned long m_FrameIndex;
			WeakPtr<ProjectFile> m_Proj;
	};
	class RotateFrameAction : public Action {
		public:
			RotateFrameAction(unsigned long frameIndex, int32_t deg,
				const SharedPtr<ProjectFile>& proj)
				: m_FrameIndex(frameIndex), m_Proj(proj), m_Deg(deg) {}

			void Undo() override;
			void Redo() override;

			inline int32_t Deg() { return m_Deg; };

		private:
			unsigned long m_FrameIndex;
			int32_t m_Deg;
			WeakPtr<ProjectFile> m_Proj;
	};
	class RotateSelectionAction : public Action {
		public:
			RotateSelectionAction(unsigned long frameIndex,
				WeakPtr<Selection> sel, int32_t deg,
				const SharedPtr<ProjectFile>& proj)
				: m_FrameIndex(frameIndex), m_Sel(std::move(sel)), m_Proj(proj),
				  m_Deg(deg) {}

			void Undo() override;
			void Redo() override;

			inline int32_t Deg() { return m_Deg; };
			inline WeakPtr<Selection> Sel() { return m_Sel; };

		private:
			unsigned long m_FrameIndex;
			WeakPtr<Selection> m_Sel;
			int32_t m_Deg;
			WeakPtr<ProjectFile> m_Proj;
	};

	class DeleteSelectionAction : public Action {
		public:
			DeleteSelectionAction(unsigned long frameIndex,
				WeakPtr<Selection> sel, std::vector<Col> prevPixels,
				const SharedPtr<ProjectFile>& proj)
				: m_FrameIndex(frameIndex), m_Sel(std::move(sel)),
				  m_PrevPixels(std::move(prevPixels)), m_Proj(proj) {}

			void Undo() override;
			void Redo() override;

			inline WeakPtr<Selection> Sel() { return m_Sel; };

		private:
			unsigned long m_FrameIndex;
			WeakPtr<Selection> m_Sel;
			std::vector<Col> m_PrevPixels;
			WeakPtr<ProjectFile> m_Proj;
	};
}  // namespace FuncDoodle
