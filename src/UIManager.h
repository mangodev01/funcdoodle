#pragma once

#include "AppSettings.h"
#include "AssetLoader.h"
#include "EditorController.h"
#include "Keybinds.h"
#include "Manager.h"
#include "Platform/Window.h"
#include "PopupRegistry.h"
#include "Project.h"

#include <array>

namespace FuncDoodle {
	class UIManager {
		public:
			UIManager();

			void SaveChanges();
			void MainMenuBar();
			void Keybinds();
			void CheckKeybinds();
			void NewProj();
			void EditProj();
			void ExportProj();

		private:
			const char* m_WaitingForKey = nullptr;
			std::array<float, 3> m_CacheBGCol = {1.0f, 1.0f, 1.0f};
			int m_ExportFormat = 0;
			bool m_SFXEnabled = true;
	};
}  // namespace FuncDoodle
