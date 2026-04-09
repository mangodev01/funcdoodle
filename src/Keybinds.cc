#include "Keybinds.h"
#include "exepath.h"
#include "imgui.h"
#include <iterator>
#define TOML_EXCEPTIONS 0
#include "toml++.h"
#include <cstring>
#include <map>

#include "MacroUtils.h"

namespace FuncDoodle {
	KeyMask::KeyMask() {
		for (int i = 0; i < KEY_MASK_SIZE; i++) {
			m_Keys[i] = 0;
		}
	}

	KeyMask::KeyMask(ImGuiKey key) {
		for (int i = 0; i < KEY_MASK_SIZE; i++) {
			m_Keys[i] = 0;
		}
		if (key != ImGuiKey_None) {
			int bucket = key / 64;
			m_Keys[bucket] |= (1ull << (key % 64));
		}
	}

	bool KeyMask::IsPressed() const {
		for (int i = 0; i < KEY_MASK_SIZE; i++) {
			if (m_Keys[i] == 0)
				continue;
			for (int j = 0; j < 64; j++) {
				if (m_Keys[i] & (1ull << j)) {
					ImGuiKey key = (ImGuiKey)(i * 64 + j);

					if (ImGui::IsKeyPressed(key)) {
						return true;
					}
				}
			}
		}
		return false;
	}

	KeyMask::operator char*() const {
		static char buf[1024] = {};
		buf[0] = '\0';
		int offset = 0;
		bool first = true;
		for (int k = ImGuiKey_NamedKey_BEGIN; k < ImGuiKey_NamedKey_END; k++) {
			int bucket = k / 64;
			if (m_Keys[bucket] & (1ull << (k % 64))) {
				const char* keyName = ImGui::GetKeyName((ImGuiKey)k);
				if (!keyName)
					continue;

				if (offset >= sizeof(buf))
					break;

				if (!first) {
					offset +=
						snprintf(buf + offset, sizeof(buf) - offset, " | ");
				}

				offset +=
					snprintf(buf + offset, sizeof(buf) - offset, "%s", keyName);
				first = false;
			}
		}
		return buf;
	}

	KeyMask KeyMask::operator|(const KeyMask& other) const {
		KeyMask result;
		for (int i = 0; i < KEY_MASK_SIZE; i++) {
			result.m_Keys[i] = m_Keys[i] | other.m_Keys[i];
		}
		return result;
	}

	Shortcut::Shortcut()
		: RequiresCtrl(false), RequiresShift(false), RequiresSuper(false),
		  Key(ImGuiKey_None) {}

	Shortcut::Shortcut(
		bool requiresCtrl, bool requiresShift, bool requiresSuper, KeyMask key)
		: RequiresCtrl(requiresCtrl), RequiresShift(requiresShift),
		  RequiresSuper(requiresSuper), Key(key) {}

	Shortcut::Shortcut(const char* str)
		: RequiresCtrl(false), RequiresShift(false), RequiresSuper(false),
		  Key(ImGuiKey_None) {
		if (!str || !*str)
			return;

		auto trim = [](std::string& s) {
			while (!s.empty() && std::isspace(s.front()))
				s.erase(s.begin());
			while (!s.empty() && std::isspace(s.back()))
				s.pop_back();
		};

		auto keyFromName = [](const std::string& name) -> ImGuiKey {
			if (name.length() == 1 && name[0] >= '0' && name[0] <= '9') {
				return (ImGuiKey)(ImGuiKey_0 + (name[0] - '0'));
			}
			for (int k = ImGuiKey_NamedKey_BEGIN; k < ImGuiKey_NamedKey_END;
				k++) {
				const char* keyName = ImGui::GetKeyName((ImGuiKey)k);
				if (keyName && name == keyName)
					return (ImGuiKey)k;
			}
			return ImGuiKey_None;
		};

		std::string input = str;

		size_t start = 0;
		while (true) {
			size_t pos = input.find('+', start);
			std::string token = input.substr(
				start, pos == std::string::npos ? pos : pos - start);

			trim(token);

			if (token == "Ctrl")
				RequiresCtrl = true;
			else if (token == "Shift")
				RequiresShift = true;
			else if (token == "Cmd" || token == "Super")
				RequiresSuper = true;
			else {
				// Key mask (may contain |)
				KeyMask mask;

				size_t kstart = 0;
				while (true) {
					size_t kpos = token.find('|', kstart);
					std::string keyName = token.substr(kstart,
						kpos == std::string::npos ? kpos : kpos - kstart);
					trim(keyName);

					ImGuiKey k = keyFromName(keyName);
					if (k != ImGuiKey_None)
						mask = mask | KeyMask(k);

					if (kpos == std::string::npos)
						break;

					kstart = kpos + 1;
				}

				Key = mask;
			}

			if (pos == std::string::npos)
				break;

			start = pos + 1;
		}
	}

