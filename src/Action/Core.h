/**
 * @file Action/Core.h
 * @brief Defines the base Action class for undo/redo functionality.
 *
 * This file contains the abstract Action interface used by all
 * editor operations that support undo/redo behavior.
 */


#pragma once

#include <utility>

#include "Common.h"
#include "Project.h"
#include "../Frame.h"

namespace FuncDoodle {

	/**
	 * @class DrawAction
	 * @brief Represents a single-pixel draw operation for undo/redo.
	 *
	 * Stores the previous and new color of a pixel so the change
	 * can be reverted or reapplied on a specific frame.
	 */
	class DrawAction : public Action {
		public:

		/**
		 * @brief Creates a draw action for a single pixel change.
		 */
		DrawAction(int x, int y, Col prev, Col next, unsigned long frameI,
			const SharedPtr<ProjectFile>& proj)
			: m_X(x), m_Y(y), m_Prev(prev), m_Next(next), m_FrameIndex(frameI),
			  m_Proj(proj) {};

		/**
		 * @brief Copy constructor.
		 */
		DrawAction(const DrawAction& other)
			 = default;

		/**
		 * @brief Destructor.
		 */
		~DrawAction() override = default;

		/**
		 * @brief Reverts the pixel to its previous color.
		 */
		void Undo() override;

		/**
		 * @brief Applies the new pixel color.
		 */
		void Redo() override;

		private:
		int m_X, m_Y;
		Col m_Prev;
		Col m_Next;
		unsigned long m_FrameIndex;
		WeakPtr<ProjectFile> m_Proj;
	};

	/**
	 * @class FillAction
	 * @brief Represents a bucket fill operation for undo/redo.
	 *
	 * Stores all affected pixels so the fill can be reversed or reapplied.
	 */
	class FillAction : public Action {
		public:

		/**
		 * @brief Creates a fill action from a flood-fill operation.
		 */
		FillAction(Col prev, Col next, unsigned long frameI,
			const SharedPtr<ProjectFile>& proj,
			std::vector<std::pair<int, int>> affected)
			: m_Prev(prev), m_Next(next), m_FrameIndex(frameI), m_Proj(proj),
			  m_Pixels(std::move(affected)) {};

		/**
		 * @brief Copy constructor.
		 */
		FillAction(const FillAction& other)
			
			  = default;

		/**
		 * @brief Destructor.
		 */
		~FillAction() override = default;

		/**
		 * @brief Reverts the fill operation.
		 */
		void Undo() override;

		/**
		 * @brief Reapplies the fill operation.
		 */
		void Redo() override;

		private:
		Col m_Prev;
		Col m_Next;
		unsigned long m_FrameIndex;
		WeakPtr<ProjectFile> m_Proj;
		std::vector<std::pair<int, int>> m_Pixels;
	};

	/**
	 * @class StrokeAction
	 * @brief Represents a multi-pixel stroke operation for undo/redo.
	 *
	 * Stores a list of pixel changes applied during a brush stroke.
	 */
	class StrokeAction : public Action {
		public:

		/**
		 * @struct PixelChange
		 * @brief Represents a single pixel modification in a stroke.
		 */
		struct PixelChange {
			int x;     ///< X coordinate of the changed pixel.
			int y;     ///< Y coordinate of the changed pixel.
			Col prev;  ///< Pixel color before the stroke touched it.
			Col next;  ///< Pixel color after the stroke applied.
		};

		/**
		 * @brief Creates a stroke action from a list of pixel changes.
		 */
		StrokeAction(unsigned long frameI, const SharedPtr<ProjectFile>& proj,
			std::vector<PixelChange> changes)
			: m_FrameIndex(frameI), m_Proj(proj),
			  m_Changes(std::move(changes)) {}

		/**
		 * @brief Reverts all pixel changes in the stroke.
		 */
		void Undo() override;

		/**
		 * @brief Reapplies all pixel changes in the stroke.
		 */
		void Redo() override;

		private:
		unsigned long m_FrameIndex;
		WeakPtr<ProjectFile> m_Proj;
		std::vector<PixelChange> m_Changes;
	};

}  // namespace FuncDoodle
