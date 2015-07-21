#include <iostream>


// no header files test function names from cpp files
//extern int test_colorMaps();
extern int test_colourConverter();
extern int test_qem();
extern int test_quadricRGBfunctions();
extern int test_quantization();
//extern int test_shared_mem();
extern int test_sphericalHarmonics();
extern int test_texture();


int main()
{
	//test_colorMaps();
	test_colourConverter();
	test_qem();
	test_quadricRGBfunctions();
	test_quantization();
//	test_shared_mem();
	test_sphericalHarmonics();
	test_texture();
	return 0;
}
