/**
 * @file Action/Direction.h
 * @brief Defines direction enum and context for selection movement actions.
 *
 * This header provides the Direction enum used for moving selections
 * and the MoveSelectionActionContext structure, which bundles all data
 * required to execute a move-selection action within a project.
 */

#pragma once

#include "../Selection.h"
#include "Ptr.h"

namespace FuncDoodle {

	class ProjectFile;

	/**
	 * @enum Direction
	 * @brief Direction used for moving a selection.
	 */
	enum class Direction { None = 0, Left, Right, Up, Down };

	/**
	 * @struct MoveSelectionActionContext
	 * @brief Context data required to perform a selection move action.
	 *
	 * Contains the frame index, selection reference, movement direction,
	 * and a reference to the active project.
	 */
	struct MoveSelectionActionContext {
		public:
		/** Index of the frame being modified. */
		unsigned long FrameIndex;

		/** Selection to move (weak reference). */
		WeakPtr<Selection> Sel;

		/** Direction in which the selection should be moved. */
		Direction MoveDir;

		/** Active project owning the selection. */
		const SharedPtr<ProjectFile>& Proj;
	};

}  // namespace FuncDoodle
