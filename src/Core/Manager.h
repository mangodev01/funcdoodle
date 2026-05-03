/**
 * @file Manager.h
 * @brief Defines AnimationManager, the central coordinator for animation
 * playback, timeline UI, and editor integration.
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
 * @warning SetPlayer() must be called before replacing or assigning project
 * data.
 */

#pragma once

#include "Core/AppSettings.h"
#include "Keybinds/Keybinds.h"
#include "Project/Project.h"
#include "Util/Ptr.h"

#include "Drawing/EditorController.h"
#include "Rendering/FrameRenderer.h"

#include "Tool/ToolManager.h"

#include "Core/Player.h"

#include "Asset/AssetLoader.h"

namespace FuncDoodle {
	/**
	 * @class AnimationManager
	 * @brief Responsible for registering keybinds for animations, rendering
	 * timeline, logs window, controls window, etc.
	 *
	 * Manages animation playback and user interaction, including handling input
	 * bindings and coordinating UI elements such as the timeline, logs, and
	 * control panels. Keeps animation state in sync with the editor and ensures
	 * consistent behavior during playback and editing.
	 *
	 * @invariant m_Proj is always non-null after construction.
	 * @invariant m_Player is always non-null and references the same
	 * ProjectFile as m_Proj.
	 * @invariant m_SelectedFrame is always a valid index into
	 * m_Proj->AnimFrames().
	 * @invariant m_FrameRenderer and m_TimelineFrameRenderer are initialized
	 * before any render call.
	 * @invariant m_EditorController is always non-null.
	 * @invariant m_AssetLoader is non-null and owned externally.
	 * @invariant m_Settings and m_Keybinds remain valid for the lifetime of
	 * this instance.k
	 *
	 * @note This class does not own the AssetLoader.
	 * @warning SetPlayer() must be called before SetProj().
	 *
	 * @see AnimationPlayer
	 * @see FrameRenderer
	 */
	class AnimationManager {
		public:
		/**
		 * @fn AnimationManager
		 * @brief Creates an animation manager for a project and its UI state.
		 *
		 * @param proj Active project to manage.
		 * @param assetLoader Asset loader used for icons and sounds.
		 * @param editorController Shared editor controller used by frame
		 * renderers.
		 * @param keybinds Registry containing animation-related keybinds.
		 * @param settings Mutable application settings.
		 */
		AnimationManager(SharedPtr<ProjectFile> proj, AssetLoader* assetLoader,
			SharedPtr<EditorController> editorController,
			KeybindsRegistry& keybinds, AppSettings& settings);
		~AnimationManager();

		/**
		 * @fn RegisterKeybinds
		 * @brief Registers animation-specific keybinds.
		 */
		void RegisterKeybinds();
		/**
		 * @fn RenderTimeline
		 * @brief Renders the timeline UI.
		 *
		 * @param prevEnabled Whether previous-frame preview is enabled.
		 */
		void RenderTimeline(bool prevEnabled);
		/**
		 * @fn RenderControls
		 * @brief Renders playback and frame-management controls.
		 */
		void RenderControls();
		/**
		 * @fn RenderLogs
		 * @brief Renders animation-related log or debug output.
		 */
		static void RenderLogs();

		/**
		 * @fn Proj
		 * @brief Returns the currently managed project.
		 *
		 * @return Shared pointer to the active project.
		 */
		[[nodiscard]] SharedPtr<ProjectFile> Proj() const { return m_Proj; }
		/**
		 * @fn SetProj
		 * @brief Replaces the active project and updates the player to match.
		 *
		 * @param proj Project to manage.
		 */
		void SetProj(SharedPtr<ProjectFile> proj) {
			m_Proj = proj;
			m_Player->SetProj(proj);
		}
		/**
		 * @fn SetSettings
		 * @brief Rebinds the manager to a different settings object.
		 *
		 * @param settings Settings reference to store.
		 */
		void SetSettings(AppSettings& settings) { m_Settings = settings; }
		/**
		 * @fn Player
		 * @brief Returns the animation player.
		 *
		 * @return Pointer to the playback controller.
		 */
		[[nodiscard]] AnimationPlayer* Player() const { return m_Player.get(); }
		/**
		 * @fn SetPlayer
		 * @brief Replaces the owned animation player.
		 *
		 * @param player New player instance to own.
		 */
		void SetPlayer(AnimationPlayer* player) { m_Player.reset(player); }

		/**
		 * @fn SelectedFrameI
		 * @brief Returns the selected frame index.
		 *
		 * @return Current frame index.
		 */
		[[nodiscard]] unsigned long SelectedFrameI() const {
			return m_SelectedFrame;
		}
		/**
		 * @fn SelectedFrame
		 * @brief Returns the selected frame object.
		 *
		 * @return Pointer to the selected frame.
		 */
		Frame* SelectedFrame() {
			return m_Proj->AnimFrames()->Get(m_SelectedFrame);
		}

		/**
		 * @fn GetFrameRenderer
		 * @brief Returns the primary frame renderer.
		 *
		 * @return Owned frame renderer pointer wrapper.
		 */
		[[nodiscard]] const UniquePtr<FrameRenderer>& GetFrameRenderer() const {
			return m_FrameRenderer;
		}

		private:
		SharedPtr<ProjectFile> m_Proj;
		unsigned long m_SelectedFrame{0};
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
