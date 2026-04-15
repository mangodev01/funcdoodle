#pragma once

#include <map>
#include <string>

namespace FuncDoodle {
	class PopupRegistry {
		public:
		PopupRegistry() = default;
		~PopupRegistry() = default;

		void Register(std::string_view id);
		void Open(std::string_view id);
		void Close(std::string_view id);
		bool IsOpen(std::string_view id) const;
		bool* Get(std::string_view id);
		void CloseAll();
		void CloseAllExcept(std::string_view exception);

		private:
		std::map<std::string_view, bool, std::less<>> m_Popups;
	};
}  // namespace FuncDoodle
