#include "DynArr.h"
#include "Frame.h"
#include "Test.h"

int FuncDoodle_RunColTests();
int FuncDoodle_RunImageArrayTests();
int FuncDoodle_RunFrameTests();
int FuncDoodle_RunUUIDTests();
int FuncDoodle_RunLongIndexArrayTests();
int FuncDoodle_RunExePathTests();
int FuncDoodle_RunActionTests();

int FuncDoodle_RunTests() {
	FuncDoodle_RunColTests();
	FuncDoodle_RunImageArrayTests();
	FuncDoodle_RunFrameTests();
	FuncDoodle_RunActionTests();
	FuncDoodle_RunUUIDTests();
	FuncDoodle_RunLongIndexArrayTests();
	FuncDoodle_RunExePathTests();

	TEST_SCOPE("Original Tests");

	FuncDoodle::LongIndexArray lia =
		FuncDoodle::LongIndexArray(32, 32, FuncDoodle::Col{255, 255, 255});

	CHECK((lia.Size() == 0), "initialized lia is not empty");

	FuncDoodle::Frame frame1(32, 32, {255, 0, 0});
	lia.PushBack(&frame1);
	CHECK((lia.Size() == 1), "lia size should be 1 after PushBack");
	CHECK(((*lia[0]) == frame1), "lia[0] should be frame1");

	lia.PushBackEmpty();
	CHECK((lia.Size() == 2), "lia size should be 2 after PushBackEmpty");

	FuncDoodle::Frame frame2(32, 32, {0, 255, 0});
	lia.InsertAfter(0, &frame2);
	CHECK((lia.Size() == 3), "lia size should be 3 after InsertAfter");
	CHECK(((*lia[1]) == frame2), "lia[1] should be frame2");
	CHECK((lia[2]->Pixels()->BgCol() == FuncDoodle::Col({255, 255, 255})),
		"lia[2] should be empty frame");

	FuncDoodle::Frame frame3(32, 32, {0, 0, 255});
	lia.InsertBefore(1, &frame3);
	CHECK((lia.Size() == 4), "lia size should be 4 after InsertBefore");
	CHECK(((*lia[1]) == frame3), "lia[1] should be frame3");
	CHECK(((*lia[2]) == frame2), "lia[2] should be frame2");

	lia.Remove(1);
	CHECK((lia.Size() == 3), "lia size should be 3 after Remove");
	CHECK(((*lia[1]) == frame2), "lia[1] should be frame2 after Remove");

	lia.MoveForward(0);
	CHECK(((*lia[0]) == frame2), "lia[0] should be frame2 after MoveForward");
	CHECK(((*lia[1]) == frame1), "lia[1] should be frame1 after MoveForward");

	lia.MoveBackward(1);
	CHECK(((*lia[0]) == frame1), "lia[0] should be frame1 after MoveBackward");
	CHECK(((*lia[1]) == frame2), "lia[1] should be frame2 after MoveBackward");

	FuncDoodle::Frame* frame_get = lia.Get(1);
	CHECK(((*frame_get) == frame2), "Get(1) should return frame2");

	FuncDoodle::LongIndexArray lia2 =
		FuncDoodle::LongIndexArray(1, 1, {0, 0, 0}, 2);
	CHECK((lia2.Capacity() == 2), "initial capacity should be 2");
	FuncDoodle::Frame f1(1, 1, {0, 0, 0});
	lia2.PushBack(&f1);
	lia2.PushBack(&f1);
	lia2.PushBack(&f1);
	CHECK((lia2.Capacity() > 2),
		"capacity should be greater than 2 after resize");
	CHECK((lia2.Size() == 3), "size should be 3 after 3 pushbacks");

	return 0;
}
