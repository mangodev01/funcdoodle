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

#include "Frame.h"

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
		LongIndexArray(int width, int height, Col bgCol,
			unsigned long initial_capacity = 10);
		~LongIndexArray();

		void PushBack(const Frame* value);
		void PushBackEmpty();
		void InsertAfterEmpty(unsigned long index);
		void InsertBeforeEmpty(unsigned long index);
		void InsertAfter(unsigned long index, const Frame* value);
		void InsertAfter(unsigned long index, const Frame val);
		void InsertBefore(unsigned long index, const Frame* value);
		void InsertBefore(unsigned long index, const Frame val);
		void MoveBackward(unsigned long i);
		void MoveForward(unsigned long i);
		void Remove(unsigned long index);

		Frame* operator[](unsigned long index);
		const Frame* operator[](unsigned long index) const;
		Frame* Get(unsigned long index);

		inline unsigned long Size() const { return size; }
		inline unsigned long Capacity() const { return m_Capacity; }

		private:
		std::vector<Frame> m_Data;
		unsigned long m_Capacity;
		unsigned long size = 0;
		int m_Width;
		int m_Height;
		Col m_BG;

		void Resize(unsigned long new_capacity);
	};
}  // namespace FuncDoodle
