#pragma once

#include "PopupRegistry.h"
#include <array>

namespace FuncDoodle {
	class UIManager {
		public:
		UIManager();

		void Render();
		void Options();

		inline PopupRegistry& GetPopups() { return m_Popups; }

		private:
		void SaveChanges();
		void MainMenuBar();
		void Keybinds();
		void CheckKeybinds();
		void NewProj();
		void EditProj();
		void ExportProj();
		void EditPrefs();
		void Rotate();

		private:
		const char* m_WaitingForKey = nullptr;
		std::array<float, 3> m_CacheBGCol = {1.0f, 1.0f, 1.0f};
		PopupRegistry m_Popups;
		int m_Deg = 90;
	};
}  // namespace FuncDoodle
