#include <iostream>

extern int test_export();
extern int test_exportPov();
extern int test_exportVTU();
extern int test_exportVol();

int main()
{
	test_export();
	test_exportPov();
	test_exportVTU();
	test_exportVol();

	return 0;
}
