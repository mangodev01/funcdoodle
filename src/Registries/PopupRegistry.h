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

		/**
		 * @fn Register
		 * @brief Registers a popup identifier in the registry.
		 *
		 * @param id Popup identifier to track.
		 */
		void Register(std::string_view id);
		/**
		 * @fn Open
		 * @brief Marks a popup as open.
		 *
		 * @param id Popup identifier to open.
		 */
		void Open(std::string_view id);
		/**
		 * @fn Close
		 * @brief Marks a popup as closed.
		 *
		 * @param id Popup identifier to close.
		 */
		void Close(std::string_view id);

		/**
		 * @fn IsOpen
		 * @brief Returns whether a popup is currently open.
		 *
		 * @param id Popup identifier to query.
		 * @return Whether the popup is open.
		 */
		[[nodiscard]] bool IsOpen(std::string_view id) const;

		/**
		 * @fn Get
		 * @brief Returns direct access to a popup open-state flag.
		 *
		 * @param id Popup identifier to query.
		 * @return Pointer to the popup flag, or nullptr if missing.
		 */
		bool* Get(std::string_view id);
		/**
		 * @fn CloseAll
		 * @brief Closes every registered popup.
		 */
		void CloseAll();
		/**
		 * @fn CloseAllExcept
		 * @brief Closes every popup except one.
		 *
		 * @param exception Popup identifier to keep open.
		 */
		void CloseAllExcept(std::string_view exception);

		private:
		std::map<std::string_view, bool, std::less<>> m_Popups;
	};
}  // namespace FuncDoodle
