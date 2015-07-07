#include <iostream>
#include <Geometry/inclusion.h>

using namespace CGoGN;

/*****************************************
*		INCLUSION INSTANTIATION
*****************************************/

template Geom::Inclusion Geom::isPointInTriangle(const Geom::Vec3f& point, const Geom::Vec3f& Ta, const Geom::Vec3f& Tb, const Geom::Vec3f& Tc);
template bool Geom::isPointInSphere(const Geom::Vec3f& point, const Geom::Vec3f& center, const float& radius);
template Geom::Inclusion Geom::isSegmentInTriangle2D(const Geom::Vec3f& P1, const Geom::Vec3f& P2, const Geom::Vec3f& Ta, const Geom::Vec3f& Tb, const Geom::Vec3f& Tc, const Geom::Vec3f& N);
template bool Geom::isPointInTetrahedron(Geom::Vec3f points[4], Geom::Vec3f& point, bool);
template bool Geom::isEdgeInOrIntersectingTetrahedron(Geom::Vec3f points[4], Geom::Vec3f& point1, Geom::Vec3f& point2, bool CCW);
template bool Geom::arePointsEquals(const Geom::Vec3f& point1, const Geom::Vec3f& point2);

template Geom::Inclusion Geom::isPointInTriangle(const Geom::Vec3d& point, const Geom::Vec3d& Ta, const Geom::Vec3d& Tb, const Geom::Vec3d& Tc);
template bool Geom::isPointInSphere(const Geom::Vec3d& point, const Geom::Vec3d& center, const double& radius);
template Geom::Inclusion Geom::isSegmentInTriangle2D(const Geom::Vec3d& P1, const Geom::Vec3d& P2, const Geom::Vec3d& Ta, const Geom::Vec3d& Tb, const Geom::Vec3d& Tc, const Geom::Vec3d& N);
template bool Geom::isPointInTetrahedron(Geom::Vec3d points[4], Geom::Vec3d& point, bool);
template bool Geom::isEdgeInOrIntersectingTetrahedron(Geom::Vec3d points[4], Geom::Vec3d& point1, Geom::Vec3d& point2, bool CCW);
template bool Geom::arePointsEquals(const Geom::Vec3d& point1, const Geom::Vec3d& point2);

int test_inclusion()
{
	return 0;
}
