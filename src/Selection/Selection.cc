#include "Selection.h"
#include <ranges>

FuncDoodle::Selection::~Selection() = default;

std::vector<ImVec2i> FuncDoodle::Selection::All() {
	return {};
}

std::vector<ImVec2i> FuncDoodle::SquareSelection::All() {
	if (Active) {
		return {};
	}
	int minX = std::min(Min.x, Max.x);
	int maxX = std::max(Min.x, Max.x);
	int minY = std::min(Min.y, Max.y);
	int maxY = std::max(Min.y, Max.y);

	std::vector<ImVec2i> points;

	for (int y : std::views::iota(minY, maxY + 1))
		for (int x : std::views::iota(minX, maxX + 1))
			points.emplace_back(x, y);

	return points;
}

std::vector<ImVec2i> FuncDoodle::ArbitrarySelection::All() {
	if (!Active)
		return {};

	return All_;
}
