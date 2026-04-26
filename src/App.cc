#include "AssetLoader.h"
#include "Constants.h"
#include "Frame.h"
#include "Gui.h"
#include "KeyHandler.h"
#include "MacroUtils.h"
#include <glad/glad.h>

#include "Action/Action.h"

#include "App.h"

#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <imgui.h>
#include <iostream>
#include <stdint.h>
#include <string.h>
#include <string>
#include <thread>

#include <stb_image.h>

#include "LoadedAssets.h"
#include "Platform/Window.h"
#include "TextUtil.h"

#include "MacroUtils.h"

#include "ImUtil.h"
#include "Themes.h"
#include "Tool.h"

#include "exepath.h"
#include "imgui_te_ui.h"
#include "nfd.h"

#include "Keybinds.h"

#include "Test.h"

namespace FuncDoodle {
	static constexpr const char* g_SupportedExtensionsForImporting =
		"png,jpg,jpeg,bmp,tga,psd,gif,hdr,pic,ppm,pgm,pnm";

	Application* Application::s_Instance = nullptr;
	Application::Application()
		: m_FilePath(""), m_CurrentProj(nullptr), m_CacheProj(nullptr),
		  m_EditorController(std::make_shared<EditorController>()),
		  m_CacheBGCol({g_MaxColorValue, g_MaxColorValue, g_MaxColorValue}),
		  m_Theme(UUID::FromString("d0c1a009-d09c-4fe6-84f8-eddcb2da38f9")),
		  m_FrameLimitCache(m_Settings.FrameLimit),
		  m_Keybinds(std::filesystem::path("")),
		  m_Window({.Width = g_DefaultWindowWidth,
			  .Height = g_DefaultWindowHeight,
			  .Title = "",
			  .Monitor = -1}) {
#ifdef FUNCDOODLE_BUILD_TESTS
		FuncDoodle_RunTests();
		FuncDoodle::TestRegistry::Instance().PrintSummary();

		std::exit(0);
#endif

		std::filesystem::path path = exepath::get();

		std::filesystem::path rootPath = path.parent_path();

		m_AssetLoader = std::make_unique<AssetLoader>(rootPath / "assets");

		m_ThemesPath = rootPath / "themes";

		m_Keybinds = KeybindsRegistry(rootPath);
		m_FrameLimitCache = m_Settings.FrameLimit;
		m_Manager = std::make_unique<AnimationManager>(nullptr,
			m_AssetLoader.get(), m_EditorController, m_Keybinds, m_Settings),

		RegisterKeybinds();

		auto title = FUNC_FMT("FuncDoodle {}", FUNCVER);
		m_Window.SetTitle(title.c_str());

		m_AssetLoader->LoadAssets();

		m_Window.SetDropCallback(
			[](Platform::Window*, int count, const char** paths) {
				Get()->DropCallback(count, paths);
			});

		m_Window.SetCloseCallback([](Platform::Window* win) {
			win->SetShouldClose(false);
			if (Get()->GetCurProj()) {
				Get()->OpenSaveChangesDialog();
			} else {
				win->SetShouldClose(true);
			}
		});

#ifdef DEBUG
		m_Window.SetErrorCallback([](int err, const char* desc) {
			FUNC_ERR("GLFW ERROR (" << err << "): " << desc);
		});
#endif

		s_Instance = this;

		InitImGui();
	}

