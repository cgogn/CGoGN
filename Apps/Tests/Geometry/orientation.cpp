#include <iostream>
#include <Geometry/orientation.h>

using namespace CGoGN;

/*****************************************
*		ORIENTATION INSTANTIATION
*****************************************/
template Geom::OrientationLine Geom::testOrientationLines(const Geom::Vec3f& a, const Geom::Vec3f& b, const Geom::Vec3f& c, const Geom::Vec3f& d);
template Geom::Orientation3D Geom::testOrientation3D(const Geom::Vec3f& P, const Geom::Vec3f& A, const Geom::Vec3f& B, const Geom::Vec3f& C);
template Geom::Orientation3D Geom::testOrientation3D(const Geom::Vec3f& P, const Geom::Vec3f& N, const Geom::Vec3f& PP);
template Geom::Orientation2D Geom::testOrientation2D(const Geom::Vec3f& P, const Geom::Vec3f& Pa, const Geom::Vec3f& Pb);
template int Geom::orientation2D(const Geom::Vec3f& u, const Geom::Vec3f& v);
template int Geom::aligned2D(const Geom::Vec3f& u, const Geom::Vec3f& v);
template bool Geom::isBetween(const Geom::Vec3f& u, const Geom::Vec3f& v, const Geom::Vec3f& w);
template bool Geom::isTetrahedronWellOriented(const Geom::Vec3f points[4], bool CCW);


template Geom::OrientationLine Geom::testOrientationLines(const Geom::Vec3d& a, const Geom::Vec3d& b, const Geom::Vec3d& c, const Geom::Vec3d& d);
template Geom::Orientation3D Geom::testOrientation3D(const Geom::Vec3d& P, const Geom::Vec3d& A, const Geom::Vec3d& B, const Geom::Vec3d& C);
template Geom::Orientation3D Geom::testOrientation3D(const Geom::Vec3d& P, const Geom::Vec3d& N, const Geom::Vec3d& PP);
template Geom::Orientation2D Geom::testOrientation2D(const Geom::Vec3d& P, const Geom::Vec3d& Pa, const Geom::Vec3d& Pb);
template int Geom::orientation2D(const Geom::Vec3d& u, const Geom::Vec3d& v);
template int Geom::aligned2D(const Geom::Vec3d& u, const Geom::Vec3d& v);
template bool Geom::isBetween(const Geom::Vec3d& u, const Geom::Vec3d& v, const Geom::Vec3d& w);
template bool Geom::isTetrahedronWellOriented(const Geom::Vec3d points[4], bool CCW);


int test_orientation()
{

	return 0;
}
