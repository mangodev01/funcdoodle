/**
 * @file Action/Common.h
 * @brief Defines the base interface for undoable and redoable editor actions.
 *
 * This header declares the Action class, which represents a single operation
 * that can be applied and reverted within the application’s undo/redo system.
 * Typical actions include drawing, filling, and deleting frames.
 */

#pragma once

namespace FuncDoodle {
	/**
	 * @class Action
	 * @brief Base interface for undoable and redoable editor actions.
	 *
	 * Represents a single operation that can be applied and reverted
	 * within the application’s undo/redo system (e.g. drawing, filling,
	 * deleting frames).
	 *
	 * Derived classes must implement both Undo() and Redo() to define
	 * how the action is reversed and reapplied.
	 */
	class Action {
		public:
		/**
		 * @brief Reverts the effects of this action.
		 */
		virtual void Undo() = 0;

		/**
		 * @brief Reapplies the effects of this action.
		 */
		virtual void Redo() = 0;

		/**
		 * @brief Virtual destructor for safe polymorphic destruction.
		 */
		virtual ~Action() = default;

		// stopped adding friends because that is unnecessary
		// friend class DrawAction;
		// friend class FillAction;
		// friend class DeleteFrameAction;
	};
}  // namespace FuncDoodle
