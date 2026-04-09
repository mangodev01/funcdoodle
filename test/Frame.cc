#include "Frame.h"
#include "Action/Direction.h"
#include "Test.h"

int FuncDoodle_RunFrameTests() {
	TEST_SCOPE("Frame Tests");

	FuncDoodle::Frame frame1;
	CHECK((frame1.Width() == 1), "Default width should be 1");
	CHECK((frame1.Height() == 1), "Default height should be 1");

	FuncDoodle::Frame frame2(10, 20, FuncDoodle::Col{255, 0, 0});
	CHECK((frame2.Width() == 10), "width should be 10");
	CHECK((frame2.Height() == 20), "height should be 20");
	CHECK((frame2.Pixels()->BgCol() == FuncDoodle::Col{255, 0, 0}),
		"bgCol should be red");

	FuncDoodle::Frame frame3(5, 5, FuncDoodle::Col{0, 255, 0});
	frame3.SetPixel(0, 0, FuncDoodle::Col{255, 0, 0});
	frame3.SetPixel(4, 4, FuncDoodle::Col{0, 0, 255});
	CHECK((frame3.Pixels()->Get(0, 0) == FuncDoodle::Col{255, 0, 0}),
		"SetPixel should work at (0,0)");
	CHECK((frame3.Pixels()->Get(4, 4) == FuncDoodle::Col{0, 0, 255}),
		"SetPixel should work at (4,4)");

	FuncDoodle::Col center_col = frame3.Pixels()->Get(2, 2);
	CHECK((center_col == FuncDoodle::Col{0, 255, 0}),
		"Unset pixel should have bgCol");

	FuncDoodle::Frame frame_copy = frame2;
	CHECK((frame_copy.Width() == frame2.Width()),
		"Copy constructor should preserve width");
	CHECK((frame_copy.Height() == frame2.Height()),
		"Copy constructor should preserve height");

	FuncDoodle::Frame frame_assigned;
	frame_assigned = frame2;
	CHECK((frame_assigned.Width() == frame2.Width()),
		"Assignment should preserve width");
	CHECK((frame_assigned.Height() == frame2.Height()),
		"Assignment should preserve height");

	FuncDoodle::Frame frame_eq1(10, 10, FuncDoodle::Col{100, 100, 100});
	FuncDoodle::Frame frame_eq2(10, 10, FuncDoodle::Col{100, 100, 100});
	CHECK((frame_eq1 == frame_eq2), "Identical frames should be equal");

	FuncDoodle::Frame frame_neq(10, 10, FuncDoodle::Col{100, 100, 101});
	CHECK((frame_eq1 != frame_neq), "Different frames should not be equal");

	FuncDoodle::Frame frame_diff_size1(10, 10, FuncDoodle::Col{100, 100, 100});
	FuncDoodle::Frame frame_diff_size2(20, 10, FuncDoodle::Col{100, 100, 100});
	CHECK((frame_diff_size1 != frame_diff_size2),
		"Different size frames should not be equal");

	FuncDoodle::Frame frame_reinit(5, 5, FuncDoodle::Col{0, 0, 0});
	frame_reinit.ReInit(100, 50, FuncDoodle::Col{128, 128, 128});
	CHECK((frame_reinit.Width() == 100), "ReInit should change width");
	CHECK((frame_reinit.Height() == 50), "ReInit should change height");
	CHECK((frame_reinit.Pixels()->BgCol() == FuncDoodle::Col{128, 128, 128}),
		"ReInit should change bgCol");

	std::vector<FuncDoodle::Col> data = frame2.Data();
	CHECK((data.size() == 200), "Data size should be width * height");

	FuncDoodle::ImageArray arr(3, 3, FuncDoodle::Col{255, 255, 0});
	FuncDoodle::Frame frame_from_arr(arr);
	CHECK((frame_from_arr.Width() == 3),
		"Frame from ImageArray width should be 3");
	CHECK((frame_from_arr.Height() == 3),
		"Frame from ImageArray height should be 3");

	FuncDoodle::ImageArray* arr_ptr =
		new FuncDoodle::ImageArray(4, 4, FuncDoodle::Col{0, 255, 255});
	FuncDoodle::Frame frame_from_ptr(arr_ptr);
	CHECK(
		(frame_from_ptr.Width() == 4), "Frame from pointer width should be 4");
	CHECK((frame_from_ptr.Height() == 4),
		"Frame from pointer height should be 4");
	delete arr_ptr;

	FuncDoodle::ImageArray* null_arr = nullptr;
	FuncDoodle::Frame frame_from_null_ptr(null_arr);
	CHECK((frame_from_null_ptr.Width() == 1),
		"Frame from null ptr should have default width");
	CHECK((frame_from_null_ptr.Height() == 1),
		"Frame from null ptr should have default height");

	FuncDoodle::Frame frame_mut = frame2;
	frame_mut.PixelsMut()->SetBG(FuncDoodle::Col{255, 255, 255});
	CHECK((frame_mut.Pixels()->BgCol() == FuncDoodle::Col{255, 255, 255}),
		"PixelsMut should allow mutation");

	// Frame rotation (clockwise)
	FuncDoodle::Frame rot_frame(3, 3, FuncDoodle::Col{255, 255, 255});
	FuncDoodle::Col A{255, 0, 0};
	FuncDoodle::Col B{0, 255, 0};
	FuncDoodle::Col C{0, 0, 255};
	FuncDoodle::Col D{255, 255, 0};
	FuncDoodle::Col E{255, 0, 255};
	FuncDoodle::Col F{0, 255, 255};
	FuncDoodle::Col G{128, 128, 128};
	FuncDoodle::Col H{64, 64, 64};
	FuncDoodle::Col I{32, 32, 32};

	rot_frame.SetPixel(0, 0, A);
	rot_frame.SetPixel(1, 0, B);
	rot_frame.SetPixel(2, 0, C);
	rot_frame.SetPixel(0, 1, D);
	rot_frame.SetPixel(1, 1, E);
	rot_frame.SetPixel(2, 1, F);
	rot_frame.SetPixel(0, 2, G);
	rot_frame.SetPixel(1, 2, H);
	rot_frame.SetPixel(2, 2, I);

	FuncDoodle::Frame rot_original = rot_frame;
	rot_frame.Rotate(90);

	CHECK((rot_frame.Pixels()->Get(0, 0) == G),
		"Rotate(90) should move (0,2)->(0,0)");
	CHECK((rot_frame.Pixels()->Get(1, 0) == D),
		"Rotate(90) should move (0,1)->(1,0)");
	CHECK((rot_frame.Pixels()->Get(2, 0) == A),
		"Rotate(90) should move (0,0)->(2,0)");
	CHECK((rot_frame.Pixels()->Get(0, 1) == H),
		"Rotate(90) should move (1,2)->(0,1)");
	CHECK(
		(rot_frame.Pixels()->Get(1, 1) == E), "Rotate(90) should keep center");
	CHECK((rot_frame.Pixels()->Get(2, 1) == B),
		"Rotate(90) should move (1,0)->(2,1)");
	CHECK((rot_frame.Pixels()->Get(0, 2) == I),
		"Rotate(90) should move (2,2)->(0,2)");
	CHECK((rot_frame.Pixels()->Get(1, 2) == F),
		"Rotate(90) should move (2,1)->(1,2)");
	CHECK((rot_frame.Pixels()->Get(2, 2) == C),
		"Rotate(90) should move (2,0)->(2,2)");

	rot_frame.Rotate(-90);
	CHECK((rot_frame == rot_original),
		"Rotate(90) then Rotate(-90) should restore the original frame");

	// Selection rotation
	FuncDoodle::Frame sel_frame(4, 4, FuncDoodle::Col{255, 255, 255});
	auto sel = std::make_shared<FuncDoodle::SquareSelection>();
	sel->Active = false;
	sel->Min = ImVec2i(1, 1);
	sel->Max = ImVec2i(2, 2);

	FuncDoodle::Col SA{10, 20, 30};
	FuncDoodle::Col SB{40, 50, 60};
	FuncDoodle::Col SC{70, 80, 90};
	FuncDoodle::Col SD{100, 110, 120};

	sel_frame.SetPixel(1, 1, SA);
	sel_frame.SetPixel(2, 1, SB);
	sel_frame.SetPixel(1, 2, SC);
	sel_frame.SetPixel(2, 2, SD);

	FuncDoodle::Frame sel_original = sel_frame;
	WeakPtr<FuncDoodle::Selection> selPtr = sel;

	sel_frame.RotateSelection(selPtr, 90);

	CHECK((sel_frame.Pixels()->Get(1, 1) == SC),
		"RotateSelection(90) should rotate within selection");
	CHECK((sel_frame.Pixels()->Get(2, 1) == SA),
		"RotateSelection(90) should rotate within selection");
	CHECK((sel_frame.Pixels()->Get(1, 2) == SD),
		"RotateSelection(90) should rotate within selection");
	CHECK((sel_frame.Pixels()->Get(2, 2) == SB),
		"RotateSelection(90) should rotate within selection");

	sel_frame.RotateSelection(selPtr, -90);
	CHECK((sel_frame == sel_original),
		"RotateSelection(90) then RotateSelection(-90) should restore");

	// Selection movement
	FuncDoodle::Frame moveFrame(5, 5, FuncDoodle::Col{255, 255, 255});
	auto moveSel = std::make_shared<FuncDoodle::SquareSelection>();
	moveSel->Active = false;
	moveSel->Min = ImVec2i(1, 1);
	moveSel->Max = ImVec2i(2, 2);

	FuncDoodle::Col pixelA{10, 20, 30};
	FuncDoodle::Col pixelB{40, 50, 60};
	FuncDoodle::Col pixelC{70, 80, 90};
	FuncDoodle::Col pixelD{100, 110, 120};

	moveFrame.SetPixel(1, 1, pixelA);
	moveFrame.SetPixel(2, 1, pixelB);
	moveFrame.SetPixel(1, 2, pixelC);
	moveFrame.SetPixel(2, 2, pixelD);

	WeakPtr<FuncDoodle::Selection> moveSelPtr = moveSel;

	moveFrame.MoveSelection(moveSelPtr, FuncDoodle::Direction::Right,
		FuncDoodle::Col{255, 255, 255});
	CHECK(
		(moveSel->Min.x == 2), "MoveSelection(Right) should move Min.x right");
	CHECK(
		(moveSel->Max.x == 3), "MoveSelection(Right) should move Max.x right");
	CHECK((moveFrame.Pixels()->Get(1, 1) == FuncDoodle::Col{255, 255, 255}),
		"Old position should be bgCol");
	CHECK((moveFrame.Pixels()->Get(2, 1) == pixelA),
		"New position should have moved pixel");
	CHECK((moveFrame.Pixels()->Get(3, 1) == pixelB),
		"New position should have moved pixel");
	CHECK((moveFrame.Pixels()->Get(2, 2) == pixelC),
		"New position should have moved pixel");
	CHECK((moveFrame.Pixels()->Get(3, 2) == pixelD),
		"New position should have moved pixel");

	moveFrame.MoveSelection(moveSelPtr, FuncDoodle::Direction::Down,
		FuncDoodle::Col{255, 255, 255});
	CHECK((moveSel->Min.y == 2), "MoveSelection(Down) should move Min.y down");
	CHECK((moveSel->Max.y == 3), "MoveSelection(Down) should move Max.y down");
	CHECK((moveFrame.Pixels()->Get(2, 1) == FuncDoodle::Col{255, 255, 255}),
		"Old y position (2,1) should be bgCol");
	CHECK((moveFrame.Pixels()->Get(3, 1) == FuncDoodle::Col{255, 255, 255}),
		"Old y position (3,1) should be bgCol");
	CHECK((moveFrame.Pixels()->Get(3, 3) == pixelD),
		"pixelD should have moved to (3,3)");

	moveFrame.MoveSelection(moveSelPtr, FuncDoodle::Direction::Left,
		FuncDoodle::Col{255, 255, 255});
	CHECK((moveSel->Min.x == 1),
		"MoveSelection(Left) should move selection left");

	moveFrame.MoveSelection(
		moveSelPtr, FuncDoodle::Direction::Up, FuncDoodle::Col{255, 255, 255});
	CHECK((moveSel->Min.y == 1), "MoveSelection(Up) should move selection up");

	return 0;
}
