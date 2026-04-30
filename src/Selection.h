/**
 * @file Selection.h
 * @brief Defines selection primitives for editing regions of pixels.
 *
 * This file contains the Selection interface and its implementations,
 * used to represent and compute selectable regions within frames.
 *
 * Includes:
 * - Selection: abstract base class defining selection behavior
 * - SquareSelection: rectangular selection implementation using min/max bounds
 *
 * Selections are used for editing operations such as move, delete, rotate,
 * and other region-based transformations.
 *
 * @warning This system is designed for subclassing; behavior depends on derived types.
 */

#pragma once

#include "Gui.h"
#include <vector>

namespace FuncDoodle {
	/**
	 * @class Selection
	 * @brief Base class for selection shapes.
	 *
	 * Defines the interface for retrieving selected points.
	 *
	 * @warning Intended to be subclassed. Behavior depends on derived implementations.
	 */
	class Selection {
		public:
		virtual ~Selection();
		virtual std::vector<ImVec2i> All();
	};

	/**
	 * @struct SquareSelection
	 * @brief Rectangular selection defined by two corner points.
	 *
	 * Represents a square/rectangle selection region using a minimum and maximum
	 * coordinate. Used to collect all points inside the defined bounds.
	 */
	struct SquareSelection : public Selection {
		bool Active;
		ImVec2i Min;
		ImVec2i Max;

		std::vector<ImVec2i> All() override;
	};

	// will add more selection types later on if i feel like it (eg CircleSelection, LassoSelection)
}  // namespace FuncDoodle
