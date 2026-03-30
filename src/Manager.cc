#include "Manager.h"

#include "Keybinds.h"
#include "Project.h"

#include "FrameRenderer.h"
#include "KeyHandler.h"

#include "ToolManager.h"

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>

#include "LoadedAssets.h"

#include "Ptr.h"
#include "imgui.h"

namespace FuncDoodle {
	AnimationManager::AnimationManager(SharedPtr<ProjectFile> proj,
		AssetLoader* assetLoader, SharedPtr<EditorController> editorController,
		KeybindsRegistry& keybinds, bool prevEnabled)
		: m_Proj(proj), m_SelectedFrame(0), m_Player(new AnimationPlayer(proj)),
		  m_EditorController(editorController), m_AssetLoader(assetLoader),
		  m_Keybinds(keybinds),
		  m_ToolManager(std::make_unique<ToolManager>(keybinds)) {
		m_FrameRenderer =
			std::make_unique<FrameRenderer>(nullptr, -1, m_ToolManager.get(),
				m_Player.get(), m_EditorController, prevEnabled);
		m_TimelineFrameRenderer =
			std::make_unique<FrameRenderer>(nullptr, -1, m_ToolManager.get(),
				m_Player.get(), m_EditorController, prevEnabled);
		m_FrameRenderer->SetUndoByStroke(m_UndoByStroke);
	}

	AnimationManager::~AnimationManager() {}

	void AnimationManager::RegisterKeybinds() {
		m_Keybinds.Register("rewind", {false, false, false, ImGuiKey_J});
		m_Keybinds.Register("play", {false, false, false, ImGuiKey_K});
		m_Keybinds.Register("end", {false, false, false, ImGuiKey_L});
		m_ToolManager->RegisterKeybinds();
	}

