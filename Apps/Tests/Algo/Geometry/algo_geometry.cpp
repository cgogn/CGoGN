#include <iostream>

extern int test_area();
extern int test_centroid();
extern int test_boundingbox();
extern int test_basic();
extern int test_convexity();
extern int test_curvature();
extern int test_distances();


int main()
{
	test_area();
	test_centroid();
	test_boundingbox();
	test_basic();
	test_convexity();
	test_curvature();
	test_distances();

	return 0;
}
