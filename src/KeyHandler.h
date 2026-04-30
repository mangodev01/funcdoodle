/**
 * @file KeyHandler.h
 * @brief Handles keyboard shortcuts and timeline-related input actions.
 *
 * This file defines the KeyHandler system, which is responsible for:
 * - Processing global and timeline-specific keyboard shortcuts
 * - Registering application keybinds into the KeybindsRegistry
 * - Coordinating input actions with the animation timeline and player state
 *
 * It also defines TimelineKeyContext, a lightweight structure used to bundle
 * commonly required parameters for timeline shortcut processing.
 *
 * @note This module does not own application state; it operates on provided references.
 */

#pragma once

#include "FrameRenderer.h"
#include "Keybinds.h"
#include "Player.h"
#include "Project.h"
#include "Ptr.h"

namespace FuncDoodle {
	/**
	 * @struct TimelineKeyContext
	 * @brief Bundles commonly used parameters for timeline rendering.
	 *
	 * Provides a single struct to avoid passing multiple arguments into
	 * timeline rendering functions.
	 */
	struct TimelineKeyContext {
		SharedPtr<ProjectFile> Proj;
		AnimationPlayer* Player = nullptr;
		FrameRenderer* FrameRenderer = nullptr;
		unsigned long* SelectedFrame = nullptr;
	};

	/**
	 * @class KeyHandler
	 * @brief Processes keybinds & shortcuts.
	 */
	class KeyHandler {
		public:
			static void HandleTimelineShortcuts(
				TimelineKeyContext& context, KeybindsRegistry& keybinds);
			static void RegisterKeybinds(KeybindsRegistry& keybinds);
	};
}  // namespace FuncDoodle
