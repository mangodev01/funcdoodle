/**
 * @file PopupRegistry.h
 * @brief Central registry for UI popup state management.
 *
 * This file defines the PopupRegistry class, which manages the open/close state
 * of UI popups within FuncDoodle.
 *
 * Responsibilities include:
 * - Registering popup identifiers
 * - Opening and closing individual popups
 * - Querying popup visibility state
 * - Bulk closing operations (all or all except one)
 *
 * Popups are stored in a map keyed by string_view identifiers.
 *
 * @note Popup state is stored as simple boolean flags.
 * @warning string_view keys must reference stable string storage elsewhere.
 */

#pragma once

#include <map>
#include <string_view>

namespace FuncDoodle {
	/**
	 * @class PopupRegistry
	 * @brief Stores all popups used in FuncDoodle
	 *
	 * Handles fetching and setting popup state.
	 *
	 * @see KeybindsRegistry
	 */
	class PopupRegistry {
		public:
		PopupRegistry() = default;
		~PopupRegistry() = default;

		void Register(std::string_view id);
		void Open(std::string_view id);
		void Close(std::string_view id);

		[[nodiscard]] bool IsOpen(std::string_view id) const;

		bool* Get(std::string_view id);
		void CloseAll();
		void CloseAllExcept(std::string_view exception);

		private:
		std::map<std::string_view, bool, std::less<>> m_Popups;
	};
}  // namespace FuncDoodle
