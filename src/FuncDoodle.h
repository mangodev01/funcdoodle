/**
 * @file FuncDoodle.h
 * @brief Main namespace documentation for FuncDoodle.
 *
 * @namespace FuncDoodle
 * @brief The FuncDoodle C++ namespace.
 *
 * All classes, functions, and utilities in FuncDoodle are contained
 * within this namespace. It encompasses the complete pixel art
 * animation application, including:
 * - Application lifecycle (Application)
 * - Animation data (ProjectFile, Frame, LongIndexArray)
 * - Editing tools (ToolManager, EditorController)
 * - UI management (UIManager, Manager)
 * - Input handling (Keybinds, KeyHandler)
 * - Utilities (UUID, Ptr, Constants, etc.)
 *
 * @note This namespace avoids conflicts with other libraries.
 */

#pragma once

/**
 * @mainpage FuncDoodle
 *
 * FuncDoodle is a frame-based animation editor built with ImGui and OpenGL.
 *
 * @section features Features
 * - Frame-based animation editing with a long-index array structure
 * - Grid-based canvas with configurable visibility and snapping
 * - Asset loading system for images and sounds via PortAudio and stb_image
 * - Theme system with TOML-based theme files
 * - Action/undo system for frame, selection, and core operations
 * - Cross-platform support (Linux, macOS, Windows)
 *
 * @section build Building
 * FuncDoodle uses CMake. Debug builds are tagged with the `-dev` version suffix.
 * The project file format is versioned via @ref FDPVERMAJOR and @ref FDPVERMINOR.
 *
 * @section structure Code Structure
 * - `src/` — application source
 * - `src/Action/` — undo/redo action system
 * - `src/Platform/` — platform-specific window and input handling
 * - `lib/` — third-party libraries
 */

namespace FuncDoodle {
	// Documentation-only namespace declaration
}
