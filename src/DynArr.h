#pragma once

#include <iostream>

#include "Frame.h"

namespace FuncDoodle {
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
