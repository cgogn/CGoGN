#include <iostream>
#include <Geometry/transfo.h>


using namespace CGoGN;


/*****************************************
*		TRNASFO INSTANTIATION
*****************************************/

template void Geom::scale(float sx, float sy, float sz, Geom::Matrix<4, 4, float>& mat);
template void Geom::translate(float tx, float ty, float tz, Matrix<4, 4, float>& mat);
template void Geom::rotateZ(float angle, Matrix<4, 4, float>& mat);
template void Geom::rotateY(float angle, Matrix<4, 4, float>& mat);
template void Geom::rotateX(float angle, Matrix<4, 4, float>& mat);
template void Geom::rotate(float axis_x, float axis_y, float axis_z, float angle, Matrix<4, 4, float>& mat);
template void Geom::rotate(Geom::Vector<3, float>& axis, float angle, Matrix<4, 4, float>& mat);
template Geom::Vector<3, float> Geom::transform(const Vector<3, float>& P, const Matrix<4, 4, float>& mat);

template void Geom::scale(double sx, double sy, double sz, Geom::Matrix<4, 4, double>& mat);
template void Geom::translate(double tx, double ty, double tz, Matrix<4, 4, double>& mat);
template void Geom::rotateZ(double angle, Matrix<4, 4, double>& mat);
template void Geom::rotateY(double angle, Matrix<4, 4, double>& mat);
template void Geom::rotateX(double angle, Matrix<4, 4, double>& mat);
template void Geom::rotate(double axis_x, double axis_y, double axis_z, double angle, Matrix<4, 4, double>& mat);
template void Geom::rotate(Geom::Vector<3, double>& axis, double angle, Matrix<4, 4, double>& mat);
template Geom::Vector<3, double> Geom::transform(const Vector<3, double>& P, const Matrix<4, 4, double>& mat);



int test_transfo()
{
	return 0;
}
