/**
 * @file Ptr.h
 * @brief Convenience type aliases for standard smart pointers.
 *
 * This header provides simple aliases over the C++ standard library smart pointers:
 * - UniquePtr: exclusive ownership (std::unique_ptr)
 * - SharedPtr: shared ownership with reference counting (std::shared_ptr)
 * - WeakPtr: non-owning weak reference to a SharedPtr (std::weak_ptr)
 *
 * These aliases are used throughout FuncDoodle to improve readability and allow
 * future replacement with custom smart pointer implementations if needed.
 *
 * @note No additional behavior is added beyond std::smart pointers.
 */

#pragma once

#include <memory>

// don't think any Doxygen comments are needed here - pretty self-explanatory header file

// in-case i'd ever want to make my own smart ptr thing or smth
// with like ref count
// for debugging
/** @brief Alias for an exclusively owned smart pointer. */
template <typename T> using UniquePtr = std::unique_ptr<T>;

/** @brief Alias for a reference-counted shared smart pointer. */
template <typename T> using SharedPtr = std::shared_ptr<T>;

/** @brief Alias for a non-owning observer of a SharedPtr. */
template <typename T> using WeakPtr = std::weak_ptr<T>;
