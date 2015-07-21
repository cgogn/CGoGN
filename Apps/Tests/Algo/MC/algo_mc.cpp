#include <iostream>

extern int test_image();
extern int test_planeCutting();
extern int test_marchingcube();
extern int test_marchingcubeGen();
extern int test_windowing();

int main()
{
	test_image();
	test_marchingcube();
	test_marchingcubeGen();
	test_windowing();

	return 0;
}
