/**
 * @file Keybinds.h
 * @brief Keyboard input system including key masks, shortcuts, and keybind registry.
 *
 * This file defines the input abstraction layer used by FuncDoodle for handling
 * keyboard shortcuts and user-configurable bindings.
 *
 * It includes:
 * - KeyMask: a bitmask representation of ImGui keys for efficient combination handling
 * - Shortcut: a key + modifier combination representing an action trigger
 * - ShortcutWithUser: a default shortcut with optional user override
 * - KeybindsRegistry: system for registering, storing, and persisting keybinds
 *
 * Keybinds are stored and loaded from disk (keybinds.toml) and support user overrides
 * over default application bindings.
 *
 * @note This system is tightly integrated with ImGui input handling.
 */

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

	/**
	 * @class KeyMask
	 * @brief Bitmask representing a set of ImGui keys.
	 *
	 * Stores keys internally as bits, allowing efficient storage and comparison
	 * of multiple key states. Key combinations are created using bitwise OR
	 * rather than direct multi-key construction.
	 *
	 * Used for representing keyboard input states such as shortcuts.
	 *
	 * @invariant Each ImGuiKey maps to exactly one bit in m_Keys.
	 */
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

	/**
	 * @class Shortcut
	 * @brief Keyboard shortcut definition (key + mods).
	 *
	 * Represents a combo of a main key + mods like shift, meta, super.
	 *
	 * @invariant Key holds the primary input key.
	 * @invariant RequiresXXXX hold their corresponding modifier keys.
	 */
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

	/**
	 * @class ShortcutWithUser
	 * @brief Stores a default shortcut and an optional user-defined one.
	 *
	 * Contains two shortcuts:
	 * - Default: is used on first application start
	 * - User: optional override set by the user
	 *
	 * @note Prefer using User over Default when available.
	 */
	struct ShortcutWithUser {
		Shortcut Default;
		std::optional<Shortcut> User;
	};

	/**
	 * @class KeybindsRegistry
	 * @brief Stores and persists all keybinds used in FuncDoodle (default + user-defined)
	 *
	 * Handles loading and saving keybinds via keybinds.toml.
	 *
	 * @invariant m_RootPath must exist
	 */
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
