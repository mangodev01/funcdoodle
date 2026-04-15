#pragma once

#include <vector>

#include <cstring>
#include <filesystem>
#include <imgui.h>
#include <map>
#include <optional>
#include <vector>

namespace FuncDoodle {
	constexpr int KEY_MASK_SIZE = (ImGuiKey_NamedKey_END + 63) / 64;

	class KeyMask {
		public:
		KeyMask();
		KeyMask(ImGuiKey key);
		KeyMask(const KeyMask& other) = default;

		bool IsPressed() const;
		operator char*() const;

		void Reset() {
			for (int i = 0; i < KEY_MASK_SIZE; i++) {
				m_Keys[i] = 0;
			}
		}

		std::vector<ImGuiKey> All() const {
			std::vector<ImGuiKey> keys;
			for (int i = 0; i < KEY_MASK_SIZE; i++) {
				if (m_Keys[i] == 0)
					continue;
				for (int j = 0; j < 64; j++) {
					if (m_Keys[i] & (1ull << j)) {
						ImGuiKey key = (ImGuiKey)(i * 64 + j);
						keys.push_back(key);
					}
				}
			}
			return keys;
		}

		KeyMask operator|(const KeyMask& other) const;
		bool operator==(const KeyMask& other) const {
			for (int i = 0; i < KEY_MASK_SIZE; i++) {
				if (m_Keys[i] != other.m_Keys[i])
					return false;
			}
			return true;
		}

		private:
		unsigned long long m_Keys[KEY_MASK_SIZE];
	};

	struct Shortcut {
		Shortcut();
		Shortcut(const char* str);
		Shortcut(bool requiresCtrl, bool requiresShift, bool requiresSuper,
			KeyMask key);

		bool RequiresCtrl;
		bool RequiresShift;
		bool RequiresSuper;
		KeyMask Key;

		operator char*() const;
		bool IsPressed() const;
		bool operator==(const Shortcut& other) const {
			return RequiresCtrl == other.RequiresCtrl &&
				   RequiresShift == other.RequiresShift &&
				   RequiresSuper == other.RequiresSuper && Key == other.Key;
		}
	};

	struct ShortcutWithUser {
		Shortcut Default;
		std::optional<Shortcut> User;
	};

	class KeybindsRegistry {
		public:
		KeybindsRegistry(std::filesystem::path rootPath);
		~KeybindsRegistry();
		Shortcut Get(const char* id);
		void Register(const char* id, Shortcut shortcut);
		void End();
		void Write();
		std::vector<std::pair<const char*, ShortcutWithUser>>& GetAll();

		private:
		std::vector<std::pair<const char*, ShortcutWithUser>> m_Reg;
		std::filesystem::path m_RootPath;
	};
}  // namespace FuncDoodle
