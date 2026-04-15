#pragma once

#include "AssetLoader.h"
#include "EditorController.h"
#include "Keybinds.h"
#include "Manager.h"
#include "Platform/Window.h"
#include "PopupRegistry.h"
#include "Project.h"
#include "Ptr.h"
#include "UUID.h"

#include <chrono>
#include <functional>
#include <string>

#include "Ptr.h"

namespace FuncDoodle {
	class Application {
		public:
			Application();
			~Application();
			inline static Application* Get() { return s_Instance; };
			void Run();
			void InitImGui();
			void RenderImGui();
			void OpenFileDialog(std::function<void()> done);
			void SaveFileDialog(std::function<void()> done);
			void ReadProjectFile();
			void SaveProjectFile();
			void RegisterKeybinds();
			void RegisterPopups();
			void CheckKeybinds();
			void RenderOptions();
			void SaveChangesDialog();
			void OpenSaveChangesDialog();
			bool SaveChangesDialogOpen() {
				return m_Popups.IsOpen("save_changes");
			}
			inline SharedPtr<ProjectFile> CurProj() { return m_CurrentProj; }
			inline SharedPtr<ProjectFile> CacheProj() { return m_CacheProj; }
			inline bool ShouldClose() { return m_ShouldClose; }
			inline UUID Theme() { return m_Theme; }
			inline void SetTheme(UUID theme) { m_Theme = theme; }
			inline void SetSFXEnabled(bool enabled) { m_SFXEnabled = enabled; }
			inline void SetPrevEnabled(bool enabled) {
				m_PrevEnabled = enabled;
			}
			inline void SetUndoByStroke(bool enabled) {
				m_UndoByStroke = enabled;
				if (m_Manager) {
					m_Manager->SetUndoByStroke(enabled);
				}
			}
			inline void SetFrameLimit(double frameLimit) {
				m_FrameLimit = frameLimit;
			}
			inline bool PrevEnabled() { return m_PrevEnabled; }
			inline bool SFXEnabled() { return m_SFXEnabled; }
			inline bool UndoByStroke() { return m_UndoByStroke; }
			inline double FrameTime() { return 1.0 / m_FrameLimit; }
			inline double FrameLimit() { return m_FrameLimit; }
			inline PopupRegistry* Popups() { return &m_Popups; }
			void UpdateFPS(double deltaTime);
			void DropCallback(int count, const char** paths);
			void RenderEditProj();
			void RenderNewProj();
			void Update();
			void Save(bool exit = false);
			void SaveAt(const char* path);
			void RenderMainMenuBar();
			void RenderEditPrefs();
			void RenderRotate();
			void RenderExport();
			void RenderKeybinds();
			void Rotate(int32_t deg);

			void DeleteCurrentSelection();
			void MoveCurrentSelection(Direction direction);

		private:
			std::string m_FilePath;
			int m_Deg = 90;
			PopupRegistry m_Popups;
			KeybindsRegistry m_Keybinds;
			SharedPtr<ProjectFile> m_CurrentProj;
			SharedPtr<ProjectFile> m_CacheProj;
			UniquePtr<AnimationManager> m_Manager;
			Platform::Window m_Window;
			AssetLoader* m_AssetLoader;
			SharedPtr<EditorController> m_EditorController;
			int m_ExportFormat = 0;
			UUID m_Theme;
			const char* m_WaitingForKey = nullptr;
			bool m_ShouldClose = false;
			bool m_SFXEnabled = true;
			bool m_PrevEnabled = false;
			bool m_UndoByStroke = false;
			bool m_ShowTests = true;
			double m_FrameLimit = 1000.0;
			double m_FrameLimitCache = 1000.0;
			double m_LastFrameTime = 0.0;
			std::chrono::time_point<std::chrono::high_resolution_clock>
				m_LastFrame;
			double m_FPS = 0.0;
			std::array<float, 3> m_CacheBGCol;
			std::filesystem::path m_ThemesPath;
			static Application* s_Instance;
	};
}  // namespace FuncDoodle
