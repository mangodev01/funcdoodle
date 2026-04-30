/**
 * @file App.h
 * @brief Core FuncDoodle application class and runtime control interface.
 *
 * This header defines the Application class, which manages the main
 * lifecycle of FuncDoodle including initialization, rendering, project
 * management, input handling, theming, and editor coordination.
 */

#pragma once

/**
 * @file App.h
 * @brief Core FuncDoodle application class and runtime control interface.
 *
 * This header defines the Application class, which manages the main
 * event loop, window, UI, tools, and project lifecycle.
 */

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
	/**
	 * @class Application
	 * @brief Class holding most of the data required to launch FuncDoodle.
	 *
	 * Main application class.
	 *
	 * @invariant s_Instance != nullptr when application is running
	 * @invariant GetCurProj() may be nullptr but will never be invalid pointer
	 * @note Constructing multiple Application instances is unsupported, because
	 * it is a semi-singleton.
	 */
	class Application {
		public:
		Application();
		~Application();
		/**
		 * @fn Get
		 * @brief Returns the active application singleton.
		 *
		 * @return Current application instance.
		 */
		inline static Application* Get() { return s_Instance; };
		/**
		 * @fn Run
		 * @brief Runs the main application loop.
		 */
		void Run();
		/**
		 * @fn InitImGui
		 * @brief Initializes ImGui for the application window.
		 */
		void InitImGui();
		/**
		 * @fn RenderImGui
		 * @brief Renders one ImGui frame.
		 */
		void RenderImGui();
		/**
		 * @fn OpenFileDialog
		 * @brief Opens the project open dialog.
		 *
		 * @param done Callback invoked after dialog handling completes.
		 */
		void OpenFileDialog(std::function<void()> done);
		/**
		 * @fn SaveFileDialog
		 * @brief Opens the project save dialog.
		 *
		 * @param done Callback invoked after dialog handling completes.
		 */
		void SaveFileDialog(std::function<void()> done);
		/**
		 * @fn ReadProjectFile
		 * @brief Reads the currently selected project file from disk.
		 */
		void ReadProjectFile();
		/**
		 * @fn SaveProjectFile
		 * @brief Saves the current project using its active path.
		 */
		void SaveProjectFile();
		/**
		 * @fn RegisterKeybinds
		 * @brief Registers application-level keybinds.
		 */
		void RegisterKeybinds();
		/**
		 * @fn OpenSaveChangesDialog
		 * @brief Opens the unsaved-changes confirmation dialog.
		 */
		void OpenSaveChangesDialog();
		/**
		 * @fn SaveChangesDialogOpen
		 * @brief Returns whether the save-changes dialog is open.
		 *
		 * @return Popup open state.
		 */
		bool SaveChangesDialogOpen() {
			return m_UiManager.GetPopups().IsOpen("save_changes");
		}

		/**
		 * @fn HideCursor
		 * @brief Hides the application cursor.
		 */
		void HideCursor();
		/**
		 * @fn ShowCursor
		 * @brief Shows the application cursor.
		 */
		void ShowCursor();

		/**
		 * @fn ApplyThemeStyle
		 * @brief Applies an ImGui style to the application.
		 *
		 * @param themeStyle Theme style to copy into ImGui.
		 */
		static void ApplyThemeStyle(const ImGuiStyle& themeStyle) {
			ImGuiStyle& style = ImGui::GetStyle();
			style = themeStyle;
			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				style.WindowRounding = 1.0f;
			}
		}

		/**
		 * @fn ApplyThemeUuid
		 * @brief Applies a theme by UUID, falling back to the default theme.
		 *
		 * @param uuid Theme UUID to apply.
		 */
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

		/**
		 * @fn GetCurProj
		 * @brief Returns the currently open project.
		 *
		 * @return Shared pointer to the active project.
		 */
		inline SharedPtr<ProjectFile> GetCurProj() { return m_CurrentProj; }
		/**
		 * @fn SetCurProj
		 * @brief Sets the currently open project.
		 *
		 * @param proj Project to make active.
		 */
		inline void SetCurProj(SharedPtr<ProjectFile> proj) {
			m_CurrentProj = proj;
		}
		/**
		 * @fn GetCacheProj
		 * @brief Returns the cached project used during prompts and swaps.
		 *
		 * @return Cached project pointer.
		 */
		inline SharedPtr<ProjectFile> GetCacheProj() { return m_CacheProj; }
		/**
		 * @fn SetCacheProj
		 * @brief Sets the cached project reference.
		 *
		 * @param proj Project to cache.
		 */
		inline void SetCacheProj(SharedPtr<ProjectFile> proj) {
			m_CacheProj = proj;
		}
		/**
		 * @fn GetAssetLoader
		 * @brief Returns the global asset loader.
		 *
		 * @return Asset loader pointer.
		 */
		inline AssetLoader* GetAssetLoader() { return m_AssetLoader.get(); }
		/**
		 * @fn GetShouldClose
		 * @brief Returns whether the application should exit.
		 *
		 * @return Close flag.
		 */
		inline bool GetShouldClose() { return m_ShouldClose; }
		/**
		 * @fn GetTheme
		 * @brief Returns the currently selected theme UUID.
		 *
		 * @return Active theme UUID.
		 */
		inline UUID GetTheme() { return m_Theme; }
		/**
		 * @fn SetTheme
		 * @brief Sets the current theme UUID.
		 *
		 * @param theme Theme UUID to store.
		 */
		inline void SetTheme(UUID theme) { m_Theme = theme; }
		/**
		 * @fn SetFrameLimit
		 * @brief Sets the main-loop frame cap.
		 *
		 * @param frameLimit Target frame limit.
		 */
		inline void SetFrameLimit(double frameLimit) {
			m_Settings.FrameLimit = frameLimit;
		}
		/**
		 * @fn SetShouldClose
		 * @brief Sets whether the application should exit.
		 *
		 * @param shouldClose New close flag value.
		 */
		inline void SetShouldClose(bool shouldClose) {
			m_ShouldClose = shouldClose;
		}

		/**
		 * @fn GetWindow
		 * @brief Returns the mutable application window.
		 *
		 * @return Window pointer.
		 */
		inline Platform::Window* GetWindow() { return &m_Window; }
		/**
		 * @fn GetSettings
		 * @brief Returns mutable application settings.
		 *
		 * @return Settings reference.
		 */
		inline AppSettings& GetSettings() { return m_Settings; }
		/**
		 * @fn GetFrameTime
		 * @brief Returns target frame time derived from the frame limit.
		 *
		 * @return Seconds per frame.
		 */
		inline double GetFrameTime() { return 1.0 / m_Settings.FrameLimit; }
		/**
		 * @fn GetKeybinds
		 * @brief Returns the application keybind registry.
		 *
		 * @return Keybind registry reference.
		 */
		inline KeybindsRegistry& GetKeybinds() { return m_Keybinds; }
		/**
		 * @fn GetExportFormat
		 * @brief Returns the selected export format identifier.
		 *
		 * @return Export format value.
		 */
		inline int GetExportFormat() { return m_ExportFormat; }
		/**
		 * @fn GetUiManager
		 * @brief Returns the top-level UI manager.
		 *
		 * @return UI manager reference.
		 */
		inline UIManager& GetUiManager() { return m_UiManager; }
		/**
		 * @fn GetExportFormatPtr
		 * @brief Returns a mutable reference to the export format field.
		 *
		 * @return Export format reference.
		 */
		inline int& GetExportFormatPtr() { return m_ExportFormat; }
		/**
		 * @fn GetThemesPath
		 * @brief Returns the directory used for theme files.
		 *
		 * @return Themes directory path.
		 */
		inline std::filesystem::path GetThemesPath() { return m_ThemesPath; }
		/**
		 * @fn GetFrameLimitCache
		 * @brief Returns the cached previous frame limit.
		 *
		 * @return Cached frame limit reference.
		 */
		inline double& GetFrameLimitCache() { return m_FrameLimitCache; }
		/**
		 * @fn SetExportFormat
		 * @brief Sets the export format identifier.
		 *
		 * @param format Export format value.
		 */
		inline void SetExportFormat(int format) { m_ExportFormat = format; }

		/**
		 * @fn GetManager
		 * @brief Returns the animation manager.
		 *
		 * @return Animation manager pointer.
		 */
		inline AnimationManager* GetManager() { return m_Manager.get(); }

		/**
		 * @fn GetController
		 * @brief Returns the shared editor controller.
		 *
		 * @return Editor controller pointer.
		 */
		inline SharedPtr<EditorController> GetController() {
			return m_EditorController;
		}

		/**
		 * @brief Returns a copy of the application window wrapper.
		 *
		 * @return Window wrapper copy.
		 */
		inline Platform::Window GetWindow() const { return m_Window; }

		/**
		 * @fn UpdateFPS
		 * @brief Updates FPS tracking from frame delta time.
		 *
		 * @param deltaTime Frame time in seconds.
		 */
		void UpdateFPS(double deltaTime);
		/**
		 * @fn DropCallback
		 * @brief Handles file-drop events from the window.
		 *
		 * @param count Number of dropped paths.
		 * @param paths Array of dropped file paths.
		 */
		void DropCallback(int count, const char** paths);
		/**
		 * @fn Update
		 * @brief Updates per-frame application state outside direct rendering.
		 */
		void Update();
		/**
		 * @fn Save
		 * @brief Saves the current project and optionally exits.
		 *
		 * @param exit Whether to close the app after saving.
		 */
		void Save(bool exit = false);
		/**
		 * @fn SaveAt
		 * @brief Saves the current project at an explicit path.
		 *
		 * @param path Output project path.
		 */
		void SaveAt(const char* path);
		/**
		 * @fn RenderRotate
		 * @brief Renders the rotate-project UI.
		 */
		void RenderRotate();
		/**
		 * @fn Rotate
		 * @brief Rotates the active frame or selection by degrees.
		 *
		 * @param deg Rotation angle in degrees.
		 */
		void Rotate(int32_t deg);

		// lol idk what to call this
		/**
		 * @enum Where
		 * @brief Controls whether imported content is inserted before or after the current frame.
		 */
		enum class Where { Before = 0, After = 1 };
		/**
		 * @fn Import
		 * @brief Imports content relative to the current frame.
		 *
		 * @param where Whether to insert before or after.
		 */
		void Import(Where where);

		/**
		 * @fn DeleteCurrentSelection
		 * @brief Deletes the active selection contents.
		 */
		void DeleteCurrentSelection();
		/**
		 * @fn MoveCurrentSelection
		 * @brief Moves the active selection by one step.
		 *
		 * @param direction Direction to move in.
		 */
		void MoveCurrentSelection(Direction direction);
		/**
		 * @fn IsPosInFrame
		 * @brief Returns whether a screen-space point lies within the frame view.
		 *
		 * @param pos Position to test.
		 * @return Whether the position is inside the frame area.
		 */
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
