/**
 * @file UUID.h
 * @brief Defines a custom 16-byte UUID (universally unique identifier).
 *
 * This file contains the UUID class used in FuncDoodle themes.
 *
 * The implementation uses a fixed 16-byte array and provides:
 * - String serialization/deserialization
 * - Equality and ordering operators
 * - Simple FNV-based hashing support
 * - Basic arithmetic operators (XOR and -)
 *
 * @note This UUID implementation is custom and does NOT follow RFC 4122.
 * @warning Operator+ and operator- are not mathematically meaningful UUID
 * operations; they are used purely for internal utility behavior.
 */

#pragma once
#include <array>
#include <iostream>
#include <memory.h>
#include <ostream>

namespace FuncDoodle {
	/**
	 * @class UUID
	 * @brief Represents a single 16-byte opaque uuid-like unique identifier.
	 *
	 * @note This does not conform to RFC 4122 and uses a custom format.
	 */
	class UUID {
		public:
		/** @brief Constructs a UUID from raw bytes. */
		UUID(std::array<unsigned char, 16> bytes) { m_Bytes = bytes; }
		UUID() { m_Bytes = std::array<unsigned char, 16>(); }
		/** @brief Returns the raw 16-byte UUID payload. */
		std::array<unsigned char, 16> Bytes() { return m_Bytes; }
		/** @brief Converts the UUID into its string form. */
		[[nodiscard]] const char* ToString() const;
		/** @brief Compares two UUID values for equality. */
		bool operator==(const UUID& other) const;
		/** @brief Compares two UUID values for inequality. */
		bool operator!=(const UUID& other) const;
		/** @brief Generates a new UUID value. */
		static UUID Gen();
		/** @brief Parses a UUID from a string representation. */
		static UUID FromString(const char* str);
		/** @brief Streams the UUID in its string form. */
		friend std::ostream& operator<<(std::ostream& os, const UUID& obj);
		/** @brief Hash functor for using UUID as an associative-container key.
		 */
		struct Hash {
			/** @brief Computes a stable hash for a UUID value. */
			size_t operator()(const UUID& uuid) const {
				static const size_t FNV_PRIME = 1099511628211ULL;
				static const size_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
				size_t hash = FNV_OFFSET_BASIS;
				for (size_t i = 0; i < 16; ++i) {
					hash ^= static_cast<size_t>(uuid.m_Bytes[i]);
					hash *= FNV_PRIME;
				}
				return hash;
			}
		};
		/** @brief Orders UUID values lexicographically by raw bytes. */
		bool operator<(const UUID& other) const {
			return m_Bytes < other.m_Bytes;
		}
		/** @brief Orders UUID values lexicographically by raw bytes. */
		bool operator>(const UUID& other) const {
			return m_Bytes > other.m_Bytes;
		}
		/** @brief Returns true when this UUID sorts before or equal to another.
		 */
		bool operator<=(const UUID& other) const {
			return m_Bytes <= other.m_Bytes;
		}
		/** @brief Returns true when this UUID sorts after or equal to another.
		 */
		bool operator>=(const UUID& other) const {
			return m_Bytes >= other.m_Bytes;
		}

		/** @brief Combines two UUID values with byte-wise XOR. */
		UUID operator+(const UUID& other) const {
			UUID result;
			for (int i = 0; i < 16; ++i) {
				result.m_Bytes[i] =
					this->m_Bytes[i] ^ other.m_Bytes[i];  // XOR operation
			}
			return result;
		}

		/** @brief Subtracts UUID bytes pairwise. */
		UUID operator-(const UUID& other) const {
			UUID result;
			for (int i = 0; i < 16; ++i) {
				result.m_Bytes[i] = this->m_Bytes[i] - other.m_Bytes[i];
			}
			return result;
		}

		private:
		std::array<unsigned char, 16> m_Bytes;
	};
}  // namespace FuncDoodle
