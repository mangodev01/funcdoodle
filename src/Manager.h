/**
 * @file Manager.h
 * @brief Defines AnimationManager, the central coordinator for animation playback, timeline UI, and editor integration.
 *
 * This file contains the AnimationManager class, which is responsible for:
 * - Managing animation playback state via AnimationPlayer
 * - Rendering and updating the timeline UI
 * - Handling control panels and logs UI
 * - Coordinating editor interaction through EditorController
 * - Managing frame selection and rendering via FrameRenderer
 * - Integrating tool input through ToolManager
 *
 * The manager acts as a high-level orchestrator between the project data,
 * rendering system, input system, and editor tools.
 *
 * It ensures synchronization between playback state and editing state,
 * and provides the main UI entry points for animation-related controls.
 *
 * @note AnimationManager does not own AssetLoader.
 * @warning SetPlayer() must be called before replacing or assigning project data.
 */

#pragma once

#include "AppSettings.h"
#include "Keybinds.h"
#include "Project.h"
#include "Ptr.h"

#include "EditorController.h"
#include "FrameRenderer.h"

#include "ToolManager.h"

#include "Player.h"

#include "AssetLoader.h"

#include "Ptr.h"

namespace FuncDoodle {
	/**
	 * @class AnimationManager
	 * @brief Responsible for registering keybinds for animations, rendering timeline, logs window, controls window, etc.
	 *
	 * Manages animation playback and user interaction, including handling input bindings
	 * and coordinating UI elements such as the timeline, logs, and control panels.
	 * Keeps animation state in sync with the editor and ensures consistent behavior
	 * during playback and editing.
	 *
	 * @invariant m_Proj is always non-null after construction.
	 * @invariant m_Player is always non-null and references the same ProjectFile as m_Proj.
	 * @invariant m_SelectedFrame is always a valid index into m_Proj->AnimFrames().
	 * @invariant m_FrameRenderer and m_TimelineFrameRenderer are initialized before any render call.
	 * @invariant m_EditorController is always non-null.
	 * @invariant m_AssetLoader is non-null and owned externally.
	 * @invariant m_Settings and m_Keybinds remain valid for the lifetime of this instance.k
	 *
	 * @note This class does not own the AssetLoader.
	 * @warning SetPlayer() must be called before SetProj().
	 *
	 * @see AnimationPlayer
	 * @see FrameRenderer
	 */
	class AnimationManager {
		public:
		AnimationManager(SharedPtr<ProjectFile> proj, AssetLoader* assetLoader,
			SharedPtr<EditorController> editorController,
			KeybindsRegistry& keybinds, AppSettings& settings);
		~AnimationManager();

		void RegisterKeybinds();
		void RenderTimeline(bool prevEnabled);
		void RenderControls();
		void RenderLogs();

		const SharedPtr<ProjectFile> Proj() const { return m_Proj; }
		void SetProj(SharedPtr<ProjectFile> proj) {
			m_Proj = proj;
			m_Player->SetProj(proj);
		}
		void SetSettings(AppSettings& settings) { m_Settings = settings; }
		AnimationPlayer* Player() const { return m_Player.get(); }
		void SetPlayer(AnimationPlayer* player) { m_Player.reset(player); }

		unsigned long SelectedFrameI() const { return m_SelectedFrame; }
		Frame* SelectedFrame() {
			return m_Proj->AnimFrames()->Get(m_SelectedFrame);
		}

		inline const UniquePtr<FrameRenderer>& GetFrameRenderer() const {
			return m_FrameRenderer;
		}

		private:
		SharedPtr<ProjectFile> m_Proj;
		unsigned long m_SelectedFrame;
		UniquePtr<FrameRenderer> m_FrameRenderer;
		UniquePtr<FrameRenderer> m_TimelineFrameRenderer;
		UniquePtr<ToolManager> m_ToolManager;
		UniquePtr<AnimationPlayer> m_Player;
		SharedPtr<EditorController> m_EditorController;
		KeybindsRegistry& m_Keybinds;
		AssetLoader* m_AssetLoader;
		AppSettings& m_Settings;
	};
}  // namespace FuncDoodle
