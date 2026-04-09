#include "Action/Action.h"
#include "Project.h"
#include "Selection.h"
#include "Test.h"

int FuncDoodle_RunActionTests() {
	TEST_SCOPE("Action Tests");

	// Rotate frame action
	{
		char name[] = "Test";
		char author[] = "Tester";
		char desc[] = "Rotate frame";
		auto proj = std::make_shared<FuncDoodle::ProjectFile>(name, 3, 3,
			author, 12, desc, nullptr, FuncDoodle::Col{255, 255, 255});

		FuncDoodle::Frame* frame = proj->AnimFrames()->Get(0);
		FuncDoodle::Col A{255, 0, 0};
		FuncDoodle::Col B{0, 255, 0};
		FuncDoodle::Col C{0, 0, 255};
		FuncDoodle::Col D{255, 255, 0};
		FuncDoodle::Col E{255, 0, 255};
		FuncDoodle::Col F{0, 255, 255};
		FuncDoodle::Col G{128, 128, 128};
		FuncDoodle::Col H{64, 64, 64};
		FuncDoodle::Col I{32, 32, 32};

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
			author, 12, desc, nullptr, FuncDoodle::Col{255, 255, 255});
		FuncDoodle::Frame* frame = proj->AnimFrames()->Get(0);

		auto sel = std::make_shared<FuncDoodle::SquareSelection>();
		sel->Active = false;
		sel->Min = ImVec2i(1, 1);
		sel->Max = ImVec2i(2, 2);

		FuncDoodle::Col SA{10, 20, 30};
		FuncDoodle::Col SB{40, 50, 60};
		FuncDoodle::Col SC{70, 80, 90};
		FuncDoodle::Col SD{100, 110, 120};

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
			author, 12, desc, nullptr, FuncDoodle::Col{255, 255, 255});
		FuncDoodle::Frame* frame = proj->AnimFrames()->Get(0);

		auto sel = std::make_shared<FuncDoodle::SquareSelection>();
		sel->Active = false;
		sel->Min = ImVec2i(1, 1);
		sel->Max = ImVec2i(2, 2);

		FuncDoodle::Col SA{10, 20, 30};
		FuncDoodle::Col SB{40, 50, 60};
		FuncDoodle::Col SC{70, 80, 90};
		FuncDoodle::Col SD{100, 110, 120};

		frame->SetPixel(1, 1, SA);
		frame->SetPixel(2, 1, SB);
		frame->SetPixel(1, 2, SC);
		frame->SetPixel(2, 2, SD);

		std::vector<ImVec2i> sel_pixels = sel->All();
		std::vector<FuncDoodle::Col> prev_pixels;
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
