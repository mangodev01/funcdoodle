/**
 * @file Action/Selection.h
 * @brief Undo/redo actions related to selection manipulation.
 *
 * This file defines actions that operate on selections, including
 * rotation, deletion, and movement. Each action stores enough state
 * to support full undo/redo behavior within the editor.
 */

#pragma once

#include "Common.h"
#include "../Selection.h"
#include "Project.h"
#include "Ptr.h"
#include <cstdint>

namespace FuncDoodle {

	/**
	 * @class RotateSelectionAction
	 * @brief Rotates a selection by a fixed angle in degrees.
	 */
	class RotateSelectionAction : public Action {
	public:
		/**
		 * @brief Constructs a rotation action.
		 * @param frameIndex Frame in which the rotation occurs.
		 * @param sel Selection to rotate.
		 * @param deg Rotation amount in degrees.
		 * @param proj Owning project reference.
		 */
		RotateSelectionAction(unsigned long frameIndex,
			WeakPtr<Selection> sel,
			int32_t deg,
			const SharedPtr<ProjectFile>& proj)
			: m_FrameIndex(frameIndex),
			  m_Sel(std::move(sel)),
			  m_Proj(proj),
			  m_Deg(deg) {}

		/**
		 * @brief Undoes the rotation.
		 */
		void Undo() override;

		/**
		 * @brief Reapplies the rotation.
		 */
		void Redo() override;

		/**
		 * @brief Returns rotation amount in degrees.
		 */
		[[nodiscard]] int32_t Deg() const { return m_Deg; }

		/**
		 * @brief Returns the affected selection.
		 */
		[[nodiscard]] WeakPtr<Selection> Sel() const { return m_Sel; }

	private:
		unsigned long m_FrameIndex;        ///< Frame index where action occurred
		WeakPtr<Selection> m_Sel;         ///< Target selection
		int32_t m_Deg;                    ///< Rotation in degrees
		WeakPtr<ProjectFile> m_Proj;      ///< Owning project
	};

	/**
	 * @class DeleteSelectionAction
	 * @brief Deletes a selection and stores previous pixel data for undo.
	 */
	class DeleteSelectionAction : public Action {
	public:
		/**
		 * @brief Constructs a delete-selection action.
		 * @param frameIndex Frame in which deletion occurs.
		 * @param sel Selection to delete.
		 * @param prevPixels Pixels removed by deletion.
		 * @param proj Owning project reference.
		 */
		DeleteSelectionAction(unsigned long frameIndex,
			WeakPtr<Selection> sel,
			std::vector<Col> prevPixels,
			const SharedPtr<ProjectFile>& proj)
			: m_FrameIndex(frameIndex),
			  m_Sel(std::move(sel)),
			  m_PrevPixels(std::move(prevPixels)),
			  m_Proj(proj) {}

		/**
		 * @brief Restores deleted pixels.
		 */
		void Undo() override;

		/**
		 * @brief Re-applies deletion.
		 */
		void Redo() override;

		/**
		 * @brief Returns affected selection.
		 */
		[[nodiscard]] WeakPtr<Selection> Sel() const { return m_Sel; }

	private:
		unsigned long m_FrameIndex;            ///< Frame index of action
		WeakPtr<Selection> m_Sel;             ///< Target selection
		std::vector<Col> m_PrevPixels;        ///< Stored pixels for undo
		WeakPtr<ProjectFile> m_Proj;          ///< Owning project
	};

	/**
	 * @class MoveSelectionAction
	 * @brief Moves a selection in a given direction.
	 */
	class MoveSelectionAction : public Action {
	public:
		/**
		 * @brief Constructs a move-selection action.
		 * @param frame Frame state before movement.
		 * @param ctx Movement context (direction, selection, project, etc.)
		 */
		MoveSelectionAction(Frame frame, MoveSelectionActionContext ctx)
			: m_FrameBeforeMove(frame),
			  m_Ctx(ctx) {}

		/**
		 * @brief Undoes movement.
		 */
		void Undo() override;

		/**
		 * @brief Reapplies movement.
		 */
		void Redo() override;

		/**
		 * @brief Returns selection being moved.
		 */
		[[nodiscard]] WeakPtr<Selection> Sel() const { return m_Ctx.Sel; }

		/**
		 * @brief Returns movement direction.
		 */
		[[nodiscard]] Direction Dir() const { return m_Ctx.MoveDir; }

		/**
		 * @brief Returns owning project.
		 */
		[[nodiscard]] SharedPtr<ProjectFile> Proj() const { return m_Ctx.Proj; }

		/**
		 * @brief Returns frame index.
		 */
		[[nodiscard]] unsigned long FrameIndex() const { return m_Ctx.FrameIndex; }

		/**
		 * @brief Returns full context snapshot.
		 */
		[[nodiscard]] MoveSelectionActionContext Ctx() const { return m_Ctx; }

	private:
		MoveSelectionActionContext m_Ctx;   ///< Movement context
		Frame m_FrameBeforeMove;            ///< Frame state before move
	};

} // namespace FuncDoodle
