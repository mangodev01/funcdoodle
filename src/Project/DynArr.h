/**
 * @file DynArr.h
 * @brief Dynamic container for storing animation frames in indexed order.
 *
 * This file defines LongIndexArray, a custom dynamic array used to store
 * and manage Frame objects for animation timelines in FuncDoodle.
 *
 * It provides:
 * - Random access to frames via index
 * - Insertion/removal before and after indices
 * - Frame reordering operations (move forward/backward)
 * - Lazy expansion via internal capacity management
 *
 * Frames are owned by the container and copied on insertion. The class
 * is responsible for lifetime management of all stored frames.
 *
 * @note This is not thread-safe.
 */

#pragma once

/**
 * @file DynArr.h
 * @brief Dynamic container for storing animation frames in indexed order.
 *
 * This file defines LongIndexArray, a custom dynamic array used to store
 * Frame objects for an animation sequence with O(1) random access.
 */

#include "Project/Frame.h"

namespace FuncDoodle {
	/**
	 * @class LongIndexArray
	 * @brief Dynamic array of Frame objects for animation storage.
	 *
	 * Ordered container holding all frames in an animation. Provides O(1)
	 * random access by index, amortized O(1) push to end, and O(n)
	 * insert/remove. Owns the frame data - frames are copied on insert, owned
	 * and destructed on remove.
	 *
	 * @invariant size
	 */
	class LongIndexArray {
		public:
		/**
		 * @fn LongIndexArray
		 * @brief Creates a frame array with default empty frames.
		 *
		 * @param width Width used for newly created empty frames.
		 * @param height Height used for newly created empty frames.
		 * @param bgCol Background color used for empty frames.
		 * @param initial_capacity Initial reserved frame capacity.
		 */
		LongIndexArray(
			int width, int height, Col bgCol, uint64_t initialcap = 10);
		~LongIndexArray();

		/**
		 * @fn PushBack
		 * @brief Appends a copy of a frame.
		 *
		 * @param value Frame to append.
		 */
		void PushBack(const Frame* value);
		/**
		 * @fn PushBackEmpty
		 * @brief Appends a newly created empty frame.
		 */
		void PushBackEmpty();
		/**
		 * @fn InsertAfterEmpty
		 * @brief Inserts an empty frame after an index.
		 *
		 * @param index Index after which to insert.
		 */
		void InsertAfterEmpty(uint64_t index);
		/**
		 * @fn InsertBeforeEmpty
		 * @brief Inserts an empty frame before an index.
		 *
		 * @param index Index before which to insert.
		 */
		void InsertBeforeEmpty(uint64_t index);
		/**
		 * @brief Inserts a frame copy after an index.
		 *
		 * @param index Index after which to insert.
		 * @param value Frame to copy.
		 */
		void InsertAfter(uint64_t index, const Frame* value);
		/**
		 * @brief Inserts a frame value after an index.
		 *
		 * @param index Index after which to insert.
		 * @param val Frame value to copy.
		 */
		void InsertAfter(uint64_t index, Frame val);
		/**
		 * @brief Inserts a frame copy before an index.
		 *
		 * @param index Index before which to insert.
		 * @param value Frame to copy.
		 */
		void InsertBefore(uint64_t index, const Frame* value);
		/**
		 * @brief Inserts a frame value before an index.
		 *
		 * @param index Index before which to insert.
		 * @param val Frame value to copy.
		 */
		void InsertBefore(uint64_t index, Frame val);
		/**
		 * @fn MoveBackward
		 * @brief Moves a frame one slot toward the beginning.
		 *
		 * @param i Index to move.
		 */
		void MoveBackward(uint64_t i);
		/**
		 * @fn MoveForward
		 * @brief Moves a frame one slot toward the end.
		 *
		 * @param i Index to move.
		 */
		void MoveForward(uint64_t i);
		/**
		 * @fn Remove
		 * @brief Removes a frame at an index.
		 *
		 * @param index Index to remove.
		 */
		void Remove(uint64_t index);

		/** @brief Returns mutable frame access by index. @param index Frame
		 * index. @return Pointer to the frame. */
		Frame* operator[](uint64_t index);
		/** @brief Returns immutable frame access by index. @param index Frame
		 * index. @return Pointer to the frame. */
		const Frame* operator[](uint64_t index) const;
		/**
		 * @fn Get
		 * @brief Returns mutable frame access by index.
		 *
		 * @param index Frame index.
		 * @return Pointer to the frame.
		 */
		Frame* Get(uint64_t index);

		/**
		 * @fn Size
		 * @brief Returns the number of stored frames.
		 *
		 * @return Current frame count.
		 */
		[[nodiscard]] uint64_t Size() const { return size; }
		/**
		 * @fn Capacity
		 * @brief Returns the reserved frame capacity.
		 *
		 * @return Current capacity.
		 */
		[[nodiscard]] uint64_t Capacity() const { return m_Capacity; }

		private:
		std::vector<Frame> m_Data;
		uint64_t m_Capacity;
		uint64_t size = 0;
		int m_Width;
		int m_Height;
		Col m_BG;

		void Resize(uint64_t newCap);
	};
}  // namespace FuncDoodle
