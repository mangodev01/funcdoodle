#include "UI/UIManager.h"
#include "Asset/LoadedAssets.h"
#include "Conf/Constants.h"
#include "Conf/FuncPCH.h"
#include "Core/App.h"
#include "Selection/Selection.h"
#include "UI/ImUtil.h"
#include "UI/Themes.h"
#include "Util/Ptr.h"
#include "Util/TextUtil.h"
#include <algorithm>
#include <imgui.h>
#include <optional>
#include <unordered_set>

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

		size.y -= menuBarHeight;
		ImGui::SetNextWindowSize(size, ImGuiCond_Always);
		ImGui::SetNextWindowViewport(vp->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		ImGui::Begin("Options", nullptr,
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
		float groupHeight = (btnSize.y * 2) + rowGap;

		ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
		ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
		float contentWidth = contentMax.x - contentMin.x;
		float rowStartX = contentMin.x + ((contentWidth - groupWidth) * 0.5f);
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
				btnTopY + ((btnSize.y - titleHeight) * 0.5f) - textYOffset);
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
			ImGui::OpenPopup("Preferences");
			m_Popups.Close("pref");
		}
		if (ImGui::BeginPopup("Preferences")) {
			ImGui::SeparatorText("Themes");

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

			if (ImGui::BeginTable("themes", 2)) {
				ImGui::TableSetupColumn(
					"", ImGuiTableColumnFlags_WidthStretch, 1.0f);
				ImGui::TableSetupColumn(
					"", ImGuiTableColumnFlags_WidthStretch, 1.0f);

				ImGui::TableNextColumn();
				if (ImGui::Button("Save", ImVec2(-FLT_MIN, 0))) {
					Themes::g_SaveThemeOpen = true;
				}

				ImGui::TableNextColumn();
				if (ImGui::Button("Add", ImVec2(-FLT_MIN, 0))) {
					FileDialog dialog = "toml";
					static Themes::CustomTheme* style;
					std::vector<std::filesystem::path> themes =
						dialog.OpenMultiple();

					for (const std::filesystem::path& path : themes) {
						style =
							Themes::LoadThemeFromFile(path.string().c_str());

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

				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				if (ImGui::Button("Open directory", ImVec2(-FLT_MIN, 0))) {
					OpenFileExplorer(app->GetThemesPath());
				}

				ImGui::TableNextColumn();
				if (ImGui::Button("Refresh", ImVec2(-FLT_MIN, 0))) {
					Themes::ClearThemes();
					Themes::LoadThemes(app->GetThemesPath());
				}

				ImGui::EndTable();
			}

			ImGui::SeparatorText("Canvas");
			ImGui::Checkbox("Preview", &app->GetSettings().Preview);
			ImGui::SameLine();
			ImGui::Checkbox("Undo by stroke", &app->GetSettings().UndoByStroke);

			ImGui::SeparatorText("Audio");
			ImGui::Checkbox("SFX", &app->GetSettings().Sfx);

			ImGui::SeparatorText("Performance");
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
						app->CloseCurrentProject();
					}
					if (ImGui::MenuItem("Edit")) {
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
						if (ImGui::BeginMenu("Rotate")) {
							if (ImGui::MenuItem("90°")) {
								app->Rotate(90);
							}

							if (ImGui::MenuItem("-90°")) {
								app->Rotate(-90);
							}

							if (ImGui::MenuItem("Rotate")) {
								m_Popups.Open("rotate");
							}

							ImGui::EndMenu();
						}

						if (ImGui::BeginMenu("Move")) {
							auto moveItem = [&](ImTextureID tex,
												const char* keybindId,
												Direction dir) {
								ImVec2 pos = ImGui::GetCursorScreenPos();

								ImGui::PushID(keybindId);

								bool clicked =
									ImGui::Selectable("##selectable", false);

								ImGui::PopID();

								ImGui::SetCursorScreenPos(pos);
								ImGui::Image(tex, ImVec2(20, 20));
								ImGui::SameLine();
								ImGui::TextUnformatted(
									m_WaitingForKey
										? nullptr
										: app->GetKeybinds().Get(keybindId));
								if (clicked && app->GetController()->Sel())
									app->MoveCurrentSelection(dir);
							};

							moveItem((ImTextureID)(intptr_t)s_Left,
								"move_selection_left", Direction::Left);
							moveItem((ImTextureID)(intptr_t)s_Right,
								"move_selection_right", Direction::Right);
							moveItem((ImTextureID)(intptr_t)s_Up,
								"move_selection_up", Direction::Up);
							moveItem((ImTextureID)(intptr_t)s_Down,
								"move_selection_down", Direction::Down);

							ImGui::EndMenu();
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

					if (ImGui::BeginMenu("Selection")) {
						if (ImGui::MenuItem("Invert")) {
							SharedPtr<Selection> selection =
								app->GetController()->Sel();

							std::vector<ImVec2i> all =
								selection == nullptr ? std::vector<ImVec2i>{}
													 : selection->All();
							// hehe musescore reference whoa
							std::vector<ImVec2i> newAll;

							std::unordered_set<ImVec2i, ImUtil::ImVec2iHash,
								ImUtil::ImVec2iEqual>
								selSet(all.begin(), all.end());

							SharedPtr<ProjectFile> proj = app->GetCurProj();

							for (int y = 0; y < proj->AnimHeight(); y++) {
								for (int x = 0; x < proj->AnimWidth(); x++) {
									if (!selSet.contains(ImVec2i(x, y))) {
										newAll.emplace_back(x, y);
									}
								}
							}

							auto sel = std::make_shared<ArbitrarySelection>();
							sel->Active = true;
							sel->All_ = newAll;

							app->GetController()->SetSel(sel);
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
			const ImGuiStyle& style = ImGui::GetStyle();
			const float keyboardWidth = 860.0f;
			const float keyHeight = ImGui::GetFrameHeight() * 1.25f;
			const float maxRowUnits = 15.0f;

			auto keyLabel = [](ImGuiKey key) -> const char* {
				return key == ImGuiKey_GraveAccent	  ? "`"
					   : key == ImGuiKey_Escape		  ? "Esc"
					   : key == ImGuiKey_Equal		  ? "="
					   : key == ImGuiKey_Backspace	  ? "Backspace"
					   : key == ImGuiKey_LeftShift	  ? "Shift"
					   : key == ImGuiKey_RightShift	  ? "Shift"
					   : key == ImGuiKey_LeftAlt	  ? "Alt"
					   : key == ImGuiKey_RightAlt	  ? "Alt"
					   : key == ImGuiKey_Apostrophe	  ? "'"
					   : key == ImGuiKey_LeftSuper	  ? "Super"
					   : key == ImGuiKey_RightSuper	  ? "Super"
					   : key == ImGuiKey_LeftCtrl	  ? "Ctrl"
					   : key == ImGuiKey_RightCtrl	  ? "Ctrl"
					   : key == ImGuiKey_LeftBracket  ? "["
					   : key == ImGuiKey_RightBracket ? "]"
					   : key == ImGuiKey_Backslash	  ? "\\"
					   : key == ImGuiKey_Semicolon	  ? ";"
					   : key == ImGuiKey_CapsLock	  ? "Caps"
					   : key == ImGuiKey_Minus		  ? "-"
					   : key == ImGuiKey_Comma		  ? ","
					   : key == ImGuiKey_Period		  ? "."
					   : key == ImGuiKey_Slash		  ? "/"
													  : ImGui::GetKeyName(key);
			};

			auto keyUnits = [](ImGuiKey key) -> float {
				return key == ImGuiKey_Backspace	? 2.0f
					   : key == ImGuiKey_Tab		? 1.5f
					   : key == ImGuiKey_Backslash	? 1.5f
					   : key == ImGuiKey_CapsLock	? 1.75f
					   : key == ImGuiKey_Enter		? 2.25f
					   : key == ImGuiKey_LeftShift	? 2.25f
					   : key == ImGuiKey_RightShift ? 2.75f
					   : key == ImGuiKey_Space		? 6.25f
					   : key == ImGuiKey_LeftCtrl	? 1.25f
					   : key == ImGuiKey_RightCtrl	? 1.25f
					   : key == ImGuiKey_LeftSuper	? 1.25f
					   : key == ImGuiKey_RightSuper ? 1.25f
					   : key == ImGuiKey_LeftAlt	? 1.25f
					   : key == ImGuiKey_RightAlt	? 1.25f
					   : key == ImGuiKey_Menu		? 1.25f
													: 1.0f;
			};

			auto rowUnits = [&](size_t rowStart, size_t rowEnd) {
				float units = 0.0f;
				for (size_t i = rowStart; i < rowEnd; ++i) {
					units += keyUnits(c_Qwerty[i]);
				}
				return units;
			};

			size_t rowStart = 0;
			while (rowStart < c_Qwerty.size()) {
				size_t rowEnd = rowStart;
				while (rowEnd < c_Qwerty.size() &&
					   c_Qwerty[rowEnd] != ImGuiKey_End) {
					++rowEnd;
				}

				const size_t keyCount = rowEnd - rowStart;
				const float rowSpacing =
					style.ItemSpacing.x * (keyCount > 0 ? keyCount - 1 : 0);
				const float unitWidth =
					(keyboardWidth - rowSpacing) / maxRowUnits;
				const float rowWidth =
					(rowUnits(rowStart, rowEnd) * unitWidth) + rowSpacing;
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
									 ((keyboardWidth - rowWidth) * 0.5f));

				for (size_t i = rowStart; i < rowEnd; ++i) {
					ImGuiKey key = c_Qwerty[i];
					const char* keyName = keyLabel(key);
					const ImVec2 keySize(keyUnits(key) * unitWidth, keyHeight);

					ImGui::PushID(ImGui::GetKeyName(key));

					bool styleVarPushed = false;

					if (ImGui::IsKeyDown(key)) {
						styleVarPushed = true;
						ImGui::PushStyleColor(ImGuiCol_Button,
							ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
					}

					if (ImGui::Button(keyName, keySize)) {
						ImGuiIO& io = ImGui::GetIO();
						m_AssigningToShortcut =
							Shortcut(io.KeyCtrl, io.KeyShift, io.KeySuper, key);
						ImGui::OpenPopup("assign_keys_to");
					}

					if (styleVarPushed) {
						ImGui::PopStyleColor();
					}

					if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
						ImGuiIO& io = ImGui::GetIO();
						auto& allKeybinds = app->GetKeybinds().GetAll();

						auto assignedKeybind = std::ranges::find_if(
							allKeybinds, [&](const auto& entry) {
								auto resolved = entry.second.User.value_or(
									entry.second.Default);

								return resolved.Key == key &&
									   resolved.RequiresCtrl == io.KeyCtrl &&
									   resolved.RequiresShift == io.KeyShift &&
									   resolved.RequiresSuper == io.KeySuper;
							});

						assignedKeybind->second.User = std::nullopt;
					}

					const bool keyHovered = ImGui::IsItemHovered();

					if (ImGui::BeginPopup("assign_keys_to")) {
						ImGui::InputTextWithHint("##search",
							"Search actions...", m_SearchQuery, 256);

						// if string isnt empty
						if (strcmp(m_SearchQuery, "") != 0) {
							for (auto& [k, v] : app->GetKeybinds().GetAll()) {
								if (strcasestr(k, m_SearchQuery) ||
									strcasestr(v.FriendlyName, m_SearchQuery) ||
									strcasestr(v.Description, m_SearchQuery)) {
									ImGui::PushID(k);

									ImVec2 pos = ImGui::GetCursorScreenPos();
									bool clicked = ImGui::Selectable(
										"##keybind_selectable", false);

									ImGui::PopID();

									ImGui::SetCursorScreenPos(pos);
									ImGui::SameLine();

									ImGui::Text("%s", v.FriendlyName);

									ImGui::PushFont(nullptr, 14.0f);
									ImGui::Text("%s", v.Description);
									ImGui::PopFont();

									if (clicked) {
										v.User = m_AssigningToShortcut;
										ImGui::CloseCurrentPopup();
										// lol
										m_SearchQuery[0] = '\0';
									}
								}
							}
						}

						ImGui::EndPopup();
					}

					if (keyHovered) {
						ImGui::BeginTooltip();

						auto allKeybinds = app->GetKeybinds().GetAll();

						ImGuiIO& io = ImGui::GetIO();
						auto assignedKeybind = std::ranges::find_if(
							allKeybinds, [&](const auto& entry) {
								if (entry.second.User.has_value())
									return entry.second.User->Key == key &&
										   entry.second.User->RequiresCtrl ==
											   io.KeyCtrl &&
										   entry.second.User->RequiresShift ==
											   io.KeyShift &&
										   entry.second.User->RequiresSuper ==
											   io.KeySuper;

								return entry.second.Default.Key == key &&
									   entry.second.Default.RequiresCtrl ==
										   io.KeyCtrl &&
									   entry.second.Default.RequiresShift ==
										   io.KeyShift &&
									   entry.second.Default.RequiresSuper ==
										   io.KeySuper;
							});

						if (assignedKeybind != allKeybinds.end()) {
							ImGui::Text(
								"%s", assignedKeybind->second.FriendlyName);
						} else {
							ImGui::Text("Unassigned");
						}

						ImGui::EndTooltip();
					}

					ImGui::PopID();

					if (i + 1 < rowEnd) {
						ImGui::SameLine();
					}
				}

				rowStart = rowEnd + 1;
			}

			ImGui::PushFont(nullptr, 14.0f);
			ImGui::Text("Hold Ctrl, Shift, or Super to preview shortcuts for "
						"modified keys.");
			ImGui::Text("Click a key to assign an action to that shortcut.");
			ImGui::Text("Right click a key to reset its shortcut.");
			ImGui::PopFont();

			if (ImUtil::CloseButton() || ImUtil::EscPressed()) {
				ImGui::CloseCurrentPopup();
				m_Popups.Close("keybinds");
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
			ImGui::OpenPopup("Edit Project");
		}

		if (ImGui::BeginPopupModal("Edit Project", m_Popups.Get("edit_proj"),
				ImGuiWindowFlags_AlwaysAutoResize) &&
			app->GetCurProj()) {
			SharedPtr<ProjectFile> proj = app->GetCurProj();
			SharedPtr<ProjectFile> cacheProj = app->GetCacheProj();

			ImGui::SeparatorText("Project");

			char name[256];
			strcpy(name, proj->AnimName());

			char author[100];
			strcpy(author, proj->AnimAuthor());

			char desc[512];
			strcpy(desc, proj->AnimDesc());

			int width = proj->AnimWidth();
			int height = proj->AnimHeight();
			int fps = proj->AnimFPS();

			if (ImGui::InputText("Name", name, sizeof(name))) {
				proj->SetAnimName(name);
			}
			if (ImGui::InputText("Author", author, sizeof(author))) {
				proj->SetAnimAuthor(author);
			}
			if (ImGui::InputText("Description", desc, sizeof(desc))) {
				proj->SetAnimDesc(desc);
			}

			ImGui::SeparatorText("Canvas");

			if (ImGui::InputInt("Width", &width)) {
				if (width < 1)
					width = 1;
				proj->SetAnimWidth(width);
			}
			if (ImGui::InputInt("Height", &height)) {
				if (height < 1)
					height = 1;
				proj->SetAnimHeight(height);
			}
			if (ImGui::InputInt("FPS", &fps)) {
				proj->SetAnimFPS(fps);
			}

			ImUtil::ButtonRowResult choice = ImUtil::OkCancelButtons();
			if (choice == ImUtil::ButtonRowResult::Secondary ||
				ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
				m_Popups.Close("edit_proj");
				ImGui::CloseCurrentPopup();
			}
			if (choice == ImUtil::ButtonRowResult::Primary ||
				ImGui::IsKeyPressed(ImGuiKey_Enter, false) ||
				ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false)) {
				m_Popups.Close("edit_proj");
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void UIManager::NewProj() {
		Application* app = Application::Get();

		if (m_Popups.IsOpen("new")) {
			ImGui::OpenPopup("New project");
		}

		if (ImGui::BeginPopupModal("New project", m_Popups.Get("new"),
				ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::SeparatorText("Project");

			bool justOpened = ImGui::IsWindowAppearing();
			char name[256] = "Untitled Project";
			int width = g_DefaultCanvasWidth;
			int height = g_DefaultCanvasHeight;
			char* author = const_cast<char*>(ImUtil::GetUsername());
			int fps = 10;
			char desc[512] = "An Untitled FuncDoodle project";

			if (!app->GetCacheProj()) {
				app->SetCacheProj(std::make_shared<ProjectFile>(name, width,
					height, author, fps, desc, app->GetWindow(), Col()));
			} else {
				SharedPtr<ProjectFile> cache = app->GetCacheProj();

				strcpy(name, cache->AnimName());
				width = cache->AnimWidth();
				height = cache->AnimHeight();
				strcpy(author, cache->AnimAuthor());
				fps = cache->AnimFPS();
				strcpy(desc, cache->AnimDesc());

				Col bg = cache->BgCol();

				float r = (float)(bg.r) / g_MaxColorValue;
				float g = (float)(bg.g) / g_MaxColorValue;
				float b = (float)(bg.b) / g_MaxColorValue;

				m_CacheBGCol[0] = r;
				m_CacheBGCol[1] = g;
				m_CacheBGCol[2] = b;
			}

			// GUI inputs for project properties
			if (ImGui::InputText("Name", name, sizeof(name))) {
				app->GetCacheProj()->SetAnimName(name);
			}
			if (ImGui::InputText("Author", author, sizeof(author))) {
				app->GetCacheProj()->SetAnimAuthor(author);
			}
			if (ImGui::InputText("Description", desc, sizeof(desc))) {
				app->GetCacheProj()->SetAnimDesc(desc);
			}

			ImGui::SeparatorText("Canvas");

			if (ImGui::InputInt("Width", &width)) {
				if (width < 1)
					width = 1;
				if (app->GetCurProj())
					app->GetCacheProj()->SetAnimWidth(width, false);
				else {
					if (app->GetCacheProj()) {
						app->GetCacheProj()->SetAnimWidth(width, true);
					}
				}
			}
			if (ImGui::InputInt("Height", &height)) {
				if (height < 1)
					height = 1;
				if (app->GetCurProj())
					app->GetCacheProj()->SetAnimHeight(height, false);
				else
					app->GetCacheProj()->SetAnimHeight(height, true);
			}
			if (ImGui::InputInt("FPS", &fps)) {
				app->GetCacheProj()->SetAnimFPS(fps);
			}

			if (ImGui::ColorButton(
					"##bg", ImVec4(m_CacheBGCol[0], m_CacheBGCol[1],
								m_CacheBGCol[2], 1.0f))) {
				ImGui::OpenPopup("BackgroundColorPicker");
			}
			ImGui::SameLine();
			ImGui::Text("Background");

			if (ImGui::BeginPopup("BackgroundColorPicker")) {
				if (ImGui::ColorPicker3("##background", m_CacheBGCol.data())) {
					if (app->GetCacheProj())
						app->GetCacheProj()->SetBgCol(m_CacheBGCol.data());
				}

				ImGui::EndPopup();
			}

			ImUtil::ButtonRowResult choice = ImUtil::CreateCancelButtons();
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

				FUNC_INF("Exporting to {}", path.string());

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