	void AnimationManager::RenderTimeline(bool prevEnabled) {
		// Set scrollbar size (thickness)
		ImGui::GetStyle().ScrollbarSize =
			20.0f;	// Increase the thickness of the scrollbars

		// Lock window height but allow horizontal resizing
		float fixedHeight = 160.0f;
		ImGui::SetNextWindowSizeConstraints(
			ImVec2(0, fixedHeight), ImVec2(FLT_MAX, fixedHeight));

		// Begin the window with horizontal scrollbar enabled
		ImGui::SetNextWindowPos(ImVec2(0, 920), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(1074, 160), ImGuiCond_FirstUseEver);
		ImGui::Begin("Timeline", nullptr,
			ImGuiWindowFlags_HorizontalScrollbar |
			ImGuiWindowFlags_NoBackground);
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		float frameWidth = (float)m_Proj->AnimWidth();
		float frameHeight = (float)m_Proj->AnimHeight();
		float padding = 25.0f;

		// Calculate total width required for all frames
		float totalWidth = m_Proj->AnimFrameCount() * (frameWidth + padding);

		// Create a scrollable region
		ImGui::BeginChild("FrameScrollRegion",
			ImVec2(ImGui::GetContentRegionAvail().x, frameHeight + padding),
			false, ImGuiWindowFlags_HorizontalScrollbar);

		// Get the initial top-left position
		ImVec2 topLeft = ImGui::GetCursorScreenPos();
		ImVec2 bottomRight =
			ImVec2(topLeft.x + frameWidth, topLeft.y + frameHeight);

		ImFont* font = ImGui::GetFont();
		float fontSize = ImGui::GetFontSize();

		TimelineKeyContext keyContext;
		keyContext.Proj = m_Proj;
		keyContext.Player = m_Player.get();
		keyContext.FrameRenderer = m_FrameRenderer.get();
		keyContext.SelectedFrame = &m_SelectedFrame;
		KeyHandler::HandleTimelineShortcuts(keyContext, m_Keybinds);
		if (m_SelectedFrame >= m_Proj->AnimFrameCount()) {
			m_SelectedFrame = m_Proj->AnimFrameCount() - 1;
		}

		// Render frames
		for (unsigned long i = 0; i < m_Proj->AnimFrameCount(); i++) {
			drawList->AddText(font, fontSize,
				m_SelectedFrame == i
					? ImVec2(topLeft.x + frameWidth / 2, bottomRight.y + 10)
					: ImVec2(topLeft.x + frameWidth / 2, bottomRight.y),
				IM_COL32(255, 255, 255, 255), std::to_string(i).c_str());

			if (m_TimelineFrameRenderer->Ctx()->Frame !=
				m_Proj->AnimFrames()->Get(i)) {
				m_TimelineFrameRenderer->Ctx()->Frame =
					m_Proj->AnimFrames()->Get(i);
			}

			float width = bottomRight.x - topLeft.x;
			float height = bottomRight.y - topLeft.y;
			float scaleX = width / frameWidth;
			float scaleY = width / frameHeight;

			m_TimelineFrameRenderer->Ctx()->PixelScale =
				std::min<float>(scaleX, scaleY);

			m_TimelineFrameRenderer->RenderFramePixels(
				topLeft.x, topLeft.y, ImGui::GetWindowDrawList(), true);

			if ((m_Player->Playing() && m_Player->CurFrame() == i) ||
				(!m_Player->Playing() && m_SelectedFrame == i)) {

				const auto frames = m_Proj->AnimFrames();
				if (m_FrameRenderer->Ctx()->Frame != frames->Get(i))
					m_FrameRenderer->Ctx()->Frame = frames->Get(i);

				m_FrameRenderer->Ctx()->Index = i;

				if (i > 0) {
					m_FrameRenderer->Ctx()->PreviousFrame = frames->Get(i - 1);
				}
				m_FrameRenderer->RenderFrame();
				drawList->AddRect(topLeft, ImVec2(bottomRight.x, bottomRight.y),
					IM_COL32(255, 0, 0, 255),  // Red color
					0.0f,					   // rounding
					0,						   // flags
					8.0f  // thickness - increased to make it much thicker
				);
			}
			ImVec2 mousePos = ImGui::GetMousePos();
			bool isHovered =
				(mousePos.x >= topLeft.x && mousePos.x <= bottomRight.x &&
					mousePos.y >= topLeft.y && mousePos.y <= bottomRight.y);

			char menuName[32];	// Make buffer big enough for "frame" + numbers
								// + "menu" + null terminator
			snprintf(menuName, 31, "##frame%ldmenu", i);
			char* menuNamePtr = menuName;

			// Create unique ID for this popup
			if (isHovered && ImGui::IsMouseClicked(1)) {
				ImGui::OpenPopup(menuNamePtr);
			} else if (isHovered && ImGui::IsMouseClicked(0)) {
				m_SelectedFrame = i;
			}

			if (ImGui::BeginPopup(menuNamePtr)) {
				if (ImGui::MenuItem("Delete", "\\")) {
					if (m_Proj->AnimFrameCount() != 1) {
						Frame deletedFrame =
							*m_Proj->AnimFrames()->Get(m_SelectedFrame);
						m_Proj->AnimFrames()->Remove(m_SelectedFrame);
						DeleteFrameAction action = DeleteFrameAction(
							m_SelectedFrame, &deletedFrame, m_Proj);
						m_Proj->PushUndoable(action);
						m_Proj->AnimFrames()->Remove(i);
					}
				}
				if (ImGui::MenuItem("Insert before", "O")) {
					m_Proj->AnimFrames()->InsertBeforeEmpty(m_SelectedFrame);
					m_SelectedFrame++;
					InsertFrameAction action =
						InsertFrameAction(m_SelectedFrame - 1, m_Proj);
					m_Proj->PushUndoable(action);
				}
				if (ImGui::MenuItem("Insert after", "P")) {
					m_Proj->AnimFrames()->InsertAfterEmpty(m_SelectedFrame);
					InsertFrameAction action =
						InsertFrameAction(m_SelectedFrame + 1, m_Proj);
					m_Proj->PushUndoable(action);
				}
				if (ImGui::MenuItem("Move forward", "I")) {
					m_Proj->AnimFrames()->MoveForward(i);
				}
				if (ImGui::MenuItem("Move backward", "U")) {
					m_Proj->AnimFrames()->MoveBackward(i);
				}
				if (ImGui::MenuItem("Copy", ",")) {
					m_Proj->AnimFrames()->Get(i)->CopyToClipboard();
				}
				if (ImGui::MenuItem("Paste before", ".")) {
					Frame* frame = Frame::PastedFrame();
					m_Proj->AnimFrames()->InsertBefore(i, frame);
				}
				if (ImGui::MenuItem("Paste after", "/")) {
					Frame* frame = Frame::PastedFrame();
					m_Proj->AnimFrames()->InsertAfter(i, frame);
				}
				ImGui::EndPopup();
			}

			topLeft.x += frameWidth + padding;
			bottomRight.x += frameWidth + padding;
		}

		// Ensure the scroll region size is based on total width of all frames
		ImGui::Dummy(ImVec2(totalWidth - 25, frameHeight));

		ImGui::EndChild();
		ImGui::End();

		m_ToolManager->RenderTools();
	}

