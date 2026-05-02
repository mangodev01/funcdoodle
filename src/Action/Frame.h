/**
 * @file Action/Frame.h
 * @brief Undo/redo actions for frame-level operations.
 *
 * This file contains actions that operate on animation frames,
 * including insertion, deletion, movement, and rotation.
 * Each action stores enough state to fully support undo/redo.
 */

#pragma once

#include "Common.h"
#include "Project.h"
#include <optional>

namespace FuncDoodle {

	/**
	 * @class DeleteFrameAction
	 * @brief Removes a frame from a project with undo support.
	 */
	class DeleteFrameAction : public Action {
	public:
		/**
		 * @brief Creates an empty delete-frame action.
		 */
		DeleteFrameAction(unsigned long frameI,
			const SharedPtr<ProjectFile>& proj)
			: m_Proj(proj),
			  m_FrameIndex(frameI),
			  m_Empty(true),
			  m_Frame(std::nullopt) {}

		/**
		 * @brief Creates a delete-frame action storing frame data.
		 * @param frameI Index of the frame to remove.
		 * @param frame Frame to delete (copied internally).
		 * @param proj Project that owns the frame.
		 */
		DeleteFrameAction(unsigned long frameI,
			Frame* frame,
			const SharedPtr<ProjectFile>& proj)
			: m_Proj(proj),
			  m_FrameIndex(frameI),
			  m_Empty(frame == nullptr),
			  m_Frame(frame ? std::optional<Frame>(*frame) : std::nullopt) {}

		/**
		 * @brief Restores deleted frame.
		 */
		void Undo() override;

		/**
		 * @brief Re-applies frame deletion.
		 */
		void Redo() override;

	private:
		unsigned long m_FrameIndex;     ///< Frame index affected
		bool m_Empty;                   ///< Whether frame data exists
		std::optional<Frame> m_Frame;   ///< Stored frame data for undo
		WeakPtr<ProjectFile> m_Proj;   ///< Owning project
	};

	/**
	 * @class InsertFrameAction
	 * @brief Inserts a frame into a project with undo support.
	 */
	class InsertFrameAction : public Action {
	public:
		/**
		 * @brief Creates an empty insert-frame action.
		 */
		InsertFrameAction(unsigned long frameI,
			const SharedPtr<ProjectFile>& proj)
			: m_Proj(proj),
			  m_FrameIndex(frameI),
			  m_Empty(true),
			  m_Frame(std::nullopt) {}

		/**
		 * @brief Creates an insert-frame action with frame data.
		 */
		InsertFrameAction(unsigned long frameI,
			Frame* frame,
			const SharedPtr<ProjectFile>& proj)
			: m_FrameIndex(frameI),
			  m_Proj(proj),
			  m_Empty(frame == nullptr),
			  m_Frame(frame ? std::optional<Frame>(*frame) : std::nullopt) {}

		/**
		 * @brief Removes inserted frame.
		 */
		void Undo() override;

		/**
		 * @brief Re-inserts frame.
		 */
		void Redo() override;

	private:
		unsigned long m_FrameIndex;     ///< Frame index affected
		bool m_Empty;                   ///< Whether frame data exists
		std::optional<Frame> m_Frame;   ///< Stored frame data
		WeakPtr<ProjectFile> m_Proj;   ///< Owning project
	};

	/**
	 * @class MoveFrameLeftAction
	 * @brief Moves a frame one position to the left.
	 */
	class MoveFrameLeftAction : public Action {
	public:
		/** @brief Creates an action that shifts a frame one slot to the left. */
		MoveFrameLeftAction(unsigned long frameI,
			const SharedPtr<ProjectFile>& proj)
			: m_Proj(proj),
			  m_FrameIndex(frameI) {}

		void Undo() override;
		void Redo() override;

	private:
		unsigned long m_FrameIndex;   ///< Frame index
		WeakPtr<ProjectFile> m_Proj; ///< Owning project
	};

	/**
	 * @class MoveFrameRightAction
	 * @brief Moves a frame one position to the right.
	 */
	class MoveFrameRightAction : public Action {
	public:
		/** @brief Creates an action that shifts a frame one slot to the right. */
		MoveFrameRightAction(unsigned long frameI,
			const SharedPtr<ProjectFile>& proj)
			: m_Proj(proj),
			  m_FrameIndex(frameI) {}

		void Undo() override;
		void Redo() override;

	private:
		unsigned long m_FrameIndex;   ///< Frame index
		WeakPtr<ProjectFile> m_Proj; ///< Owning project
	};

	/**
	 * @class RotateFrameAction
	 * @brief Rotates a frame by a fixed angle.
	 */
	class RotateFrameAction : public Action {
	public:
		/**
		 * @brief Creates a frame rotation action.
		 * @param frameIndex Frame to rotate
		 * @param deg Rotation in degrees
		 * @param proj Owning project
		 */
		RotateFrameAction(unsigned long frameIndex,
			int32_t deg,
			const SharedPtr<ProjectFile>& proj)
			: m_FrameIndex(frameIndex),
			  m_Proj(proj),
			  m_Deg(deg) {}

		void Undo() override;
		void Redo() override;

		/**
		 * @brief Returns rotation angle in degrees.
		 */
		[[nodiscard]] int32_t Deg() const { return m_Deg; }

	private:
		unsigned long m_FrameIndex;   ///< Frame index
		int32_t m_Deg;               ///< Rotation angle
		WeakPtr<ProjectFile> m_Proj; ///< Owning project
	};

} // namespace FuncDoodle
