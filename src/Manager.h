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
	class AnimationManager {
		public:
			AnimationManager(SharedPtr<ProjectFile> proj,
				AssetLoader* assetLoader,
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
			void SetSettings(AppSettings& settings) {
				m_Settings = settings;
			}
			AnimationPlayer* Player() const { return m_Player.get(); }
			void SetPlayer(AnimationPlayer* player) { m_Player.reset(player); }

			unsigned long SelectedFrameI() const { return m_SelectedFrame; }
			Frame* SelectedFrame() {
				return m_Proj->AnimFrames()->Get(m_SelectedFrame);
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