	void Application::InitImGui() {
		ImGui::GetIO().UserData = this;
		Themes::LoadThemes(m_ThemesPath);

		ImGuiSettingsHandler handler;
		handler.TypeName = "UserData";
		handler.TypeHash = ImHashStr(handler.TypeName);
		handler.ReadOpenFn = [](ImGuiContext*, ImGuiSettingsHandler* handler,
								 const char* val) -> void* {
			if (std::strcmp(val, "Preferences") == 0) {
				(void)handler;
				return Application::Get();
			}
			return nullptr;
		};
		handler.ReadLineFn = [](ImGuiContext*, ImGuiSettingsHandler*,
								 void* entry, const char* line) {
			(void)entry;
			char sel[37] = {0};
			if (std::sscanf(line, "Theme=\"%36s\"", sel) == 1) {
				FUNC_DBG("ReadLineFn read Theme: " << sel);
				Get()->SetTheme(UUID::FromString(sel));
			}
			int sfxEnabled;
			if (std::sscanf(line, "Sfx=%d", &sfxEnabled) == 1) {
				bool sfxEnabledBool = false;
				if (sfxEnabled >= 1) {
					sfxEnabledBool = true;
				}
				Get()->GetSettings().Sfx = sfxEnabledBool;
			}
			int prevEnabled;
			if (std::sscanf(line, "Prev=%d", &prevEnabled) == 1) {
				bool prevEnabledBool = false;
				if (prevEnabled >= 1)
					prevEnabledBool = true;

				Get()->GetSettings().Preview = prevEnabledBool;
			}
			int undoByStroke;
			if (std::sscanf(line, "UndoByStroke=%d", &undoByStroke) == 1) {
				Get()->GetSettings().UndoByStroke = undoByStroke >= 1;
			}

			double frameLimit;
			if (std::sscanf(line, "FrameLimit=%lf", &frameLimit) == 1) {
				Get()->GetSettings().FrameLimit = frameLimit;
			}

			ApplyThemeUuid(Get()->GetTheme());
		};
		handler.WriteAllFn = [](ImGuiContext*, ImGuiSettingsHandler* handler,
								 ImGuiTextBuffer* buf) {
			if (!Get()) {
				FUNC_INF("???");
				return;
			}
			FuncDoodle::UUID theme = Get()->GetTheme();
			buf->reserve(buf->size() + strlen(theme.ToString()));
			buf->append("[UserData][Preferences]\n");
			buf->appendf("Theme=\"%s\"", theme.ToString());
			buf->append("\n");
			buf->appendf("Sfx=%d", Get()->GetSettings().Sfx ? 1 : 0);
			buf->append("\n");
			buf->appendf("Prev=%d", Get()->GetSettings().Preview ? 1 : 0);
			buf->append("\n");
			buf->appendf(
				"UndoByStroke=%d", Get()->GetSettings().UndoByStroke ? 1 : 0);
			buf->append("\n");
			buf->appendf("FrameLimit=%lf", Get()->GetSettings().FrameLimit);
			buf->append("\n");
		};
		ImGui::AddSettingsHandler(&handler);

		ImGui::LoadIniSettingsFromDisk(ImGui::GetIO().IniFilename);
	}

	void Application::Run() {
		ApplyThemeUuid(m_Theme);

		while (!m_Window.ShouldClose()) {
			Update();

			if (GetShouldClose()) {
				break;
			}
		}
	}

	void Application::Update() {
		using namespace std::chrono;

		ImGuiIO& io = ImGui::GetIO();

		auto currentFrameTime = high_resolution_clock::now();
		auto deltaTime =
			duration<double>(currentFrameTime - m_LastFrame).count();

		if (deltaTime >= GetFrameTime()) {
			m_LastFrame = currentFrameTime;
			m_Window.PollEvents();

			// Start the ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::DockSpaceOverViewport(0U, ImGui::GetMainViewport(),
				ImGuiDockNodeFlags_PassthruCentralNode);

			RenderImGui();

			// Rendering
			int display_w, display_h;
			m_Window.GetFramebufferSize(&display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(0.0f, 0.0f, 0.0f, 1.00f);
			glClear(GL_COLOR_BUFFER_BIT);

			ImGui::Render();

			m_Manager->GetFrameRenderer()
				->GetCtx()
				->ToolManager->UpdateCursor();

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				GLFWwindow* backup = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup);
			}

			UpdateFPS(deltaTime);

