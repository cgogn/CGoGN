#include <iostream>


// no header files test function names from cpp files
extern int test_attributeContainer();
extern int test_attributeMultiVector();
extern int test_containerBrowser();


int main()
{
	test_attributeContainer();
	test_attributeMultiVector();
	test_containerBrowser();

	return 0;
}
