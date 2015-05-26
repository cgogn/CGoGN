#include <iostream>

extern int test_basic();
extern int test_embedding();
extern int test_simplex();
extern int test_uniformOrientation();

int main()
{
	test_basic();
	test_embedding();
	test_simplex();
	test_uniformOrientation();


	return 0;
}