			m_Window.SwapBuffers();

#ifdef FUNCDOODLE_BUILD_IMTESTS
			if (s_TestEngine) {
				ImGuiTestEngine_PostSwap(s_TestEngine);
			}
#endif
		}
	}

	void Application::RegisterKeybinds() {
		// nose
		m_Keybinds.Register("new", {true, false, false, ImGuiKey_N});
		m_Keybinds.Register("open", {true, false, false, ImGuiKey_O});
		m_Keybinds.Register("save", {true, false, false, ImGuiKey_S});
		m_Keybinds.Register("save_as", {true, true, false, ImGuiKey_S});
		m_Keybinds.Register("export", {true, false, false, ImGuiKey_E});

		m_Keybinds.Register("quit", {true, false, false, ImGuiKey_Q});
		m_Keybinds.Register("pref", {true, false, false, ImGuiKey_Comma});
		m_Keybinds.Register("theme", {true, false, false, ImGuiKey_T});

		m_Keybinds.Register("keybinds", {true, false, false, ImGuiKey_H});

		m_Keybinds.Register("del", {false, false, false, ImGuiKey_Delete});

		ToolKeybindsRegister(m_Keybinds);
		KeyHandler::RegisterKeybinds(m_Keybinds);
		m_Manager->RegisterKeybinds();

		m_Keybinds.End();
	}

	Application::~Application() {
		for (char* log : s_Logs) {
			delete[] log;
		}
		s_Logs.clear();
	}

	void Application::UpdateFPS(double deltaTime) {
		constexpr double minDelta = 1.0 / 1000.0;
		constexpr double maxDelta = 1.0;
		if (deltaTime >= minDelta && deltaTime <= maxDelta) {
			m_FPS = 1.0 / deltaTime;
		} else if (deltaTime > maxDelta) {
			m_FPS = 1.0 / maxDelta;
		}
		m_LastFrameTime = deltaTime;
	}

	char* GlobalGetShortcut(const char* key, bool shift, bool super) {
		int maxLen = 11 + strlen(key);

		char* shortcut = (char*)malloc(maxLen);

#if defined(_WIN32) || defined(__linux__)
		strcpy(shortcut, "Ctrl");
#else
		strcpy(shortcut, "Cmd");
#endif

		if (shift) {
			strcat(shortcut, "+Shift");
		}

		if (super) {
			strcat(shortcut, "+Super");
		}

		strcat(shortcut, "+");
		strcat(shortcut, key);

		return shortcut;
	}

	void Application::DeleteCurrentSelection() {
		auto sel = m_EditorController->Sel();
		auto selPixels = m_EditorController->Sel()->All();
		std::vector<Col> prevPixels;

		prevPixels.reserve(selPixels.size());
		for (auto& p : selPixels) {
			prevPixels.push_back(
				m_Manager->SelectedFrame()->Pixels()->Get(p.x, p.y));
		}

		m_CurrentProj->PushUndoable(
			DeleteSelectionAction(m_Manager->SelectedFrameI(),
				m_EditorController->Sel(), prevPixels, m_CurrentProj));
		m_Manager->SelectedFrame()->DeleteSelection(
			m_EditorController->Sel(), m_CurrentProj->BgCol());
	}

	void Application::RenderImGui() {
		if (!m_CurrentProj)
			m_UiManager.Options();

#ifdef FUNCDOODLE_BUILD_IMTESTS
		if (s_TestEngine) {
			ImGuiTestEngine_ShowTestEngineWindows(s_TestEngine, &m_ShowTests);
		}
#endif

		m_UiManager.Render();

		Themes::ThemeEditor();
		Themes::SaveCurrentTheme();

		if (m_CurrentProj) {
			m_Manager->SetSettings(m_Settings);
			m_Manager->RenderTimeline(m_Settings.Preview);
			m_Manager->RenderControls();
			m_Manager->RenderLogs();
			m_Manager->Player()->Play();
			m_CurrentProj->DisplayAltFPS(m_FPS);
		}
	}
	void Application::OpenFileDialog(std::function<void()> done) {
		FileDialog dialog("fdp");
		m_FilePath = dialog.Open().string();

		done();
	}
	void Application::SaveFileDialog(std::function<void()> done) {
		if (m_CurrentProj == nullptr) {
			FUNC_INF("No project to save");
			return;
		}

		FileDialog dialog("fdp");
		m_FilePath = dialog.Save().string();
		done();
	}
	void Application::ReadProjectFile() {
		// m_FilePath is the actual file that we're going to read
		if (m_FilePath.empty()) {
			FUNC_DBG("tried reading but m_FilePath is nullptr, probably means "
					 "that the file dialog was cancelled");
			return;
		}

		if (m_CurrentProj == nullptr) {
			m_CurrentProj =
				std::make_shared<ProjectFile>((char*)"", 1, 1, (char*)"", 0,
					(char*)"", &m_Window, Col{.r = 0, .g = 0, .b = 0});
		}

		m_CurrentProj->ReadAndPopulate(m_FilePath.c_str());

		m_Manager->SetProj(m_CurrentProj);

		m_CurrentProj->UpdateTitle();
	}
	void Application::SaveProjectFile() {
		if (m_FilePath.empty()) {
			FUNC_DBG("tried saving but m_FilePath is nullptr");
			return;
		}
		m_CurrentProj->Write(m_FilePath.c_str());

		m_CurrentProj->UpdateTitle();
	}

	void Application::OpenSaveChangesDialog() {
		m_UiManager.GetPopups().Open("save_changes");
	}

	void Application::DropCallback(int count, const char** paths) {
		if (count == 0)
			return;

		if (count > 1) {
			FUNC_WARN("Attempted to drag and drop multiple items when 1 is "
					  "expected: Attempting to use first item");
		}
		m_FilePath = paths[0] ? paths[0] : "";

		ReadProjectFile();
	}
	void Application::Rotate(int32_t deg) {
		if (!m_EditorController->Sel()) {
			m_CurrentProj->PushUndoable(RotateFrameAction(
				m_Manager->SelectedFrameI(), deg, m_CurrentProj));
			m_Manager->SelectedFrame()->Rotate(deg);
		} else {
			m_CurrentProj->PushUndoable(
				RotateSelectionAction(m_Manager->SelectedFrameI(),
					m_EditorController->Sel(), deg, m_CurrentProj));

			m_Manager->SelectedFrame()->RotateSelection(
				m_EditorController->Sel(), deg);
		}
	}

	void Application::Save(bool exit) {
		if (m_Settings.Sfx)
			m_AssetLoader->PlaySound(s_ProjSaveSound);

		if (exit) {
			SaveFileDialog([this, exit]() {
				SaveProjectFile();

				if (m_Settings.Sfx)
					m_AssetLoader->PlaySound(s_ProjSaveEndSound);

				if (exit) {
					m_ShouldClose = true;
					ImGui::CloseCurrentPopup();
				}
			});
		} else {
#ifndef MACOS
			std::thread([this, exit]() {
#endif
				SaveFileDialog([this, exit]() {
					SaveProjectFile();

					if (m_Settings.Sfx)
						m_AssetLoader->PlaySound(s_ProjSaveEndSound);

					if (exit) {
						m_ShouldClose = true;
						ImGui::CloseCurrentPopup();
					}
				});
#ifndef MACOS
			}).detach();
#endif
		}
	}

	void Application::SaveAt(const char* path) {
		if (m_Settings.Sfx)
			m_AssetLoader->PlaySound(s_ProjSaveSound);

		m_FilePath = path;
		SaveProjectFile();
	}

	void Application::MoveCurrentSelection(Direction direction) {
		MoveSelectionActionContext ctx{m_Manager->SelectedFrameI(),
			m_EditorController->Sel(), direction, m_CurrentProj};
		auto action =
			MoveSelectionAction(Frame(*m_Manager->SelectedFrame()), ctx);

		m_CurrentProj->PushUndoable(action);
		m_Manager->SelectedFrame()->MoveSelection(
			m_EditorController->Sel(), direction, m_CurrentProj->BgCol());
	}

	bool Application::IsPosInFrame(ImVec2 pos) {
		ImGuiWindow* frameWindow = ImGui::FindWindowByName("Frame");
		if (!frameWindow) {
			return false;
		}

		EditorController::CanvasContext* ctx =
			Application::Get()->GetManager()->GetFrameRenderer()->GetCtx();
		if (!ctx || !ctx->Frame) {
			return false;
		}

		const float frameWidth = ctx->Frame->Width() * ctx->PixelScale;
		const float frameHeight = ctx->Frame->Height() * ctx->PixelScale;

		const float statusBarHeight = 24.0f;

		ImVec2 contentRegion = frameWindow->ContentRegionRect.GetSize();
		ImVec2 windowPos = frameWindow->Pos;

		float startX = windowPos.x + (contentRegion.x - frameWidth) * 0.5f + 9;
		float startY =
			windowPos.y +
			((contentRegion.y - statusBarHeight) - frameHeight) * 0.5f + 41;

		ImVec2 frameMin(startX, startY);
		ImVec2 frameMax(startX + frameWidth, startY + frameHeight);

		return pos.x >= frameMin.x && pos.x <= frameMax.x &&
			   pos.y >= frameMin.y && pos.y <= frameMax.y;
	}

	void Application::Import(Where where) {
		FileDialog dialog(g_SupportedExtensionsForImporting);
		std::filesystem::path path = dialog.Open();

		if (path.empty()) {
			return;
		}

		int width, height, channels;

		// data is R G B R G B R G B etc. because we're doing 3 channels
		unsigned char* data =
			stbi_load(path.c_str(), &width, &height, &channels, 3);

		if (width != m_CurrentProj->AnimWidth() ||
			height != m_CurrentProj->AnimHeight()) {
			FUNC_WARN("imported image size doesn't match animation size");
		}

		Frame frame = Frame(m_CurrentProj->AnimWidth(),
			m_CurrentProj->AnimHeight(), m_CurrentProj->BgCol());

		for (int y = 0; y < std::min(height, m_CurrentProj->AnimHeight());
			y++) {
			for (int x = 0; x < std::min(width, m_CurrentProj->AnimWidth());
				x++) {

				int i = (y * width + x) * 3;

				unsigned char r = data[i + 0];
				unsigned char g = data[i + 1];
				unsigned char b = data[i + 2];

				Col col = Col{.r = r, .g = g, .b = b};

				frame.SetPixel(x, y, col);
			}
		}

		stbi_image_free(data);

		switch (where) {
			case Where::Before:
				m_CurrentProj->AnimFrames()->InsertBefore(
					m_Manager->SelectedFrameI(), frame);
				break;

			case Where::After:
				m_CurrentProj->AnimFrames()->InsertAfter(
					m_Manager->SelectedFrameI(), frame);
				break;
		}
	}

	void Application::HideCursor() {
		m_Window.SetCursorHidden(true);
	}

	void Application::ShowCursor() {
		m_Window.SetCursorHidden(false);
	}
}  // namespace FuncDoodle
