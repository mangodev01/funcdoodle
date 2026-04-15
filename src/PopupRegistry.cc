#include "PopupRegistry.h"

namespace FuncDoodle {
	void PopupRegistry::Register(std::string_view id) {
		m_Popups.emplace(id, false);
	}

	void PopupRegistry::Open(std::string_view id) {
		m_Popups[id] = true;
	}

	void PopupRegistry::Close(std::string_view id) {
		m_Popups[id] = false;
	}

	bool PopupRegistry::IsOpen(std::string_view id) const {
		auto it = m_Popups.find(id);
		if (it != m_Popups.end()) {
			return it->second;
		}
		return false;
	}

	bool* PopupRegistry::Get(std::string_view id) {
		return &m_Popups[id];
	}

	void PopupRegistry::CloseAll() {
		for (auto& [id, open] : m_Popups) {
			open = false;
		}
	}

	void PopupRegistry::CloseAllExcept(std::string_view exception) {
		for (auto& [id, open] : m_Popups) {
			if (id == exception)
				continue;

			open = false;
		}
	}
}  // namespace FuncDoodle
