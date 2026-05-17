/**
 * @file ImUtil.h
 * @brief Small helper utilities for ImGui UI construction in FuncDoodle.
 *
 * Provides convenience functions for:
 * - button row layouts (OK/Cancel, Yes/No, etc.)
 * - keyboard input helpers
 * - alignment utilities
 * - small platform-independent utilities
 *
 * These functions are lightweight wrappers around ImGui to reduce UI
 * boilerplate.
 */

#pragma once

#include "UI/Gui.h"
#include <algorithm>
#include <fstream>
#include <filesystem>

/**
 * @namespace ImUtil
 * @brief Helper utilities for building ImGui-based UI components.
 */
namespace FuncDoodle::ImUtil {
	// NOLINTBEGIN
	inline constexpr char s_DefaultLayout[] = {
#embed "../Conf/imgui.ini"
		, 0};
	// NOLINTEND

	/**
	 * @fn SetupDefaultLayout
	 *
	 * @brief Sets up the default ImGui layout in-case there's no user-defined
	 * one.
	 */
	inline void SetupDefaultLayout() {
		ImGuiIO& io = ImGui::GetIO();
		const char* userIniPath = "imgui.ini";

		io.IniFilename = userIniPath;

		if (!std::filesystem::exists(userIniPath)) {
			ImGui::LoadIniSettingsFromMemory(s_DefaultLayout);

			std::ofstream outFile(userIniPath);
			if (outFile) {
				outFile << s_DefaultLayout;
				outFile.close();
			}
		}
	}

	/**
	 * @enum ButtonRowResult
	 * @brief Identifies which button was pressed in a button row.
	 */
	enum class ButtonRowResult { None, Primary, Secondary, Tertiary };

	/**
	 * @brief Returns the width needed to render a button with the given label.
	 *
	 * @param label Button text.
	 */
	inline float ButtonWidth(const char* label) {
		ImVec2 size = ImGui::CalcTextSize(label);
		return size.x + (ImGui::GetStyle().FramePadding.x * 2.0f);
	}

	/**
	 * @brief Right-aligns the next row of buttons within the current content
	 * region.
	 *
	 * @param totalWidth Total width of the button row.
	 */
	inline void AlignButtonsRight(float totalWidth) {
		float avail = ImGui::GetContentRegionAvail().x;
		float startX =
			ImGui::GetCursorPosX() + std::max(0.0f, avail - totalWidth);
		ImGui::SetCursorPosX(startX);
	}

