#include "AssetLoader.h"
#include "Gui.h"
#include "KeyHandler.h"
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
	namespace {
		void ApplyThemeStyle(const ImGuiStyle& themeStyle) {
			ImGuiStyle& style = ImGui::GetStyle();
			style = themeStyle;
			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				style.WindowRounding = 1.0f;
			}
		}

		void ApplyThemeUuid(const FuncDoodle::UUID& uuid) {
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
	}  // namespace

	Application* Application::s_Instance = nullptr;
	Application::Application()
		: m_FilePath(""), m_CurrentProj(nullptr), m_CacheProj(nullptr),
		  m_EditorController(std::make_shared<EditorController>()),
		  m_CacheBGCol({255, 255, 255}),
		  m_Theme(UUID::FromString("d0c1a009-d09c-4fe6-84f8-eddcb2da38f9")),
		  m_FrameLimitCache(m_Settings.FrameLimit),
		  m_Keybinds(std::filesystem::path("")),
		  m_Window(
			  {.Width = 1920, .Height = 1080, .Title = "", .Monitor = -1}) {
#ifdef FUNCDOODLE_BUILD_TESTS
		FuncDoodle_RunTests();
		FuncDoodle::TestRegistry::Instance().PrintSummary();

		std::exit(0);
#endif

		std::filesystem::path path = exepath::get();

		std::filesystem::path rootPath = path.parent_path();

		m_AssetLoader = new AssetLoader(rootPath / "assets");

		m_ThemesPath = rootPath / "themes";

		m_Keybinds = KeybindsRegistry(path);
		m_FrameLimitCache = m_Settings.FrameLimit;
		m_Manager = std::make_unique<AnimationManager>(
			nullptr, m_AssetLoader, m_EditorController, m_Keybinds, m_Settings),

		RegisterKeybinds();

		auto title = std::format("FuncDoodle {}", FUNCVER);
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

	void Application::RegisterPopups() {
		m_Popups.Register("edit_proj");
		m_Popups.Register("export");
		m_Popups.Register("keybinds");
		m_Popups.Register("new");
		m_Popups.Register("pref");
		m_Popups.Register("rotate");
		m_Popups.Register("save_changes");
	}

	Application::~Application() {
		for (char* log : s_Logs) {
			delete[] log;
		}
		s_Logs.clear();

		delete m_AssetLoader;
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
			RenderOptions();

#ifdef FUNCDOODLE_BUILD_IMTESTS
		if (s_TestEngine) {
			ImGuiTestEngine_ShowTestEngineWindows(s_TestEngine, &m_ShowTests);
		}
#endif

		m_UiManager.CheckKeybinds();
		m_UiManager.MainMenuBar();
		RenderEditPrefs();
		RenderRotate();
		SaveChangesDialog();
		m_UiManager.ExportProj();
		m_UiManager.EditProj();
		m_UiManager.Keybinds();
		m_UiManager.NewProj();

		Themes::ThemeEditor();
		Themes::SaveCurrentTheme();

		if (m_CurrentProj) {
			m_Manager->SetSettings(m_Settings);
			m_Manager->RenderTimeline(m_Settings.Preview);
			m_Manager->RenderControls();
			m_Manager->RenderLogs();
			m_Manager->Player()->Play();
			m_CurrentProj->DisplayFPS(m_FPS);
		}
	}
	void Application::OpenFileDialog(std::function<void()> done) {
		FileDialog dialog("fdp");
		m_FilePath = dialog.Open();
		done();
	}
	void Application::SaveFileDialog(std::function<void()> done) {
		if (m_CurrentProj == nullptr) {
			FUNC_INF("No project to save");
			return;
		}

		FileDialog dialog("fdp");
		m_FilePath = dialog.Save();
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
			m_CurrentProj.reset(new ProjectFile((char*)"", 1, 1, (char*)"", 0,
				(char*)"", &m_Window, Col{.r = 0, .g = 0, .b = 0}));
		}

		m_CurrentProj->ReadAndPopulate(m_FilePath.c_str());

		m_Manager->SetProj(m_CurrentProj);
	}
	void Application::SaveProjectFile() {
		if (m_FilePath.empty()) {
			FUNC_DBG("tried saving but m_FilePath is nullptr");
			return;
		}
		m_CurrentProj->Write(m_FilePath.c_str());
	}

	void Application::RenderOptions() {
		ImGuiViewport* vp = ImGui::GetMainViewport();
		ImVec2 size = vp->Size;
		ImVec2 safe = ImGui::GetStyle().DisplaySafeAreaPadding;

		float menuBarHeight = ImGui::GetFrameHeight();
		ImVec2 nextWindowPos = ImVec2(
			vp->Pos.x + safe.x - 2, vp->Pos.y + menuBarHeight + safe.y - 3);
		ImGui::SetNextWindowPos(nextWindowPos, ImGuiCond_Always);
		size.y -= menuBarHeight;
		ImGui::SetNextWindowSize(size, ImGuiCond_Always);
		ImGui::SetNextWindowViewport(vp->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		ImGui::Begin("Options", 0,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration);

		ImGui::BeginGroup();

		const ImVec2 btnSize(50, 50);
		const float titleFontSize = 25.0f;
		const float descFontSize = 18.0f;
		const float descPadFactor = 0.65f;
		const float rowGapFactor = 0.75f;
		const float openTextYOffsetFactor = 0.16f;

		ImFont* titleFont = m_AssetLoader && m_AssetLoader->GetFontBold()
								? m_AssetLoader->GetFontBold()
								: ImGui::GetFont();

		const char* measureSample = "Ag";

		const float spacingY = ImGui::GetStyle().ItemSpacing.y;
		const float textGap = ImGui::GetStyle().ItemSpacing.x;
		const float descLineHeight =
			TextUtil::TextHeight(nullptr, descFontSize, measureSample);
		const float descBottomPad = std::round(descLineHeight * descPadFactor);
		const float rowGapExtra = std::round(descLineHeight * rowGapFactor);
		const float rowGap = spacingY + rowGapExtra;

		const char* newProjTitle = "New project";
		const char* newProjDesc = "Create a new FuncDoodle project";
		const char* openProjTitle = "Open project";
		const char* openProjDesc = "Open an existing FuncDoodle project";

		float maxWidth = TextUtil::MaxWidth(titleFont, titleFontSize,
			newProjTitle, openProjTitle, nullptr, descFontSize, newProjDesc,
			openProjDesc);

		float groupWidth = btnSize.x + textGap + maxWidth;
		float groupHeight = btnSize.y * 2 + rowGap;

		ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
		ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
		float contentWidth = contentMax.x - contentMin.x;
		float rowStartX = contentMin.x + (contentWidth - groupWidth) * 0.5f;
		ImGui::SetCursorPosX(rowStartX);

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 avail = ImGui::GetContentRegionAvail();
		float rowTopY = (avail.y - groupHeight) * 0.5f;
		ImGui::SetCursorPosY(rowTopY);

		auto renderOptionRow = [&](const char* title, const char* desc,
								   uint32_t texId, float textYOffsetFactor,
								   const std::function<void()>& onClick) {
			if (ImGui::ImageButton(
					title, (ImTextureID)(intptr_t)texId, btnSize)) {
				onClick();
			}

			ImVec2 btnMin = ImGui::GetItemRectMin();
			ImVec2 btnMax = ImGui::GetItemRectMax();
			float btnTopY = btnMin.y - windowPos.y;
			float btnBottomY = btnMax.y - windowPos.y;
			float textX = btnMax.x - windowPos.x + textGap;

			ImGui::SameLine();

			ImGui::PushFont(titleFont, titleFontSize);
			float titleHeight =
				TextUtil::TextHeight(titleFont, titleFontSize, title);
			float textYOffset = std::round(titleHeight * textYOffsetFactor);
			ImGui::SetCursorPosX(textX);
			ImGui::SetCursorPosY(
				btnTopY + (btnSize.y - titleHeight) * 0.5f - textYOffset);
			ImGui::Text("%s", title);
			ImGui::PopFont();

			ImGui::PushFont(nullptr, descFontSize);
			float descLineHeightLocal = ImGui::GetTextLineHeight();
			ImGui::SetCursorPosX(textX);
			ImGui::SetCursorPosY(
				btnBottomY - descLineHeightLocal - descBottomPad - textYOffset);
			ImGui::Text("%s", desc);
			ImGui::PopFont();
		};

		renderOptionRow(newProjTitle, newProjDesc, s_AddTexId, 0.0f, [&]() {
			if (m_Settings.Sfx)
				m_AssetLoader->PlaySound(s_ProjCreateSound);
			m_Popups.Open("new");
		});

		ImGui::SetCursorPosX(rowStartX);
		ImGui::SetCursorPosY(rowTopY + btnSize.y + rowGap);

		renderOptionRow(openProjTitle, openProjDesc, s_OpenTexId,
			openTextYOffsetFactor, [&]() {
#ifndef MACOS
				std::thread([&]() {
#endif
					OpenFileDialog([&]() { ReadProjectFile(); });
#ifndef MACOS
				}).detach();
#endif
			});

		ImGui::EndGroup();
		ImGui::End();
		ImGui::PopStyleVar(2);
	}

	void Application::SaveChangesDialog() {
		if (m_Popups.IsOpen("save_changes")) {
			ImGui::OpenPopup("Save Changes");
		}

		if (ImGui::BeginPopupModal(
				"Save Changes", m_Popups.Get("save_changes"))) {
			ImGui::Text("Save changes?");
			ImUtil::ButtonRowResult choice = ImUtil::YesNoCancelButtons();
			if (choice == ImUtil::ButtonRowResult::Primary) {
				Save(true);
			} else if (choice == ImUtil::ButtonRowResult::Secondary) {
				m_ShouldClose = true;
				ImGui::CloseCurrentPopup();
			} else if (choice == ImUtil::ButtonRowResult::Tertiary) {
				m_ShouldClose = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	void Application::OpenSaveChangesDialog() {
		m_Popups.Open("save_changes");
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

	void Application::RenderEditPrefs() {
		if (m_Popups.IsOpen("pref")) {
			ImGui::OpenPopup("EditPrefs");
			m_Popups.Close("pref");
		}
		if (ImGui::BeginPopup("EditPrefs")) {
			if (ImGui::BeginCombo("Theme", Themes::g_Themes[m_Theme].Name)) {
				for (auto& [uuid, theme] : Themes::g_Themes) {
					bool is_selected = (m_Theme == uuid);
					if (ImGui::Selectable(theme.Name, is_selected)) {
						m_Theme = uuid;
						ApplyThemeStyle(theme.Style);
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text("by %s", theme.Author);
						ImGui::EndTooltip();
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			if (ImGui::Button("Save current")) {
				Themes::g_SaveThemeOpen = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Add temporary from file")) {
				FileDialog dialog = "toml";
				static Themes::CustomTheme* style;
				std::vector<std::filesystem::path> themes =
					dialog.OpenMultiple();

				for (size_t i = 0; i < themes.size(); i++) {
					std::filesystem::path path = themes[i];
					style = Themes::LoadThemeFromFile(path.c_str());

					if (style) {
						auto [it, inserted] =
							Themes::g_Themes.emplace(style->Uuid, *style);
						if (!inserted && style->OwnsMeta) {
							std::free(const_cast<char*>(style->Name));
							std::free(const_cast<char*>(style->Author));
							style->Name = "";
							style->Author = "";
							style->OwnsMeta = false;
						}
					}
				}
			}
			if (ImGui::Button("Open themes directory")) {
				OPEN_FILE_EXPLORER(m_ThemesPath);
			}
			ImGui::SameLine();
			if (ImGui::Button("Refresh")) {
				Themes::ClearThemes();
				Themes::LoadThemes(m_ThemesPath);
			}

			ImGui::Checkbox("SFX", &m_Settings.Sfx);
			ImGui::SameLine();
			ImGui::Checkbox("Preview", &m_Settings.Preview);
			ImGui::SameLine();
			ImGui::Checkbox("Undo by stroke", &m_Settings.UndoByStroke);

			ImGui::InputDouble("FPS limit", &m_FrameLimitCache);

			if (ImUtil::EnterPressed()) {
				m_Settings.FrameLimit = m_FrameLimitCache;
			}

			if (ImUtil::EscPressed() ||
				ImUtil::EnterPressed() ||
				ImUtil::OkButton()) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
	void Application::RenderRotate() {
		if (m_Popups.IsOpen("rotate")) {
			ImGui::OpenPopup("Rotate");
			m_Popups.Close("rotate");
		}

		if (ImGui::BeginPopup("Rotate")) {
			ImGui::DragInt("##Deg", &m_Deg, 1.0f, -360, 360, "%d°");

			ImUtil::ButtonRowResult choice = ImUtil::OkCancelButtons();
			if (choice == ImUtil::ButtonRowResult::Primary) {
				Rotate(m_Deg);
				ImGui::CloseCurrentPopup();
			}
			if (choice == ImUtil::ButtonRowResult::Secondary) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}  // namespace FuncDoodle
