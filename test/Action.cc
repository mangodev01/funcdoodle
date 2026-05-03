#include "Action/Action.h"
#include "Project/Project.h"
#include "Selection/Selection.h"
#include "Test/Test.h"

int FuncDoodle_RunActionTests() {
	TEST_SCOPE("Action Tests");

	// Rotate frame action
	{
		char name[] = "Test";
		char author[] = "Tester";
		char desc[] = "Rotate frame";
		auto proj = std::make_shared<FuncDoodle::ProjectFile>(name, 3, 3,
			author, 12, desc, nullptr, FuncDoodle::Col{.r=255, .g=255, .b=255});

		FuncDoodle::Frame* frame = proj->AnimFrames()->Get(0);
		FuncDoodle::Col A{.r=255, .g=0, .b=0};
		FuncDoodle::Col B{.r=0, .g=255, .b=0};
		FuncDoodle::Col C{.r=0, .g=0, .b=255};
		FuncDoodle::Col D{.r=255, .g=255, .b=0};
		FuncDoodle::Col E{.r=255, .g=0, .b=255};
		FuncDoodle::Col F{.r=0, .g=255, .b=255};
		FuncDoodle::Col G{.r=128, .g=128, .b=128};
		FuncDoodle::Col H{.r=64, .g=64, .b=64};
		FuncDoodle::Col I{.r=32, .g=32, .b=32};

		frame->SetPixel(0, 0, A);
		frame->SetPixel(1, 0, B);
		frame->SetPixel(2, 0, C);
		frame->SetPixel(0, 1, D);
		frame->SetPixel(1, 1, E);
		frame->SetPixel(2, 1, F);
		frame->SetPixel(0, 2, G);
		frame->SetPixel(1, 2, H);
		frame->SetPixel(2, 2, I);

		FuncDoodle::Frame original = *frame;

		FuncDoodle::RotateFrameAction rotate_action(0, 90, proj);
		rotate_action.Redo();

		CHECK((frame->Pixels()->Get(0, 0) == G),
			"RotateFrameAction::Redo should rotate frame");
		CHECK((frame->Pixels()->Get(2, 2) == C),
			"RotateFrameAction::Redo should rotate frame");

		rotate_action.Undo();
		CHECK((*frame == original),
			"RotateFrameAction::Undo should restore frame");
	}

	// Rotate selection action
	{
		char name[] = "Test";
		char author[] = "Tester";
		char desc[] = "Rotate selection";
		auto proj = std::make_shared<FuncDoodle::ProjectFile>(name, 4, 4,
			author, 12, desc, nullptr, FuncDoodle::Col{.r=255, .g=255, .b=255});
		FuncDoodle::Frame* frame = proj->AnimFrames()->Get(0);

		auto sel = std::make_shared<FuncDoodle::SquareSelection>();
		sel->Active = false;
		sel->Min = ImVec2i(1, 1);
		sel->Max = ImVec2i(2, 2);

		FuncDoodle::Col SA{.r=10, .g=20, .b=30};
		FuncDoodle::Col SB{.r=40, .g=50, .b=60};
		FuncDoodle::Col SC{.r=70, .g=80, .b=90};
		FuncDoodle::Col SD{.r=100, .g=110, .b=120};

		frame->SetPixel(1, 1, SA);
		frame->SetPixel(2, 1, SB);
		frame->SetPixel(1, 2, SC);
		frame->SetPixel(2, 2, SD);

		FuncDoodle::Frame original = *frame;

		FuncDoodle::RotateSelectionAction rotate_sel_action(0, sel, 90, proj);
		rotate_sel_action.Redo();

		CHECK((frame->Pixels()->Get(1, 1) == SC),
			"RotateSelectionAction::Redo should rotate selection");
		CHECK((frame->Pixels()->Get(2, 2) == SB),
			"RotateSelectionAction::Redo should rotate selection");

		rotate_sel_action.Undo();
		CHECK((*frame == original),
			"RotateSelectionAction::Undo should restore selection");
	}

	// Delete selection action
	{
		char name[] = "Test";
		char author[] = "Tester";
		char desc[] = "Delete selection";
		auto proj = std::make_shared<FuncDoodle::ProjectFile>(name, 4, 4,
			author, 12, desc, nullptr, FuncDoodle::Col{.r=255, .g=255, .b=255});
		FuncDoodle::Frame* frame = proj->AnimFrames()->Get(0);

		auto sel = std::make_shared<FuncDoodle::SquareSelection>();
		sel->Active = false;
		sel->Min = ImVec2i(1, 1);
		sel->Max = ImVec2i(2, 2);

		FuncDoodle::Col SA{.r=10, .g=20, .b=30};
		FuncDoodle::Col SB{.r=40, .g=50, .b=60};
		FuncDoodle::Col SC{.r=70, .g=80, .b=90};
		FuncDoodle::Col SD{.r=100, .g=110, .b=120};

		frame->SetPixel(1, 1, SA);
		frame->SetPixel(2, 1, SB);
		frame->SetPixel(1, 2, SC);
		frame->SetPixel(2, 2, SD);

		std::vector<ImVec2i> sel_pixels = sel->All();
		std::vector<FuncDoodle::Col> prev_pixels;
		prev_pixels.reserve(sel_pixels.size());

		for (const auto& px : sel_pixels) {
			prev_pixels.push_back(frame->Pixels()->Get(px.x, px.y));
		}

		FuncDoodle::Frame original = *frame;

		FuncDoodle::DeleteSelectionAction delete_action(
			0, sel, prev_pixels, proj);
		delete_action.Redo();

		CHECK((frame->Pixels()->Get(1, 1) == FuncDoodle::Col{255, 255, 255}),
			"DeleteSelectionAction::Redo should clear selection");
		CHECK((frame->Pixels()->Get(2, 2) == FuncDoodle::Col{255, 255, 255}),
			"DeleteSelectionAction::Redo should clear selection");

		delete_action.Undo();
		CHECK((*frame == original),
			"DeleteSelectionAction::Undo should restore selection");
	}

	return 0;
}
