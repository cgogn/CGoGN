#include <iostream>
#include <Geometry/frame.h>

using namespace CGoGN;

/*****************************************
*		FRAME INSTANTIATION
*****************************************/

template Geom::Vector<3, float> Geom::rotate(Geom::Vector<3, float> axis, float angle, Geom::Vector<3, float> p);
template Geom::Vector<3, float> Geom::sphericalToCart(const Geom::Vector<3, float>& sph);
template Geom::Vector<3, float> Geom::cartToSpherical(const Geom::Vector<3, float>& cart);
template bool Geom::isDirectOrthoNormalFrame(const Geom::Vec3f& X, const Geom::Vec3f& Y, const Geom::Vec3f& Z, float epsilon);
template bool Geom::isDirectFrame(const Geom::Vec3f& X, const Geom::Vec3f& Y, const Geom::Vec3f& Z, float epsilon);
template bool Geom::isOrthogonalFrame(const Geom::Vec3f& X, const Geom::Vec3f& Y, const Geom::Vec3f& Z, float epsilon);
template bool Geom::isNormalizedFrame(const Geom::Vec3f& X, const Geom::Vec3f& Y, const Geom::Vec3f& Z, float epsilon);

template Geom::Vector<3, double> Geom::rotate(Geom::Vector<3, double> axis, double angle, Geom::Vector<3, double> p);
template Geom::Vector<3, double> Geom::sphericalToCart(const Geom::Vector<3, double>& sph);
template Geom::Vector<3, double> Geom::cartToSpherical(const Geom::Vector<3, double>& cart);
template bool Geom::isDirectOrthoNormalFrame(const Geom::Vec3d& X, const Geom::Vec3d& Y, const Geom::Vec3d& Z, double epsilon);
template bool Geom::isDirectFrame(const Geom::Vec3d& X, const Geom::Vec3d& Y, const Geom::Vec3d& Z, double epsilon);
template bool Geom::isOrthogonalFrame(const Geom::Vec3d& X, const Geom::Vec3d& Y, const Geom::Vec3d& Z, double epsilon);
template bool Geom::isNormalizedFrame(const Geom::Vec3d& X, const Geom::Vec3d& Y, const Geom::Vec3d& Z, double epsilon);

template class Geom::Frame<float>;
template class Geom::Frame<double>;


int test_frame()
{
	return 0;
}
