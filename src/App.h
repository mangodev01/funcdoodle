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
#include "Themes.h"
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
		void OpenSaveChangesDialog();
		bool SaveChangesDialogOpen() {
			return m_UiManager.GetPopups().IsOpen("save_changes");
		}

		void HideCursor();
		void ShowCursor();

		static void ApplyThemeStyle(const ImGuiStyle& themeStyle) {
			ImGuiStyle& style = ImGui::GetStyle();
			style = themeStyle;
			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				style.WindowRounding = 1.0f;
			}
		}

		static void ApplyThemeUuid(const FuncDoodle::UUID& uuid) {
			auto it = FuncDoodle::Themes::g_Themes.find(uuid);
			if (it != FuncDoodle::Themes::g_Themes.end()) {
				ApplyThemeStyle(it->second.Style);
				return;
			}
			auto defUuid = FuncDoodle::UUID::FromString(
				FuncDoodle::Themes::s_DefaultTheme);
			auto defIt = FuncDoodle::Themes::g_Themes.find(defUuid);
			if (defIt != FuncDoodle::Themes::g_Themes.end()) {
				ApplyThemeStyle(defIt->second.Style);
			}
		}

		inline SharedPtr<ProjectFile> GetCurProj() { return m_CurrentProj; }
		inline void SetCurProj(SharedPtr<ProjectFile> proj) {
			m_CurrentProj = proj;
		}
		inline SharedPtr<ProjectFile> GetCacheProj() { return m_CacheProj; }
		inline void SetCacheProj(SharedPtr<ProjectFile> proj) {
			m_CacheProj = proj;
		}
		inline AssetLoader* GetAssetLoader() { return m_AssetLoader.get(); }
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
		inline KeybindsRegistry& GetKeybinds() { return m_Keybinds; }
		inline int GetExportFormat() { return m_ExportFormat; }
		inline int& GetExportFormatPtr() { return m_ExportFormat; }
		inline std::filesystem::path GetThemesPath() { return m_ThemesPath; }
		inline double& GetFrameLimitCache() { return m_FrameLimitCache; }
		inline void SetExportFormat(int format) { m_ExportFormat = format; }

		inline AnimationManager* GetManager() { return m_Manager.get(); }

		inline SharedPtr<EditorController> GetController() {
			return m_EditorController;
		}

		inline Platform::Window GetWindow() const { return m_Window; }

		void UpdateFPS(double deltaTime);
		void DropCallback(int count, const char** paths);
		void Update();
		void Save(bool exit = false);
		void SaveAt(const char* path);
		void RenderRotate();
		void Rotate(int32_t deg);

		void DeleteCurrentSelection();
		void MoveCurrentSelection(Direction direction);
		bool IsPosInFrame(ImVec2 pos);

		private:
		std::string m_FilePath;
		UIManager m_UiManager;
		AppSettings m_Settings;
		KeybindsRegistry m_Keybinds;
		SharedPtr<ProjectFile> m_CurrentProj;
		SharedPtr<ProjectFile> m_CacheProj;
		UniquePtr<AnimationManager> m_Manager;
		Platform::Window m_Window;
		UniquePtr<AssetLoader> m_AssetLoader;
		SharedPtr<EditorController> m_EditorController;
		int m_ExportFormat = 0;
		UUID m_Theme;
		bool m_ShouldClose = false;
		bool m_ShowTests = true;
		double m_FrameLimitCache = 1000.0;
		double m_LastFrameTime = 0.0;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_LastFrame;
		double m_FPS = 0.0;
		std::array<float, 3> m_CacheBGCol;
		std::filesystem::path m_ThemesPath;
		static Application* s_Instance;
	};
}  // namespace FuncDoodle
