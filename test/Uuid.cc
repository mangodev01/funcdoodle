#include "Util/UUID.h"
#include "Test/Test.h"

int FuncDoodle_RunUUIDTests() {
	TEST_SCOPE("UUID Tests");

	FuncDoodle::UUID uuid1;
	FuncDoodle::UUID uuid2;
	FuncDoodle::UUID uuid3;

	std::array<unsigned char, 16> bytes = {
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
	FuncDoodle::UUID uuid_from_bytes(bytes);

	std::array<unsigned char, 16> uuid_bytes = uuid_from_bytes.Bytes();
	CHECK((uuid_bytes[0] == 1), "UUID from bytes should have correct byte 0");
	CHECK(
		(uuid_bytes[15] == 16), "UUID from bytes should have correct byte 15");

	FuncDoodle::UUID uuid_same = uuid_from_bytes;
	CHECK((uuid_from_bytes == uuid_same), "UUID copy should be equal");

	FuncDoodle::UUID uuid_diff = FuncDoodle::UUID::Gen();
	CHECK(
		(uuid_from_bytes != uuid_diff), "Different UUIDs should not be equal");

	FuncDoodle::UUID uuid1_copy = uuid1;
	CHECK((uuid1 == uuid1_copy), "Same UUID should be equal to itself");

	FuncDoodle::UUID uuid_a(std::array<unsigned char, 16>{
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
	FuncDoodle::UUID uuid_b(std::array<unsigned char, 16>{
		2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
	CHECK(
		(uuid_a < uuid_b), "UUID with smaller first byte should be less than");
	CHECK((uuid_b > uuid_a),
		"UUID with larger first byte should be greater than");
	CHECK((uuid_a <= uuid_b),
		"UUID with smaller first byte should be less than or equal");
	CHECK((uuid_b >= uuid_a),
		"UUID with larger first byte should be greater than or equal");

	FuncDoodle::UUID uuid_c = uuid_a;
	CHECK((uuid_a <= uuid_c), "Same UUID should be less than or equal");
	CHECK((uuid_a >= uuid_c), "Same UUID should be greater than or equal");

	FuncDoodle::UUID uuid_xor = uuid_a + uuid_b;
	std::array<unsigned char, 16> xor_bytes = uuid_xor.Bytes();
	CHECK((xor_bytes[0] == 3), "XOR of 1 and 2 should be 3");

	FuncDoodle::UUID uuid_sub = uuid_b - uuid_a;
	std::array<unsigned char, 16> sub_bytes = uuid_sub.Bytes();
	CHECK((sub_bytes[0] == 1), "Subtraction of 1 from 2 should be 1");

	FuncDoodle::UUID uuid_gen1 = FuncDoodle::UUID::Gen();
	FuncDoodle::UUID uuid_gen2 = FuncDoodle::UUID::Gen();
	CHECK((uuid_gen1 != uuid_gen2), "Generated UUIDs should be different");

	FuncDoodle::UUID uuid_str =
		FuncDoodle::UUID::FromString("00010203-0405-0607-0809-0a0b0c0d0e0f");
	std::array<unsigned char, 16> str_bytes = uuid_str.Bytes();
	CHECK((str_bytes[0] == 0), "FromString should parse first byte correctly");
	CHECK((str_bytes[1] == 1), "FromString should parse second byte correctly");
	CHECK((str_bytes[15] == 15), "FromString should parse last byte correctly");

	FuncDoodle::UUID::Hash hash;
	size_t hash1 = hash(uuid_a);
	size_t hash2 = hash(uuid_b);
	CHECK((hash1 != hash2), "Different UUIDs should produce different hashes");

	return 0;
}
