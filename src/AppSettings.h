/**
 * @file AppSettings.h
 * @brief Runtime configuration and user-adjustable application settings.
 *
 * This file defines the AppSettings struct, which stores global settings
 * for FuncDoodle and is typically loaded/saved via ImGui configuration files.
 *
 * Settings include toggles for audio, preview behavior, undo mode, and
 * performance-related frame limiting.
 *
 * @note These settings are expected to persist across sessions via ImGui state.
 */

#pragma once

namespace FuncDoodle {
	/**
	 * @struct AppSettings
	 * @brief Represents the application's current settings and loaded from imgui.ini.
	 */
	struct AppSettings {
		bool Sfx = false;
		bool Preview = true;
		bool UndoByStroke = true;
		float FrameLimit = 1000.0;
	};
}  // namespace FuncDoodle