	Shortcut::operator char*() const {
		static char buf[256] = {};

		const char* key_str = (const char*)Key;

		snprintf(buf, sizeof(buf), "%s%s%s%s", RequiresCtrl ? "Ctrl + " : "",
			RequiresShift ? "Shift + " : "", RequiresSuper ? "Cmd + " : "",
			key_str ? key_str : "");

		return buf;
	}

	bool Shortcut::IsPressed() const {
		ImGuiIO& io = ImGui::GetIO();

		return RequiresCtrl == io.KeyCtrl && RequiresShift == io.KeyShift &&
			   RequiresSuper == io.KeySuper && Key.IsPressed();
	}

	KeybindsRegistry::KeybindsRegistry(std::filesystem::path rootPath)
		: m_Reg({}), m_RootPath(rootPath) {}
	KeybindsRegistry::~KeybindsRegistry() {
		Write();
	}

	Shortcut KeybindsRegistry::Get(const char* id) {
		auto it = std::find_if(m_Reg.begin(), m_Reg.end(),
			[id](const auto& pair) { return strcmp(pair.first, id) == 0; });
		if (it != m_Reg.end()) {
			return it->second.User.value_or(it->second.Default);
		}
		return {};
	}

	void KeybindsRegistry::Register(const char* id, Shortcut shortcut) {
		m_Reg.emplace_back(
			id, ShortcutWithUser{.Default = shortcut, .User = std::nullopt});
	}

	void KeybindsRegistry::End() {
		// read toml
		std::filesystem::path keybindsPath = m_RootPath / "keybinds.toml";

		if (!std::filesystem::exists(keybindsPath))
			return;

		auto keybindsPathStd = keybindsPath.string();
		toml::v3::noex::parse_result res =
			toml::v3::noex::parse_file(keybindsPathStd);
		toml::table* keybinds = res.table().get("keybinds")->as_table();
		for (const auto& [k, v] : *keybinds) {
			if (!v.is_string()) {
				FUNC_WARN("keybind not a string - setting to default...");
				continue;
			}

			auto sv = k.str();
			for (auto& pair : m_Reg) {
				if (std::string_view(sv) == std::string_view(pair.first)) {
					pair.second.User = v.as_string()->get().c_str();
					// FUNC_DBG("setting up, setting user: "
					// << v.as_string()->get().c_str() << "...");
					break;
				}
			}
		}
	}

	void KeybindsRegistry::Write() {
		std::filesystem::path keybindsPath = m_RootPath / "keybinds.toml";

		toml::table root;
		toml::table keybinds;

		for (auto& [id, shortcut] : m_Reg) {
			const Shortcut& s = shortcut.User.value_or(shortcut.Default);
			const char* str = (char*)s;
			keybinds.insert(id, str);
		}

		if (keybinds.empty())
			return;

		root.insert("keybinds", keybinds);

		std::ofstream out(keybindsPath);
		out << root;
	}

	std::vector<std::pair<const char*, ShortcutWithUser>>&
	KeybindsRegistry::GetAll() {
		return m_Reg;
	}
}  // namespace FuncDoodle
