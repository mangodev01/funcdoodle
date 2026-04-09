#pragma once

#include <algorithm>

#include "imgui.h"
namespace FuncDoodle {
	namespace ImUtil {
		enum class ButtonRowResult { None, Primary, Secondary, Tertiary };

		inline float ButtonWidth(const char* label) {
			ImVec2 size = ImGui::CalcTextSize(label);
			return size.x + ImGui::GetStyle().FramePadding.x * 2.0f;
		}

		inline void AlignButtonsRight(float totalWidth) {
			float avail = ImGui::GetContentRegionAvail().x;
			float startX =
				ImGui::GetCursorPosX() + std::max(0.0f, avail - totalWidth);
			ImGui::SetCursorPosX(startX);
		}

		inline ButtonRowResult ConfirmButtons(const char* primary,
			const char* secondary, const char* tertiary = nullptr) {
			float spacing = ImGui::GetStyle().ItemSpacing.x;
			float primaryWidth = ButtonWidth(primary);
			float secondaryWidth = ButtonWidth(secondary);
			float tertiaryWidth = tertiary ? ButtonWidth(tertiary) : 0.0f;
			int count = tertiary ? 3 : 2;
			float totalWidth = primaryWidth + secondaryWidth + tertiaryWidth +
							   spacing * (count - 1);
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

		inline ButtonRowResult OkCancelButtons() {
			return ConfirmButtons("OK", "Cancel");
		}

		inline ButtonRowResult YesNoButtons() {
			return ConfirmButtons("Yes", "No");
		}

		inline ButtonRowResult YesNoCancelButtons() {
			return ConfirmButtons("Yes", "No", "Cancel");
		}

		inline ButtonRowResult CloseOkButtons() {
			return ConfirmButtons("OK", "Close");
		}

		inline ButtonRowResult ExportCloseButtons() {
			return ConfirmButtons("Export", "Close");
		}

		inline bool SingleButtonRow(const char* label) {
			AlignButtonsRight(ButtonWidth(label));
			return ImGui::Button(label);
		}

		inline bool OkButton() {
			return SingleButtonRow("OK");
		}

		inline bool CloseButton() {
			return SingleButtonRow("Close");
		}

		ImGuiKey GetAnyReleasedKey() {
			for (int i = ImGuiKey_NamedKey_BEGIN; i < ImGuiKey_NamedKey_END;
				i++) {
				ImGuiKey key = (ImGuiKey)i;
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
	}  // namespace ImUtil
}  // namespace FuncDoodle
