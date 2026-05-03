#include "Project/DynArr.h"
#include "Project/Frame.h"
#include "Test/Test.h"

int FuncDoodle_RunLongIndexArrayTests() {
	TEST_SCOPE("LongIndexArray Tests");

	FuncDoodle::LongIndexArray lia(10, 10, FuncDoodle::Col{255, 255, 255});

	CHECK((lia.Size() == 0), "New array should have size 0");
	CHECK((lia.Capacity() >= 10), "Initial capacity should be at least 10");

	FuncDoodle::Frame frame1(10, 10, FuncDoodle::Col{255, 0, 0});
	lia.PushBack(&frame1);
	CHECK((lia.Size() == 1), "Size should be 1 after one PushBack");
	CHECK((lia[0] != nullptr), "lia[0] should not be null");
	CHECK(((*lia[0]) == frame1), "lia[0] should contain pushed frame");

	FuncDoodle::Frame frame2(10, 10, FuncDoodle::Col{0, 255, 0});
	lia.PushBack(&frame2);
	CHECK((lia.Size() == 2), "Size should be 2 after two PushBacks");
	CHECK((lia[1] != nullptr), "lia[1] should not be null");

	FuncDoodle::Frame frame3(10, 10, FuncDoodle::Col{0, 0, 255});
	lia.InsertAfter(0, &frame3);
	CHECK((lia.Size() == 3), "Size should be 3 after InsertAfter");
	CHECK((lia[1] != nullptr && (*lia[1]) == frame3),
		"frame3 should be at index 1 after InsertAfter(0, frame3)");
	CHECK((lia[2] != nullptr && (*lia[2]) == frame2),
		"frame2 should be at index 2 after InsertAfter");

	FuncDoodle::LongIndexArray lia2(5, 5, FuncDoodle::Col{128, 128, 128});
	lia2.PushBackEmpty();
	CHECK((lia2.Size() == 1), "PushBackEmpty should increase size");
	CHECK((lia2[0] != nullptr), "PushBackEmpty should create a frame");
	CHECK((lia2[0]->Pixels()->BgCol() == FuncDoodle::Col{128, 128, 128}),
		"PushBackEmpty should use default bgCol");

	lia2.InsertAfterEmpty(0);
	CHECK((lia2.Size() == 2), "InsertAfterEmpty should increase size");

	lia2.InsertBeforeEmpty(0);
	CHECK((lia2.Size() == 3), "InsertBeforeEmpty should increase size");

	FuncDoodle::LongIndexArray lia3(3, 3, FuncDoodle::Col{0, 0, 0});
	FuncDoodle::Frame f1(3, 3, FuncDoodle::Col{10, 10, 10});
	FuncDoodle::Frame f2(3, 3, FuncDoodle::Col{20, 20, 20});
	FuncDoodle::Frame f3(3, 3, FuncDoodle::Col{30, 30, 30});
	FuncDoodle::Frame f4(3, 3, FuncDoodle::Col{40, 40, 40});

	lia3.PushBack(&f1);
	lia3.PushBack(&f4);
	CHECK((lia3.Size() == 2), "Size should be 2");

	lia3.InsertBefore(1, &f2);
	CHECK((lia3.Size() == 3), "Size should be 3 after InsertBefore");
	CHECK((lia3[1] != nullptr && (*lia3[1]) == f2),
		"f2 should be at index 1 after InsertBefore");

	lia3.InsertBefore(1, &f3);
	CHECK((lia3.Size() == 4), "Size should be 4");
	CHECK((lia3[1] != nullptr && (*lia3[1]) == f3), "f3 should be at index 1");

	FuncDoodle::LongIndexArray lia4(2, 2, FuncDoodle::Col{100, 100, 100});
	FuncDoodle::Frame a(2, 2, FuncDoodle::Col{1, 1, 1});
	FuncDoodle::Frame b(2, 2, FuncDoodle::Col{2, 2, 2});
	FuncDoodle::Frame c(2, 2, FuncDoodle::Col{3, 3, 3});

	lia4.PushBack(&a);
	lia4.PushBack(&b);
	lia4.PushBack(&c);

	lia4.Remove(1);
	CHECK((lia4.Size() == 2), "Size should be 2 after Remove(1)");
	CHECK((lia4[0] != nullptr && (*lia4[0]) == a), "a should be at index 0");
	CHECK((lia4[1] != nullptr && (*lia4[1]) == c),
		"c should be at index 1 after Remove");

	FuncDoodle::LongIndexArray lia5(2, 2, FuncDoodle::Col{50, 50, 50});
	FuncDoodle::Frame x(2, 2, FuncDoodle::Col{5, 5, 5});
	FuncDoodle::Frame y(2, 2, FuncDoodle::Col{6, 6, 6});

	lia5.PushBack(&x);
	lia5.PushBack(&y);

	lia5.MoveForward(0);
	CHECK((lia5[0] != nullptr && (*lia5[0]) == y),
		"After MoveForward(0), y should be at index 0");
	CHECK((lia5[1] != nullptr && (*lia5[1]) == x),
		"After MoveForward(0), x should be at index 1");

	lia5.MoveBackward(1);
	CHECK((lia5[0] != nullptr && (*lia5[0]) == x),
		"After MoveBackward(1), x should be at index 0");
	CHECK((lia5[1] != nullptr && (*lia5[1]) == y),
		"After MoveBackward(1), y should be at index 1");

	FuncDoodle::LongIndexArray lia6(1, 1, FuncDoodle::Col{0, 0, 0}, 1);
	CHECK((lia6.Capacity() == 1), "Capacity should be 1");

	FuncDoodle::Frame big_frame(1, 1, FuncDoodle::Col{1, 1, 1});
	for (int i = 0; i < 10; i++) {
		lia6.PushBack(&big_frame);
	}
	CHECK((lia6.Size() == 10), "Size should be 10 after 10 PushBacks");
	CHECK(
		(lia6.Capacity() > 1), "Capacity should have increased after resizing");

	FuncDoodle::LongIndexArray lia7(4, 4, FuncDoodle::Col{200, 200, 200});
	lia7.PushBackEmpty();
	lia7.PushBackEmpty();
	lia7.PushBackEmpty();

	lia7.InsertAfter(0, FuncDoodle::Frame(4, 4, FuncDoodle::Col{1, 2, 3}));
	CHECK(
		(lia7.Size() == 4), "InsertAfter with value (not pointer) should work");

	lia7.InsertBefore(2, FuncDoodle::Frame(4, 4, FuncDoodle::Col{4, 5, 6}));
	CHECK((lia7.Size() == 5),
		"InsertBefore with value (not pointer) should work");

	FuncDoodle::Frame* get_result = lia7.Get(2);
	CHECK(
		(get_result != nullptr), "Get should return non-null for valid index");

	FuncDoodle::LongIndexArray lia8(3, 3, FuncDoodle::Col{255, 255, 255});
	FuncDoodle::Frame f1_8(3, 3, FuncDoodle::Col{10, 10, 10});
	FuncDoodle::Frame f2_8(3, 3, FuncDoodle::Col{20, 20, 20});
	lia8.PushBack(&f1_8);
	lia8.PushBack(&f2_8);

	lia8.MoveForward(1);
	CHECK(
		(lia8.Size() == 2), "MoveForward at last index should not change size");

	lia8.MoveBackward(0);
	CHECK((lia8.Size() == 2),
		"MoveBackward at first index should not change size");

	return 0;
}
