#include <iostream>

extern int test_particle_cell_2D();
extern int test_particle_cell_2D_memo();
extern int test_particle_cell_2D_secured();
extern int test_particle_cell_2DandHalf();
extern int test_particle_cell_2DandHalf_memo();
extern int test_particle_cell_3D();

int main()
{
	test_particle_cell_2D();
	test_particle_cell_2D_memo();
	test_particle_cell_2D_secured();
	test_particle_cell_2DandHalf();
	test_particle_cell_2DandHalf_memo();
	test_particle_cell_3D();

	return 0;
}
