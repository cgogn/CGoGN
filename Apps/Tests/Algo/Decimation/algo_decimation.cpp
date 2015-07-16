#include <iostream>

extern int test_geometryApproximator();
extern int test_colorPerVertexApproximator();

extern int test_predictor();
extern int test_geometryPredictor();

extern int test_edgeSelector();
extern int test_halfEdgeSelector();
extern int test_decimation();

int main()
{
	test_geometryApproximator();
	test_colorPerVertexApproximator();

	test_predictor();
	test_geometryPredictor();

	test_edgeSelector();
	test_halfEdgeSelector();
	test_decimation();

	return 0;
}
