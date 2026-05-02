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
 * @note This module does not own application state; it operates on provided
 * references.
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
		/**
		 * @var Proj
		 * @brief Project whose timeline is being edited.
		 */
		SharedPtr<ProjectFile> Proj;
		/**
		 * @var Player
		 * @brief Playback controller for the active project.
		 */
		AnimationPlayer* Player = nullptr;
		/**
		 * @var FrameRenderer
		 * @brief Renderer used for frame thumbnails and previews.
		 */
		FrameRenderer* FrameRenderer = nullptr;
		/**
		 * @var SelectedFrame
		 * @brief Pointer to the active timeline frame index.
		 */
		unsigned long* SelectedFrame = nullptr;
	};

	/**
	 * @class KeyHandler
	 * @brief Processes keybinds & shortcuts.
	 */
	class KeyHandler {
		public:
		/**
		 * @fn HandleTimelineShortcuts(TimelineKeyContext& context,
		 * KeybindsRegistry& keybinds)
		 * @brief Processes timeline-scoped shortcuts for the current frame.
		 */
		static void HandleTimelineShortcuts(
			TimelineKeyContext& context, KeybindsRegistry& keybinds);
		/**
		 * @fn RegisterKeybinds(KeybindsRegistry& keybinds)
		 * @brief Registers timeline shortcut bindings in the registry.
		 */
		static void RegisterKeybinds(KeybindsRegistry& keybinds);
	};
}  // namespace FuncDoodle
