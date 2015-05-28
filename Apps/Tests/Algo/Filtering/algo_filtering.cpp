#include <iostream>

extern int test_average();
extern int test_average_normals();
extern int test_bilateral();
extern int test_taubin();
extern int test_tools();

int main()
{
	test_average();
	test_average_normals();
	test_bilateral();
	test_taubin();
	test_tools();

	return 0;
}
