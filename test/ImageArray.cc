#include "Project/Frame.h"
#include "Test/Test.h"

int FuncDoodle_RunImageArrayTests() {
	TEST_SCOPE("ImageArray Tests");

	FuncDoodle::Col bg_col{.r=255, .g=255, .b=255};
	FuncDoodle::ImageArray arr(10, 20, bg_col);

	CHECK((arr.Width() == 10), "width should be 10");
	CHECK((arr.Height() == 20), "height should be 20");
	CHECK((arr.BgCol() == bg_col), "bgCol should match constructor");

	FuncDoodle::Col red{.r=255, .g=0, .b=0};
	arr.Set(0, 0, red);
	CHECK((arr.Get(0, 0) == red), "Set and Get should work for pixel");

	FuncDoodle::Col green{.r=0, .g=255, .b=0};
	arr.Set(9, 19, green);
	CHECK((arr.Get(9, 19) == green), "Set and Get at edge should work");

	arr.SetBG(FuncDoodle::Col{.r=0, .g=0, .b=0});
	CHECK((arr.BgCol() == (FuncDoodle::Col{0, 0, 0})),
		"SetBG should update background");

	const std::vector<FuncDoodle::Col>& data = arr.Data();
	CHECK((data.size() == 200), "Data size should be width * height (10 * 20)");

	FuncDoodle::ImageArray arr2(5, 5, FuncDoodle::Col{.r=128, .g=128, .b=128});
	CHECK((arr2.Width() == 5), "width should be 5");
	CHECK((arr2.Height() == 5), "height should be 5");
	CHECK((arr2.Data().size() == 25), "Data size should be 25 for 5x5");

	arr2.SetWidth(10);
	CHECK((arr2.Width() == 10), "SetWidth should update width");

	arr2.SetHeight(15);
	CHECK((arr2.Height() == 15), "SetHeight should update height");

	FuncDoodle::Col blue{.r=0, .g=0, .b=255};
	FuncDoodle::ImageArray arr3(3, 3, blue);
	arr3.SetData(std::vector<FuncDoodle::Col>(9, FuncDoodle::Col{.r=255, .g=255, .b=0}));
	CHECK((arr3.Data().size() == 9), "SetData should update data size");
	CHECK((arr3.Data()[0] == FuncDoodle::Col{255, 255, 0}),
		"SetData should update all pixels");

	FuncDoodle::ImageArray arr_small(1, 1, FuncDoodle::Col{.r=100, .g=100, .b=100});
	CHECK((arr_small.Width() == 1), "Min width 1 should work");
	CHECK((arr_small.Height() == 1), "Min height 1 should work");
	arr_small.Set(0, 0, FuncDoodle::Col{.r=50, .g=50, .b=50});
	CHECK((arr_small.Get(0, 0) == FuncDoodle::Col{50, 50, 50}),
		"Single pixel Get/Set should work");

	FuncDoodle::ImageArray copy = arr;
	CHECK((copy.Width() == arr.Width()),
		"Copy constructor should preserve width");
	CHECK((copy.Height() == arr.Height()),
		"Copy constructor should preserve height");

	FuncDoodle::ImageArray move = std::move(arr);
	CHECK((move.Width() == 10), "Move constructor should preserve width");
	CHECK((move.Height() == 20), "Move constructor should preserve height");

	return 0;
}
