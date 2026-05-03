#pragma once

#include <iostream>

#define ASSERT(x)                                                         \
	do {                                                                  \
		if (x) {                                                          \
			std::cout << "Test for: x:\033[34m pass\033[0m" << std::endl; \
		} else {                                                          \
			std::cout << "Test for: x:\033[31m fail\033[0m" << std::endl; \
		}                                                                 \
	} while (0)

#define ASSERT_EQ(x, y)                                                        \
	do {                                                                       \
		if (x == y) {                                                          \
			std::cout << "Test for: x == y:\033[34m pass\033[0m" << std::endl; \
		} else {                                                               \
			std::cout << "Test for: x == y:\033[31m fail\033[0m" << std::endl; \
		}                                                                      \
	} while (0)
