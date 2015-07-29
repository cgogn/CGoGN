#include <iostream>

extern int test_square();
extern int test_triangular();
extern int test_hexagonal();
extern int test_cubic();

int main()
{
	test_square();
	test_triangular();
	test_hexagonal();
	test_cubic();


	return 0;
}
