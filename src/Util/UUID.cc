#include "UUID.h"

#include <cstdio>
#include <cstring>

#include <random>

#include "Conf/FuncPCH.h"

namespace FuncDoodle {
	std::ostream& operator<<(std::ostream& os, const UUID& obj) {
		os << obj.ToString();
		return os;
	}
	const char* UUID::ToString() const {
		static char str[37];
		str[0] = '\0';
		char temp[3];
		for (int i = 0; i < 16; i++) {
			snprintf(temp, sizeof(temp), "%02x", m_Bytes[i]);
			if (i == 4 || i == 6 || i == 8 || i == 10) {
				strcat(str, "-");
			}
			strcat(str, temp);
		}
		return str;
	}
	bool UUID::operator!=(const UUID& other) const {
		return !(*this == other);
	}
	bool UUID::operator==(const UUID& other) const {
		return m_Bytes == other.m_Bytes;
	}
	UUID UUID::Gen() {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 255);

		std::array<unsigned char, 16> bytes{};

		for (int i = 0; i < 16; ++i) {
			bytes[i] = static_cast<unsigned char>(dis(gen));
		}

		bytes[6] = (bytes[6] & 0x0f) | 0x40;
		bytes[8] = (bytes[8] & 0x3f) | 0x80;
		UUID uuid(bytes);
		return uuid;
	}
	UUID UUID::FromString(const char* str) {
		// Validate input length (expecting 36 chars: 32 hex digits + 4 hyphens)
		size_t len = strlen(str);
		if (len != 36) {
			FUNC_FATAL("Invalid UUID string length");
		}

		std::array<unsigned char, 16> bytes;
		int byteIndex = 0;

		// Temporary buffer to avoid modifying the input string
		char buf[37];  // 36 chars + null terminator
		strcpy(buf, str);

		char* tok = strtok(buf, "-");
		while (tok != nullptr && byteIndex < 16) {
			// Each token has 2, 4, 4, 4, or 12 hex digits (2, 4, 6, or 8 bytes)
			size_t tokLen = strlen(tok);
			if (tokLen % 2 != 0) {
				FUNC_FATAL("Invalid UUID token length");
			}

			for (size_t i = 0; i < tokLen && byteIndex < 16; i += 2) {
				unsigned int cur = 0;
				if (sscanf(&tok[i], "%02x", &cur) != 1) {
					FUNC_FATAL("Failed to parse UUID hex byte");
				}
				bytes[byteIndex++] = static_cast<unsigned char>(cur);
			}
			tok = strtok(nullptr, "-");
		}

		if (byteIndex != 16) {
			FUNC_FATAL("UUID string did not yield 16 bytes");
		}

		return {bytes};
	}
}  // namespace FuncDoodle
