#include <iostream>


// no header files test function names from cpp files
extern int test_basic();
extern int test_bounding_box();
extern int test_intersection();
extern int test_orientation();
extern int test_inclusion();
extern int test_matrix();
extern int test_tensor();
extern int test_transfo();
extern int test_plane3d();
extern int test_frame();
extern int test_distances();



int main()
{
	test_basic();
	test_bounding_box();
	test_intersection();
	test_orientation();
	test_inclusion();
	test_matrix();
	test_tensor();
	test_transfo();
	test_plane3d();
	test_frame();
	test_distances();

	return 0;
}