	void AnimationManager::RenderControls() {
		ImGui::Begin("Controls");

		if (ImGui::ImageButton("rewind", (ImTextureID)(intptr_t)s_RewindTexId,
				ImVec2(20, 20)) ||
			(m_Keybinds.Get("rewind").IsPressed() &&
				!ImGui::IsAnyItemActive())) {
			m_SelectedFrame = 0;
			m_Player->Rewind();
		}

		ImGui::SameLine();

		if (ImGui::ImageButton("togglePlay",
				m_Player->Playing() ? (ImTextureID)(intptr_t)s_PauseTexId
									: (ImTextureID)(intptr_t)s_PlayTexId,
				ImVec2(20, 20)) ||
			(m_Keybinds.Get("play").IsPressed() && !ImGui::IsAnyItemActive())) {
			m_Player->SetPlaying(!m_Player->Playing());
		}

		ImGui::SameLine();

		if (ImGui::ImageButton(
				"end", (ImTextureID)(intptr_t)s_EndTexId, ImVec2(20, 20)) ||
			(m_Keybinds.Get("end").IsPressed() && !ImGui::IsAnyItemActive())) {
			m_SelectedFrame = m_Proj->AnimFrameCount() - 1;
			m_Player->End();
		}

		ImGui::End();
	}

	void AnimationManager::RenderLogs() {
		ImGui::Begin("Logs");

		const auto logColor = [](const char* s) -> ImVec4 {
			if (!s) {
				return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			}
			if (std::strstr(s, "[Error]") || std::strstr(s, "[FATAL]")) {
				return ImVec4(1.0f, 0.35f, 0.35f, 1.0f);
			}
			if (std::strstr(s, "[Warn]")) {
				return ImVec4(1.0f, 0.75f, 0.25f, 1.0f);
			}
			if (std::strstr(s, "[Debug]")) {
				return ImVec4(0.45f, 0.85f, 1.0f, 1.0f);
			}
			if (std::strstr(s, "[Info]")) {
				return ImVec4(0.55f, 0.75f, 1.0f, 1.0f);
			}
			if (std::strstr(s, "[Note]")) {
				return ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
			}
			return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		};

		if (ImGui::Button("Clear")) {
			for (char* log : s_Logs) {
				delete[] log;
			}
			s_Logs.clear();
		}

		ImGui::SameLine();

		if (ImGui::Button("Copy")) {
			ImGui::LogToClipboard();

			for (const char* str : s_Logs) {
				ImGui::LogText("%s\n", str ? str : "");
			}

			ImGui::LogFinish();
		}

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
		ImGui::BeginChild("##logscroll", ImVec2(-1, -1), false,
			ImGuiWindowFlags_HorizontalScrollbar);

		for (const char* str : s_Logs) {
			ImGui::TextColored(logColor(str), "%s", str ? str : "");
		}

		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();
		ImGui::PopStyleColor();

		ImGui::End();
	}
}  // namespace FuncDoodle
