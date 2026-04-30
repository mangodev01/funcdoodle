/**
 * @file UIManager.h
 * @brief Central UI rendering manager for FuncDoodle.
 *
 * This file defines UIManager, which is responsible for:
 * - Rendering the main application UI
 * - Handling the main menu bar
 * - Managing application popups via PopupRegistry
 * - Providing access to UI configuration and preferences
 * - Dispatching UI-related actions such as project creation, export, and settings editing
 *
 * UIManager acts as the top-level coordinator for all immediate-mode UI rendering
 * in the application.
 *
 * @invariant m_CacheBGCol values are normalized in the range [0.0f, 1.0f].
 *
 * @see PopupRegistry
 */

#pragma once

#include "PopupRegistry.h"
#include <array>

namespace FuncDoodle {
	/**
	 * @class UIManager
	 * @brief Renders main FuncDoodle UI.
	 *
	 * Renders main menu bar, most popups and owns the PopupRegistry
	 *
	 * @invariant m_CacheBGCol values are in [0.0f, 1.0f]
	 *
	 * @see PopupRegistry
	 */
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
