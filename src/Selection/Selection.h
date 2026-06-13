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
 * @warning This system is designed for subclassing; behavior depends on derived
 * types.
 */

#pragma once

#include "UI/Gui.h"
#include <vector>

namespace FuncDoodle {
	/**
	 * @class Selection
	 * @brief Base class for selection shapes.
	 *
	 * Defines the interface for retrieving selected points.
	 *
	 * @warning Intended to be subclassed. Behavior depends on derived
	 * implementations.
	 */
	class Selection {
		public:
		virtual ~Selection();
		/**
		 * @fn All()
		 * @brief Returns every pixel coordinate contained in the selection.
		 */
		virtual std::vector<ImVec2i> All();
	};

	/**
	 * @struct SquareSelection
	 * @brief Rectangular selection defined by two corner points.
	 *
	 * Represents a square/rectangle selection region using a minimum and
	 * maximum coordinate. Used to collect all points inside the defined bounds.
	 */
	struct SquareSelection : public Selection {
		/**
		 * @var Active
		 * @brief Whether the rectangular selection is currently active.
		 */
		bool Active;
		/**
		 * @var Min
		 * @brief Inclusive minimum corner of the selection.
		 */
		ImVec2i Min;
		/**
		 * @var Max
		 * @brief Inclusive maximum corner of the selection.
		 */
		ImVec2i Max;

		/**
		 * @fn All()
		 * @brief Returns every coordinate inside the rectangular bounds.
		 */
		std::vector<ImVec2i> All() override;
	};

	/**
	 * @struct ArbitrarySelection
	 * @brief Arbitrary selection defined by a list of selected pixels.
	 *
	 * Represents an arbitrary selection region using an std::vector of pixel
	 * coords. Computationally expensive, only used when you invert a selection.
	 */
	struct ArbitrarySelection : public Selection {
		/**
		 * @var Active
		 * @brief Whether the arbitrary selection is currently active.
		 */
		bool Active;

		/**
		 * @var Coords
		 * @brief Every selected coordinate
		 */
		std::vector<ImVec2i> All_;

		/**
		 * @fn All()
		 * @brief Returns every selected coordinate.
		 */
		std::vector<ImVec2i> All() override;
	};

	// will add more selection types later on if i feel like it (eg
	// CircleSelection, LassoSelection)
}  // namespace FuncDoodle
