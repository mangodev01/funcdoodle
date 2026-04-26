#include "UIManager.h"
#include "App.h"
#include "Constants.h"
#include "ImUtil.h"
#include "LoadedAssets.h"
#include "TextUtil.h"
#include "Themes.h"
#include "imgui.h"

namespace FuncDoodle {
	UIManager::UIManager() {
		// register all popups
		m_Popups.Register("edit_proj");
		m_Popups.Register("export");
		m_Popups.Register("keybinds");
		m_Popups.Register("new");
		m_Popups.Register("pref");
		m_Popups.Register("rotate");
		m_Popups.Register("save_changes");
	}

	void UIManager::Render() {
		CheckKeybinds();
		MainMenuBar();
		EditPrefs();
		Rotate();
		SaveChanges();
		ExportProj();
		EditProj();
		Keybinds();
		NewProj();
	}

	void UIManager::Options() {
		Application* app = Application::Get();

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

		ImFont* titleFont =
			app->GetAssetLoader() && app->GetAssetLoader()->GetFontBold()
				? app->GetAssetLoader()->GetFontBold()
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
			if (app->GetSettings().Sfx)
				app->GetAssetLoader()->PlaySound(s_ProjCreateSound);
			m_Popups.Open("new");
		});

		ImGui::SetCursorPosX(rowStartX);
		ImGui::SetCursorPosY(rowTopY + btnSize.y + rowGap);

		renderOptionRow(openProjTitle, openProjDesc, s_OpenTexId,
			openTextYOffsetFactor, [&]() {
#ifndef MACOS
				std::thread([app]() {
#endif
					app->OpenFileDialog([app]() { app->ReadProjectFile(); });
#ifndef MACOS
				}).detach();
#endif
			});

		ImGui::EndGroup();
		ImGui::End();
		ImGui::PopStyleVar(2);
	}

	void UIManager::Rotate() {
		Application* app = Application::Get();

		if (m_Popups.IsOpen("rotate")) {
			ImGui::OpenPopup("Rotate");
			m_Popups.Close("rotate");
		}

		if (ImGui::BeginPopup("Rotate")) {
			ImGui::DragInt("##Deg", &m_Deg, 1.0f, -g_MaxRotationDeg,
				g_MaxRotationDeg, "%d°");

			ImUtil::ButtonRowResult choice = ImUtil::OkCancelButtons();
			if (choice == ImUtil::ButtonRowResult::Primary) {
				app->Rotate(m_Deg);
				ImGui::CloseCurrentPopup();
			}
			if (choice == ImUtil::ButtonRowResult::Secondary) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void UIManager::EditPrefs() {
		Application* app = Application::Get();

		if (m_Popups.IsOpen("pref")) {
			ImGui::OpenPopup("EditPrefs");
			m_Popups.Close("pref");
		}
		if (ImGui::BeginPopup("EditPrefs")) {
			if (ImGui::BeginCombo(
					"Theme", Themes::g_Themes[app->GetTheme()].Name)) {
				for (auto& [uuid, theme] : Themes::g_Themes) {
					bool is_selected = (app->GetTheme() == uuid);
					if (ImGui::Selectable(theme.Name, is_selected)) {
						app->SetTheme(uuid);
						Application::ApplyThemeStyle(theme.Style);
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
					style = Themes::LoadThemeFromFile(path.string().c_str());

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
				OPEN_FILE_EXPLORER(app->GetThemesPath());
			}
			ImGui::SameLine();
			if (ImGui::Button("Refresh")) {
				Themes::ClearThemes();
				Themes::LoadThemes(app->GetThemesPath());
			}

			ImGui::Checkbox("SFX", &app->GetSettings().Sfx);
			ImGui::SameLine();
			ImGui::Checkbox("Preview", &app->GetSettings().Preview);
			ImGui::SameLine();
			ImGui::Checkbox("Undo by stroke", &app->GetSettings().UndoByStroke);

			ImGui::InputDouble("FPS limit", &app->GetFrameLimitCache());

			if (ImUtil::EnterPressed()) {
				app->GetSettings().FrameLimit = app->GetFrameLimitCache();
			}

			if (ImUtil::EscPressed() || ImUtil::EnterPressed() ||
				ImUtil::OkButton()) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void UIManager::SaveChanges() {
		Application* app = Application::Get();

		if (m_Popups.IsOpen("save_changes")) {
			ImGui::OpenPopup("Save Changes");
		}

		if (ImGui::BeginPopupModal(
				"Save Changes", m_Popups.Get("save_changes"))) {
			ImGui::Text("Save changes?");
			ImUtil::ButtonRowResult choice = ImUtil::YesNoCancelButtons();
			if (choice == ImUtil::ButtonRowResult::Primary) {
				app->Save(true);
			} else if (choice == ImUtil::ButtonRowResult::Secondary) {
				app->SetShouldClose(true);
				ImGui::CloseCurrentPopup();
			} else if (choice == ImUtil::ButtonRowResult::Tertiary) {
				app->SetShouldClose(false);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void UIManager::MainMenuBar() {
		Application* app = Application::Get();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File", true)) {
				if (ImGui::MenuItem("New",
						m_WaitingForKey ? nullptr
										: app->GetKeybinds().Get("new"))) {
					if (app->GetSettings().Sfx)
						app->GetAssetLoader()->PlaySound(s_ProjCreateSound);

					m_Popups.Open("new");
				}

				if (ImGui::MenuItem("Open",
						m_WaitingForKey ? nullptr
										: app->GetKeybinds().Get("open"))) {
#ifndef MACOS
					std::thread([app]() {
#endif
						app->OpenFileDialog(
							[app]() { app->ReadProjectFile(); });
#ifndef MACOS
					}).detach();
#endif
				}
				if (app->GetCurProj()) {
					const char* path = app->GetCurProj()->LastSavePath();

					if (ImGui::MenuItem("Save",
							m_WaitingForKey ? nullptr
											: app->GetKeybinds().Get("save"))) {
						if (*path) {
							app->SaveAt(path);
						} else {
							app->Save();
						}
					}

					if (*path) {
						if (ImGui::MenuItem("Save as...",
								m_WaitingForKey
									? nullptr
									: app->GetKeybinds().Get("save_as"))) {
							// force dialog if user presses save as
							app->Save();
						}
					}

					if (ImGui::MenuItem("Close")) {
						app->SetCurProj(nullptr);
						app->GetManager()->SetProj(nullptr);
					}
					if (ImGui::MenuItem("Edit project")) {
						m_Popups.Open("edit_proj");
					}
					if (ImGui::MenuItem(
							"Export", m_WaitingForKey
										  ? nullptr
										  : app->GetKeybinds().Get("export"))) {
						m_Popups.Open("export");
					}
				}
				if (ImGui::MenuItem("Exit",
						m_WaitingForKey ? nullptr
										: app->GetKeybinds().Get("quit"))) {
					m_Popups.CloseAllExcept("save_changes");
					app->GetWindow()->SetShouldClose(true);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit", true)) {
				if (app->GetCurProj()) {
					if (ImGui::BeginMenu("Transform")) {
						if (ImGui::MenuItem("Rotate 90°")) {
							app->Rotate(90);
						}

						if (ImGui::MenuItem("Rotate -90°")) {
							app->Rotate(-90);
						}

						if (ImGui::MenuItem("Rotate...")) {
							m_Popups.Open("rotate");
						}

						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Frame")) {
						if (ImGui::MenuItem("Import to frame before",
								m_WaitingForKey ? nullptr
												: app->GetKeybinds().Get(
													  "import_before"))) {
							app->Import(Application::Where::Before);
						}
						if (ImGui::MenuItem("Import to frame after",
								m_WaitingForKey
									? nullptr
									: app->GetKeybinds().Get("import_after"))) {
							app->Import(Application::Where::After);
						}

						ImGui::EndMenu();
					}

					if (ImGui::MenuItem("Delete",
							m_WaitingForKey ? nullptr
											: app->GetKeybinds().Get("del"))) {
						if (app->GetController()->Sel()) {
							app->DeleteCurrentSelection();
						}
					}

					if (ImGui::MenuItem("Move left",
							m_WaitingForKey ? nullptr
											: app->GetKeybinds().Get(
												  "move_selection_left"))) {
						if (app->GetController()->Sel()) {
							app->MoveCurrentSelection(Direction::Left);
						}
					}

					if (ImGui::MenuItem("Move right",
							m_WaitingForKey ? nullptr
											: app->GetKeybinds().Get(
												  "move_selection_right"))) {
						if (app->GetController()->Sel()) {
							app->MoveCurrentSelection(Direction::Right);
						}
					}

					if (ImGui::MenuItem("Move up",
							m_WaitingForKey ? nullptr
											: app->GetKeybinds().Get(
												  "move_selection_up"))) {
						if (app->GetController()->Sel()) {
							app->MoveCurrentSelection(Direction::Up);
						}
					}

					if (ImGui::MenuItem("Move down",
							m_WaitingForKey ? nullptr
											: app->GetKeybinds().Get(
												  "move_selection_down"))) {
						if (app->GetController()->Sel()) {
							app->MoveCurrentSelection(Direction::Down);
						}
					}
				}

				if (ImGui::MenuItem("Preferences",
						m_WaitingForKey ? nullptr
										: app->GetKeybinds().Get("pref"))) {
					m_Popups.Open("pref");
				}
				if (ImGui::MenuItem("Theme editor",
						m_WaitingForKey ? nullptr
										: app->GetKeybinds().Get("theme"))) {
					Themes::g_ThemeEditorOpen = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help", true)) {
				if (ImGui::MenuItem("Show keybinds",
						m_WaitingForKey ? nullptr
										: app->GetKeybinds().Get("keybinds"))) {
					m_Popups.Open("keybinds");
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	void UIManager::Keybinds() {
		Application* app = Application::Get();

		if (m_Popups.IsOpen("keybinds")) {
			ImGui::OpenPopup("Keybinds");
		}

		if (ImGui::BeginPopupModal("Keybinds", m_Popups.Get("keybinds"),
				ImGuiWindowFlags_AlwaysAutoResize)) {
			if (ImGui::BeginTable(
					"keybinds", 3, ImGuiTableFlags_BordersInnerH)) {
				ImGui::TableSetupColumn("Action");
				ImGui::TableSetupColumn("Keybind");
				ImGui::TableSetupColumn("Reset");
				ImGui::TableHeadersRow();

				for (auto& [k, v] : app->GetKeybinds().GetAll()) {
					ImGui::TableNextRow();
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
						ImGui::GetColorU32(ImGuiCol_FrameBg));

					ImGui::TableSetColumnIndex(0);
					ImGui::TextUnformatted(k);

					ImGui::TableSetColumnIndex(1);

					ImGui::PushID(k);
					const char* label = "...";
					if (!m_WaitingForKey || strcmp(m_WaitingForKey, k) != 0) {
						label = v.User.value_or(v.Default);
					}
					if (ImGui::Button(label)) {
						m_WaitingForKey = k;
					}

					ImGuiKey key = ImUtil::GetAnyReleasedKey();

					if (m_WaitingForKey != nullptr &&
						strcmp(m_WaitingForKey, k) == 0 &&
						key != ImGuiKey_None) {
						ImGuiIO& io = ImGui::GetIO();
						v.User =
							Shortcut(io.KeyCtrl, io.KeyShift, io.KeySuper, key);
						m_WaitingForKey = nullptr;
						io.KeysData[key].Down = false;
						io.KeyCtrl = false;
						io.KeyShift = false;
						io.KeySuper = false;
						app->GetKeybinds().Write();
					}

					ImGui::PopID();

					ImGui::TableSetColumnIndex(2);
					if (!v.User.has_value() || v.User.value() == v.Default) {
						ImGui::BeginDisabled(true);
					} else {
						ImGui::BeginDisabled(false);
					}

					ImGui::PushID(k);
					if (ImGui::Button("Reset")) {
						v.User = std::nullopt;
					}
					ImGui::PopID();

					ImGui::EndDisabled();
				}
				ImGui::EndTable();
			}

			if (ImUtil::CloseButton() || ImUtil::EscPressed()) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		} else {
			if (m_WaitingForKey != nullptr) {
				m_WaitingForKey = nullptr;
			}
		}
	}

	void UIManager::EditProj() {
		Application* app = Application::Get();

		if (m_Popups.IsOpen("edit_proj")) {
			ImGui::OpenPopup("EditProj");
		}

		if (ImGui::IsPopupOpen("EditProj")) {
			ImGui::SetNextWindowFocus();
			ImGui::SetNextWindowPos(ImVec2(485, 384), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(309, 312), ImGuiCond_FirstUseEver);
		}
		if (ImGui::BeginPopupModal("EditProj", m_Popups.Get("edit_proj"),
				ImGuiWindowFlags_AlwaysAutoResize) &&
			app->GetCurProj()) {
			char name[256];
			strcpy(name, app->GetCurProj()->AnimName());
			int width = app->GetCurProj()->AnimWidth();
			int height = app->GetCurProj()->AnimHeight();
			char author[100];
			strcpy(author, app->GetCurProj()->AnimAuthor());
			int fps = app->GetCurProj()->AnimFPS();
			char desc[512];
			strcpy(desc, app->GetCurProj()->AnimDesc());
			if (app->GetCacheProj()) {
				strcpy(name, app->GetCacheProj()->AnimName());
				width = app->GetCacheProj()->AnimWidth();
				height = app->GetCacheProj()->AnimHeight();
				strcpy(author, app->GetCacheProj()->AnimAuthor());
				fps = app->GetCacheProj()->AnimFPS();
				strcpy(desc, app->GetCacheProj()->AnimDesc());
			} else {
				strcpy(name, (char*)"Untitled Animation");
				width = 32;
				height = 32;
				const char* username = ImUtil::GetUsername();
				strncpy(author, username, sizeof(author) - 1);
				author[sizeof(author) - 1] = '\0';
				fps = 10;
				strcpy(desc, "Simple test project");
			}
			if (ImGui::InputText("Name", name, sizeof(name))) {
				app->GetCacheProj()->SetAnimName(name);
			}
			if (ImGui::InputInt("Width", &width)) {
				app->GetCacheProj()->SetAnimWidth(width);
			}
			if (ImGui::InputInt("Height", &height)) {
				app->GetCacheProj()->SetAnimHeight(height);
			}
			if (ImGui::InputText("Author", author, sizeof(author))) {
				app->GetCacheProj()->SetAnimAuthor(author);
			}
			if (ImGui::InputInt("FPS", &fps)) {
				app->GetCacheProj()->SetAnimFPS(fps);
			}
			if (ImGui::InputText("Description", desc, sizeof(desc))) {
				app->GetCacheProj()->SetAnimDesc(desc);
			}

			ImUtil::ButtonRowResult choice = ImUtil::CloseOkButtons();
			if (choice == ImUtil::ButtonRowResult::Secondary ||
				ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
				m_Popups.Close("edit_proj");
				ImGui::CloseCurrentPopup();
			}
			if (choice == ImUtil::ButtonRowResult::Primary ||
				ImGui::IsKeyPressed(ImGuiKey_Enter, false) ||
				ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false)) {
				app->GetCurProj() = app->GetCacheProj();
				m_Popups.Close("edit_proj");
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void UIManager::NewProj() {
		Application* app = Application::Get();

		if (m_Popups.IsOpen("new")) {
			ImGui::OpenPopup("NewProj");
		}

		if (ImGui::IsPopupOpen("NewProj")) {
			ImGui::SetNextWindowFocus();
			ImGui::SetNextWindowPos(ImVec2(376, 436), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(350, 336), ImGuiCond_FirstUseEver);
		}

		if (ImGui::BeginPopupModal("NewProj", m_Popups.Get("new"),
				ImGuiWindowFlags_AlwaysAutoResize)) {
			bool justOpened = ImGui::IsWindowAppearing();
			char name[256] = "";
			int width = 32;
			int height = 32;
			char author[100] = "";
			int fps = 0;
			char desc[512] = "";

			if (!app->GetCacheProj()) {
				strcpy(name, (char*)"testproj");
				width = 32;
				height = 32;
				const char* username = ImUtil::GetUsername();
				strncpy(author, username, sizeof(author) - 1);
				author[sizeof(author) - 1] = '\0';
				fps = 10;
				strcpy(desc, "Simple test project");
				app->SetCacheProj(std::make_shared<ProjectFile>(name, width,
					height, author, fps, desc, app->GetWindow(), Col()));
			} else {
				strcpy(name, app->GetCacheProj()->AnimName());
				width = app->GetCacheProj()->AnimWidth();
				height = app->GetCacheProj()->AnimHeight();
				strcpy(author, app->GetCacheProj()->AnimAuthor());
				fps = app->GetCacheProj()->AnimFPS();
				strcpy(desc, app->GetCacheProj()->AnimDesc());

				float r =
					(float)(app->GetCacheProj()->BgCol().r) / g_MaxColorValue;
				float g =
					(float)(app->GetCacheProj()->BgCol().g) / g_MaxColorValue;
				float b =
					(float)(app->GetCacheProj()->BgCol().b) / g_MaxColorValue;
				m_CacheBGCol[0] = r;
				m_CacheBGCol[1] = g;
				m_CacheBGCol[2] = b;
			}

			// GUI inputs for project properties
			if (ImGui::InputText("Name", name, sizeof(name))) {
				app->GetCacheProj()->SetAnimName(name);
			}
			if (ImGui::InputInt("Width", &width)) {
				if (app->GetCurProj())
					app->GetCacheProj()->SetAnimWidth(width, false);
				else {
					if (app->GetCacheProj()) {
						app->GetCacheProj()->SetAnimWidth(width, true);
					}
				}
			}
			if (ImGui::InputInt("Height", &height)) {
				if (app->GetCurProj())
					app->GetCacheProj()->SetAnimHeight(height, false);
				else
					app->GetCacheProj()->SetAnimHeight(height, true);
			}
			if (ImGui::InputText("Author", author, sizeof(author))) {
				app->GetCacheProj()->SetAnimAuthor(author);
			}
			if (ImGui::InputInt("FPS", &fps)) {
				app->GetCacheProj()->SetAnimFPS(fps);
			}
			if (ImGui::InputText("Description", desc, sizeof(desc))) {
				app->GetCacheProj()->SetAnimDesc(desc);
			}
			if (ImGui::ColorPicker3("BG", m_CacheBGCol.data())) {
				if (app->GetCacheProj())
					app->GetCacheProj()->SetBgCol(m_CacheBGCol.data());
			}

			ImUtil::ButtonRowResult choice = ImUtil::CloseOkButtons();
			if (choice == ImUtil::ButtonRowResult::Secondary ||
				ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
				m_Popups.Close("new");
				ImGui::CloseCurrentPopup();
			}
			bool acceptByKey =
				!justOpened &&
				(ImGui::IsKeyPressed(ImGuiKey_Enter, false) ||
					ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false));
			if (choice == ImUtil::ButtonRowResult::Primary || acceptByKey) {
				app->SetCurProj(app->GetCacheProj());
				app->GetManager()->SetProj(app->GetCurProj());
				app->GetManager()->SetSettings(app->GetSettings());

				m_Popups.Close("new");
			}

			ImGui::EndPopup();
		}
	}

	void UIManager::ExportProj() {
		Application* app = Application::Get();

		if (m_Popups.IsOpen("export")) {
			ImGui::OpenPopup("Export##export");
			m_Popups.Close("export");
		}

		if (ImGui::BeginPopup("Export##export")) {
			const char* formats[] = {"PNGs", "MP4"};
			ImGui::Combo("Export Format", &app->GetExportFormatPtr(), formats,
				IM_ARRAYSIZE(formats));
			ImUtil::ButtonRowResult choice = ImUtil::ExportCloseButtons();
			if (choice == ImUtil::ButtonRowResult::Primary ||
				ImUtil::EnterPressed()) {
				FileDialog dialog;
				std::filesystem::path path = dialog.Dir();

				if (app->GetSettings().Sfx)
					app->GetAssetLoader()->PlaySound(s_ExportSound);
				FUNC_INF("Exporting to " << path);
				app->GetCurProj()->Export(
					path.string().c_str(), app->GetExportFormat());

				ImGui::CloseCurrentPopup();
			}
			if (choice == ImUtil::ButtonRowResult::Secondary ||
				ImUtil::EscPressed()) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void UIManager::CheckKeybinds() {
		Application* app = Application::Get();
		if (m_WaitingForKey != nullptr)
			return;

		ImGuiIO& io = ImGui::GetIO();

		// Check if each shortcut is pressed and perform the appropriate action
		if (app->GetKeybinds().Get("new").IsPressed()) {
			if (app->GetSettings().Sfx)
				app->GetAssetLoader()->PlaySound(s_ProjCreateSound);
			m_Popups.Open("new");
		}
		if (app->GetKeybinds().Get("open").IsPressed()) {
#ifndef MACOS
			std::thread([app]() {
#endif
				app->OpenFileDialog([app]() { app->ReadProjectFile(); });
#ifndef MACOS
			}).detach();
#endif
		}
		if (app->GetKeybinds().Get("quit").IsPressed()) {
			app->GetWindow()->SetShouldClose(true);
		}
		if (app->GetKeybinds().Get("pref").IsPressed()) {
			m_Popups.Open("pref");
		}
		if (app->GetKeybinds().Get("theme").IsPressed()) {
			Themes::g_ThemeEditorOpen = true;
		}
		if (app->GetKeybinds().Get("del").IsPressed()) {
			if (app->GetController()->Sel() && app->GetCurProj()) {
				app->DeleteCurrentSelection();
			}
		}
		if (app->GetController()->Sel() && app->GetCurProj()) {
			if (app->GetKeybinds().Get("move_selection_left").IsPressed())
				app->MoveCurrentSelection(Direction::Left);
			if (app->GetKeybinds().Get("move_selection_right").IsPressed())
				app->MoveCurrentSelection(Direction::Right);
			if (app->GetKeybinds().Get("move_selection_up").IsPressed())
				app->MoveCurrentSelection(Direction::Up);
			if (app->GetKeybinds().Get("move_selection_down").IsPressed())
				app->MoveCurrentSelection(Direction::Down);
		}
		if (app->GetKeybinds().Get("keybinds").IsPressed()) {
			m_Popups.Open("keybinds");
		}
		if (app->GetCurProj()) {
			if (app->GetKeybinds().Get("save").IsPressed()) {
				const char* path = app->GetCurProj()->LastSavePath();

				if (*path) {
					app->SaveAt(path);
				} else {
					app->Save();
				}
			}

			if (app->GetKeybinds().Get("save_as").IsPressed()) {
				app->Save();
			}

			if (app->GetKeybinds().Get("export").IsPressed()) {
				m_Popups.Open("export");
			}
		}
	}

}  // namespace FuncDoodle
