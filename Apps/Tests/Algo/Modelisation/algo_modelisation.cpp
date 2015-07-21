#include <iostream>

extern int test_extrusion();
extern int test_planeCutting();
extern int test_polyhedron();
extern int test_subdivision();
extern int test_subdivision3();
extern int test_tetrahedralization();
extern int test_triangulation();
extern int test_voxellisation();

int main()
{
	test_extrusion();
	test_planeCutting();
	test_polyhedron();
	test_subdivision();
	test_subdivision3();
	test_tetrahedralization();
	test_triangulation();
	test_voxellisation();

	return 0;
}
