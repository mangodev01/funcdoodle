#pragma once

#include "Common.h"
#include "Project.h"
#include <optional>

namespace FuncDoodle {
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
			  m_Frame(frame ? std::optional<Frame>(*frame) : std::nullopt) {}

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
			  m_Frame(frame ? std::optional<Frame>(*frame) : std::nullopt) {}

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

		inline int32_t Deg() const { return m_Deg; };

		private:
		unsigned long m_FrameIndex;
		int32_t m_Deg;
		WeakPtr<ProjectFile> m_Proj;
	};
}