	/**
	 * @fn EnterPressed
	 * @brief Returns true when Enter or keypad Enter is pressed this frame.
	 *
	 * @return Whether an Enter key was pressed this frame.
	 */
	inline bool EnterPressed() {
		return ImGui::IsKeyPressed(ImGuiKey_Enter, false) ||
			   ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false);
	}

	/**
	 * @fn EscPressed
	 * @brief Returns true when Escape is pressed this frame.
	 *
	 * @return Whether Escape was pressed this frame.
	 */
	inline bool EscPressed() {
		return ImGui::IsKeyPressed(ImGuiKey_Escape, false);
	}

	/**
	 * @brief Renders CONFIRM buttons for popups.
	 *
	 * @param primary Label for the primary action button.
	 * @param secondary Label for the secondary action button.
	 * @param tertiary Optional label for the tertiary action button.
	 * @return ButtonRowResult corresponding to the button clicked by the user
	 */
	inline ButtonRowResult ConfirmButtons(const char* primary,
		const char* secondary, const char* tertiary = nullptr) {
		float spacing = ImGui::GetStyle().ItemSpacing.x;
		float primaryWidth = ButtonWidth(primary);
		float secondaryWidth = ButtonWidth(secondary);
		float tertiaryWidth = tertiary ? ButtonWidth(tertiary) : 0.0f;
		int count = tertiary ? 3 : 2;
		float totalWidth = primaryWidth + secondaryWidth + tertiaryWidth +
						   (spacing * (count - 1));
		AlignButtonsRight(totalWidth);

		ButtonRowResult result = ButtonRowResult::None;
		if (tertiary) {
			if (ImGui::Button(tertiary))
				result = ButtonRowResult::Tertiary;
			ImGui::SameLine();
		}
		if (ImGui::Button(secondary))
			result = ButtonRowResult::Secondary;
		ImGui::SameLine();
		if (ImGui::Button(primary))
			result = ButtonRowResult::Primary;
		return result;
	}

	/**
	 * @fn OkCancelButtons
	 * @brief Renders an OK/Cancel button row.
	 *
	 * @return Which button the user pressed.
	 */
	inline ButtonRowResult OkCancelButtons() {
		return ConfirmButtons("OK", "Cancel");
	}

	/**
	 * @fn YesNoButtons
	 * @brief Renders a Yes/No button row.
	 *
	 * @return Which button the user pressed.
	 */
	inline ButtonRowResult YesNoButtons() {
		return ConfirmButtons("Yes", "No");
	}

	/**
	 * @fn YesNoCancelButtons
	 * @brief Renders a Yes/No/Cancel button row.
	 *
	 * @return Which button the user pressed.
	 */
	inline ButtonRowResult YesNoCancelButtons() {
		return ConfirmButtons("Yes", "No", "Cancel");
	}

	/**
	 * @fn CloseOkButtons
	 * @brief Renders an OK/Close button row.
	 *
	 * @return Which button the user pressed.
	 */
	inline ButtonRowResult CloseOkButtons() {
		return ConfirmButtons("OK", "Close");
	}

	/**
	 * @fn ExportCloseButtons
	 * @brief Renders an Export/Close button row.
	 *
	 * @return Which button the user pressed.
	 */
	inline ButtonRowResult ExportCloseButtons() {
		return ConfirmButtons("Export", "Close");
	}

	/**
	 * @brief Renders a single right-aligned button.
	 * @param label Button label.
	 */
	inline bool SingleButtonRow(const char* label) {
		AlignButtonsRight(ButtonWidth(label));
		return ImGui::Button(label);
	}

	/**
	 * @fn OkButton
	 * @brief Renders a single OK button.
	 *
	 * @return Whether the button was pressed.
	 */
	inline bool OkButton() {
		return SingleButtonRow("OK");
	}

	/**
	 * @fn CloseButton
	 * @brief Renders a single Close button.
	 *
	 * @return Whether the button was pressed.
	 */
	inline bool CloseButton() {
		return SingleButtonRow("Close");
	}

	/**
	 * @fn GetAnyReleasedKey
	 * @brief Returns the first non-modifier key released this frame.
	 *
	 * @return The released key, or ImGuiKey_None if none matched.
	 */
	inline ImGuiKey GetAnyReleasedKey() {
		for (int i = ImGuiKey_NamedKey_BEGIN; i < ImGuiKey_NamedKey_END; i++) {
			auto key = (ImGuiKey)i;
			if (key == ImGuiKey_LeftCtrl || key == ImGuiKey_RightCtrl ||
				key == ImGuiKey_LeftShift || key == ImGuiKey_RightShift ||
				key == ImGuiKey_LeftAlt || key == ImGuiKey_RightAlt ||
				key == ImGuiKey_LeftSuper || key == ImGuiKey_RightSuper ||
				key == ImGuiKey_MouseLeft || key == ImGuiKey_MouseRight)
				continue;
			if (ImGui::IsKeyReleased(key))
				return key;
		}
		return ImGuiKey_None;
	}

	/**
	 * @fn GetUsername
	 * @brief Small util function for fetching the current users' username. Used
	 * for filling out the animation author field by default.
	 *
	 * @return Returns the current users' username.
	 *
	 * @note Returns "unknown" if it couldn't fetch the username from known ENV
	 * vars.
	 */
	inline const char* GetUsername() {
		const char* username =
			std::getenv("USER");  // Common on Linux and macOS
		if (!username) {
			username = std::getenv("LOGNAME");	// Fallback for Linux and macOS
		}
		if (!username) {
			username = std::getenv("USERNAME");	 // Common on Windows
		}
		if (!username) {
			username = "unknown";
		}

		return username;
	}
}  // namespace FuncDoodle::ImUtil
