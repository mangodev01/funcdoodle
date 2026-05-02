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

namespace FuncDoodle {
	/**
	 * @var KEY_MASK_SIZE
	 * @brief Number of 64-bit slots required to store all named ImGui keys.
	 */
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
		/**
		 * @brief Creates an empty key mask.
		 */
		KeyMask();
		/**
		 * @brief Creates a key mask containing a single ImGui key.
		 *
		 * @param key Key to set in the mask.
		 */
		KeyMask(ImGuiKey key);
		/**
		 * @brief Copies another key mask.
		 *
		 * @param other Key mask to copy.
		 */
		KeyMask(const KeyMask& other) = default;

		/**
		 * @fn IsPressed
		 * @brief Returns whether every key in the mask is currently pressed.
		 *
		 * @return Whether the masked key set is active.
		 */
		[[nodiscard]] bool IsPressed() const;
		/**
		 * @brief Converts the key mask to a display string.
		 *
		 * @return C string describing the keys in the mask.
		 */
		operator char*() const;

		/**
		 * @fn Reset
		 * @brief Clears all keys from the mask.
		 */
		void Reset() {
			for (unsigned long long & m_Key : m_Keys) {
				m_Key = 0;
			}
		}

		/**
		 * @fn All
		 * @brief Returns all keys currently stored in the mask.
		 *
		 * @return Vector of ImGui keys contained in the mask.
		 */
		[[nodiscard]] std::vector<ImGuiKey> All() const {
			std::vector<ImGuiKey> keys;
			for (int i = 0; i < KEY_MASK_SIZE; i++) {
				if (m_Keys[i] == 0)
					continue;
				for (int j = 0; j < 64; j++) {
					if (m_Keys[i] & (1ull << j)) {
						auto key = (ImGuiKey)((i * 64) + j);
						keys.push_back(key);
					}
				}
			}
			return keys;
		}

		/**
		 * @brief Returns a key mask containing keys from both operands.
		 *
		 * @param other Key mask to merge with this one.
		 * @return Combined key mask.
		 */
		KeyMask operator|(const KeyMask& other) const;
		/**
		 * @brief Returns whether two key masks contain the same keys.
		 *
		 * @param other Key mask to compare against.
		 * @return Whether both masks are equal.
		 */
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
		/**
		 * @brief Creates an empty shortcut.
		 */
		Shortcut();
		/**
		 * @brief Parses a shortcut from a string representation.
		 *
		 * @param str Shortcut string to parse.
		 */
		Shortcut(const char* str);
		/**
		 * @brief Creates a shortcut from modifier flags and a key mask.
		 *
		 * @param requiresCtrl Whether Ctrl is required.
		 * @param requiresShift Whether Shift is required.
		 * @param requiresSuper Whether Super/Meta is required.
		 * @param key Main key or key combination.
		 */
		Shortcut(bool requiresCtrl, bool requiresShift, bool requiresSuper,
			KeyMask key);

		/**
		 * @var RequiresCtrl
		 * @brief Whether Ctrl must be held.
		 */
		bool RequiresCtrl;
		/**
		 * @var RequiresShift
		 * @brief Whether Shift must be held.
		 */
		bool RequiresShift;
		/**
		 * @var RequiresSuper
		 * @brief Whether Super/Meta must be held.
		 */
		bool RequiresSuper;
		/**
		 * @var Key
		 * @brief Primary key mask for the shortcut.
		 */
		KeyMask Key;

		/**
		 * @brief Converts the shortcut to a display string.
		 *
		 * @return C string describing the shortcut.
		 */
		operator char*() const;
		/**
		 * @fn IsPressed
		 * @brief Returns whether the full shortcut is active.
		 *
		 * @return Whether the shortcut is currently pressed.
		 */
		[[nodiscard]] bool IsPressed() const;
		/**
		 * @brief Returns whether two shortcuts are identical.
		 *
		 * @param other Shortcut to compare against.
		 * @return Whether both shortcuts match.
		 */
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
		/**
		 * @var Default
		 * @brief Built-in shortcut shipped by the application.
		 */
		Shortcut Default;
		/**
		 * @var User
		 * @brief Optional user override loaded from configuration.
		 */
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
		/**
		 * @fn KeybindsRegistry
		 * @brief Creates a keybind registry rooted at a configuration directory.
		 *
		 * @param rootPath Directory containing keybind storage files.
		 */
		KeybindsRegistry(std::filesystem::path rootPath);
		~KeybindsRegistry();
		/**
		 * @fn Get
		 * @brief Returns the effective shortcut registered for an identifier.
		 *
		 * @param id Keybind identifier.
		 * @return Active shortcut for the identifier.
		 */
		Shortcut Get(const char* id);
		/**
		 * @fn Register
		 * @brief Registers a default shortcut for an identifier.
		 *
		 * @param id Keybind identifier.
		 * @param shortcut Default shortcut to store.
		 */
		void Register(const char* id, Shortcut shortcut);
		/**
		 * @fn End
		 * @brief Finalizes registration and loads any user overrides.
		 */
		void End();
		/**
		 * @fn Write
		 * @brief Persists user keybind overrides to disk.
		 */
		void Write();
		/**
		 * @fn GetAll
		 * @brief Returns all registered keybind entries.
		 *
		 * @return Mutable list of keybind identifier/shortcut pairs.
		 */
		std::vector<std::pair<const char*, ShortcutWithUser>>& GetAll();

		private:
		std::vector<std::pair<const char*, ShortcutWithUser>> m_Reg;
		std::filesystem::path m_RootPath;
	};
}  // namespace FuncDoodle
