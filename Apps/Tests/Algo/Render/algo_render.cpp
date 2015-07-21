#include <iostream>

extern int test_colorPerEdgeRender();
extern int test_colorPerFaceRender();
extern int test_dataPerFaceRender();
extern int test_drawerCells();
extern int test_explodeVolumeRender();
extern int test_mapRender();
extern int test_topoRender();
extern int test_topo3Render();
extern int test_mapSVGRender();
extern int test_topoPrimalRender();
extern int test_topo3PrimalRender();

int main()
{
	test_colorPerEdgeRender();
	test_dataPerFaceRender();
	test_drawerCells();
	test_explodeVolumeRender();
	test_mapRender();
	test_topoRender();
	test_topo3Render();
	test_mapSVGRender();
	test_topoPrimalRender();
	test_topo3PrimalRender();

	return 0;
}
