#include "Project/DynArr.h"
#include "Project/Frame.h"
#include "Conf/FuncPCH.h"

#include <algorithm>

namespace FuncDoodle {
	LongIndexArray::LongIndexArray(
		int width, int height, Col bgCol, unsigned long initialcap)
		: m_Capacity(initialcap), m_Width(width), m_Height(height),
		  m_BG(bgCol) {
		m_Capacity = std::max<unsigned long>(m_Capacity, 1);
		m_Width = std::max(m_Width, 1);
		m_Height = std::max(m_Height, 1);
		m_Data.reserve(m_Capacity);
	}

	LongIndexArray::~LongIndexArray() = default;

	void LongIndexArray::PushBack(const Frame* value) {
		if (!value) {
			FUNC_WARN("Tried to insert invalid frame");
			return;
		}

		if (size == m_Capacity) {
			Resize(m_Capacity * 2);
		}

		m_Data.push_back(*value);
		size = m_Data.size();
	}

	void LongIndexArray::PushBackEmpty() {
		if (size == m_Capacity) {
			Resize(m_Capacity * 2);
		}

		m_Data.emplace_back(m_Width, m_Height, m_BG);
		size = m_Data.size();
	}

	void LongIndexArray::InsertAfterEmpty(unsigned long index) {
		if (index >= size) {
			FUNC_ERR("index out of range");
			return;
		}

		if (size == m_Capacity) {
			Resize(m_Capacity * 2);
		}

		m_Data.insert(m_Data.begin() + static_cast<long>(index) + 1,
			Frame(m_Width, m_Height, m_BG));
		size = m_Data.size();
	}

	void LongIndexArray::InsertBeforeEmpty(unsigned long index) {
		if (index >= size) {
			FUNC_WARN("index out of range");
			return;
		}

		if (size == m_Capacity) {
			Resize(m_Capacity * 2);
		}

		m_Data.insert(m_Data.begin() + static_cast<long>(index),
			Frame(m_Width, m_Height, m_BG));
		size = m_Data.size();
	}

	void LongIndexArray::InsertAfter(unsigned long index, const Frame* value) {
		if (!value) {
			FUNC_WARN("Tried to insert invalid frame");
			return;
		}

		if (index >= size) {
			FUNC_WARN("index out of range");
			return;
		}

		if (size == m_Capacity) {
			Resize(m_Capacity * 2);
		}

		m_Data.insert(m_Data.begin() + static_cast<long>(index) + 1, *value);
		size = m_Data.size();
	}

	void LongIndexArray::InsertAfter(unsigned long index, const Frame val) {
		if (index >= size) {
			FUNC_WARN("index out of range");
			return;
		}

		if (size == m_Capacity) {
			Resize(m_Capacity * 2);
		}

		m_Data.insert(m_Data.begin() + static_cast<long>(index) + 1, val);
		size = m_Data.size();
	}

	void LongIndexArray::InsertBefore(unsigned long index, const Frame* value) {
		if (!value) {
			FUNC_WARN("tried to add invalid frame");
			return;
		}

		if (index >= size) {
			FUNC_WARN("index out of range");
			return;
		}

		if (size == m_Capacity) {
			Resize(m_Capacity * 2);
		}

		m_Data.insert(m_Data.begin() + static_cast<long>(index), *value);
		size = m_Data.size();
	}

	void LongIndexArray::InsertBefore(unsigned long index, const Frame val) {
		if (index >= size) {
			FUNC_WARN("index out of range");
			return;
		}

		if (size == m_Capacity) {
			Resize(m_Capacity * 2);
		}

		m_Data.insert(m_Data.begin() + static_cast<long>(index), val);
		size = m_Data.size();
	}

	void LongIndexArray::MoveBackward(unsigned long i) {
		if (i == 0 || i >= size)
			return;
		Frame temp = m_Data[i];
		m_Data[i] = m_Data[i - 1];
		m_Data[i - 1] = temp;
	}

	void LongIndexArray::MoveForward(unsigned long i) {
		if (i + 1 >= size)
			return;
		Frame temp = m_Data[i];
		m_Data[i] = m_Data[i + 1];
		m_Data[i + 1] = temp;
	}

	void LongIndexArray::Remove(unsigned long index) {
		if (index >= size) {
			FUNC_WARN("index out of range");
			return;
		}

		m_Data.erase(m_Data.begin() + static_cast<long>(index));
		size = m_Data.size();
	}

	Frame* LongIndexArray::operator[](unsigned long index) {
		if (index >= size) {
			FUNC_WARN("index out of range");
			return nullptr;
		}
		return &m_Data[index];
	}

	const Frame* LongIndexArray::operator[](unsigned long index) const {
		if (index >= size) {
			FUNC_WARN("index out of range");
			return nullptr;
		}
		return &m_Data[index];
	}

	Frame* LongIndexArray::Get(unsigned long index) {
		if (index >= size) {
			FUNC_WARN("index out of range");
			return nullptr;
		}
		return &m_Data[index];
	}

	void LongIndexArray::Resize(unsigned long newCap) {
		if (newCap <= m_Capacity)
			return;
		m_Data.reserve(newCap);
		m_Capacity = newCap;
	}
}  // namespace FuncDoodle
