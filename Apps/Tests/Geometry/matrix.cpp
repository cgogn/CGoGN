#include <iostream>

#include <Geometry/matrix.h>

using namespace CGoGN;

/*****************************************
*		MATRIX INSTANTIATION
*****************************************/

template class Geom::Matrix<2, 2, float>;
template class Geom::Matrix<3, 3, float>;
template class Geom::Matrix<4, 4, float>;
template class Geom::Matrix<7, 7, float>;

template class Geom::Matrix<2, 2, double>;
template class Geom::Matrix<3, 3, double>;
template class Geom::Matrix<4, 4, double>;
template class Geom::Matrix<7, 7, double>;


int test_matrix()
{
	// test matrixx MN
	Geom::Matrix<3, 2, float> Af;
	Geom::Matrix<2, 3, float> Bf;
	Geom::Matrix<3, 3, float> Cf = Af*Bf;
	Geom::Matrix<2, 2, float> Df = Bf*Af;

	std::cout << Cf << std::endl;
	std::cout << Df << std::endl;

	Geom::Matrix<3, 2, double> Ad;
	Geom::Matrix<2, 3, double> Bd;
	Geom::Matrix<3, 3, double> Cd = Ad*Bd;
	Geom::Matrix<2, 2, double> Dd = Bd*Ad;

	std::cout << Cd << std::endl;
	std::cout << Dd << std::endl;

	return 0;
}
