#pragma once

#include "Frame.h"

#include "AssetLoader.h"

#include "Keybinds.h"
#include "MacroUtils.h"

#include "Tool.h"

namespace FuncDoodle {
	class ToolManager {
		public:
		ToolManager(KeybindsRegistry& keybinds);
		~ToolManager();
		void RegisterKeybinds();
		void RenderTools();
		void Buttons();
		void Widgets();
		const ToolType GetSelectedTool() { return m_SelectedTool; }
		const float* GetCol() { return m_Col; }
		void SetCol(struct Col col) {
			unsigned char colArr[3] = {col.r, col.g, col.b};
			for (int j = 0; j < 3; j++) {
				m_Col[j] = static_cast<float>(colArr[j]) / 255.0f;
			}
		}
		const int GetSize() const { return m_Size; }
		void SetSize(int size) { m_Size = size; }

		private:
		ToolType m_SelectedTool = ToolType::Pencil;
		float m_Col[3] = {0.0f, 0.0f, 0.0f};
		int m_Size = 1;
		KeybindsRegistry& m_Keybinds;
	};
}  // namespace FuncDoodle
