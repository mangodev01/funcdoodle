#pragma once

#include <imgui_internal.h>
#include <vector>

namespace FuncDoodle {
	class Selection {
		public:
		virtual ~Selection();
		virtual std::vector<ImVec2i> All();
	};

	struct SquareSelection : public Selection {
		bool Active;
		ImVec2i Min;
		ImVec2i Max;

		std::vector<ImVec2i> All() override;
	};
}  // namespace FuncDoodle
