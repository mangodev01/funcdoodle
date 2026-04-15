#pragma once

#include "AppSettings.h"
#include "AssetLoader.h"
#include "EditorController.h"
#include "Keybinds.h"
#include "Manager.h"
#include "Platform/Window.h"
#include "PopupRegistry.h"
#include "Project.h"
#include "Ptr.h"
#include "UIManager.h"
#include "UUID.h"

#include <chrono>
#include <functional>
#include <string>

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
			void RenderOptions();
			void SaveChangesDialog();
			void OpenSaveChangesDialog();
			bool SaveChangesDialogOpen() {
				return m_Popups.IsOpen("save_changes");
			}

			inline SharedPtr<ProjectFile> GetCurProj() { return m_CurrentProj; }
			inline void SetCurProj(SharedPtr<ProjectFile> proj) {
				m_CurrentProj = proj;
			}
			inline SharedPtr<ProjectFile> GetCacheProj() { return m_CacheProj; }
			inline void SetCacheProj(SharedPtr<ProjectFile> proj) {
				m_CacheProj = proj;
			}
			inline AssetLoader* GetAssetLoader() { return m_AssetLoader; }
			inline bool GetShouldClose() { return m_ShouldClose; }
			inline UUID GetTheme() { return m_Theme; }
			inline void SetTheme(UUID theme) { m_Theme = theme; }
			inline void SetFrameLimit(double frameLimit) {
				m_Settings.FrameLimit = frameLimit;
			}
			inline void SetShouldClose(bool shouldClose) {
				m_ShouldClose = shouldClose;
			}

			inline Platform::Window* GetWindow() { return &m_Window; }
			inline AppSettings& GetSettings() { return m_Settings; }
			inline double GetFrameTime() { return 1.0 / m_Settings.FrameLimit; }
			inline PopupRegistry* GetPopups() { return &m_Popups; }
			inline KeybindsRegistry& GetKeybinds() { return m_Keybinds; }
			inline int GetExportFormat() { return m_ExportFormat; }
			inline int* GetExportFormatPtr() { return &m_ExportFormat; }
			inline void SetExportFormat(int format) { m_ExportFormat = format; }

			inline AnimationManager* GetManager() { return m_Manager.get(); }

			inline SharedPtr<EditorController> GetController() {
				return m_EditorController;
			}

			void UpdateFPS(double deltaTime);
			void DropCallback(int count, const char** paths);
			void Update();
			void Save(bool exit = false);
			void SaveAt(const char* path);
			void RenderEditPrefs();
			void RenderRotate();
			void Rotate(int32_t deg);

			void DeleteCurrentSelection();
			void MoveCurrentSelection(Direction direction);

		private:
			std::string m_FilePath;
			int m_Deg = 90;
			UIManager m_UiManager;
			AppSettings m_Settings;
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
			bool m_ShouldClose = false;
			bool m_ShowTests = true;
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
