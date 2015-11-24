#include <iostream>

#include <Geometry/distances.h>

using namespace CGoGN;

/*****************************************
*		DISTANCES INSTANTIATION
*****************************************/

template float Geom::squaredDistancePoint2TrianglePlane(const Geom::Vec3f& P, const Geom::Vec3f& A, const Geom::Vec3f& B, const Geom::Vec3f& C);
template float Geom::distancePoint2TrianglePlane(const Geom::Vec3f& P, const Geom::Vec3f& A, const Geom::Vec3f& B, const Geom::Vec3f& C);
template float Geom::squaredDistancePoint2Triangle(const Geom::Vec3f& P, const Geom::Vec3f& A, const Geom::Vec3f& B, const Geom::Vec3f& C);
template float Geom::squaredDistanceLine2Point(const Geom::Vec3f& A, const Geom::Vec3f& AB, float AB1, const Geom::Vec3f& P);
template float Geom::squaredDistanceLine2Point(const Geom::Vec3f& A, const Geom::Vec3f& B, const Geom::Vec3f& P);
template float Geom::squaredDistanceLine2Line(const Geom::Vec3f& A, const Geom::Vec3f& AB, float AB2, const Geom::Vec3f& P, const Geom::Vec3f& Q);
template float Geom::squaredDistanceLine2Seg(const Geom::Vec3f& A, const Geom::Vec3f& AB, float AB2, const Geom::Vec3f& P, const Geom::Vec3f& Q);
template float Geom::squaredDistanceSeg2Point(const Geom::Vec3f& A, const Geom::Vec3f& AB, float AB2, const Geom::Vec3f& P);
template bool Geom::lineLineClosestPoints(const Geom::Vec3f& P1, const Geom::Vec3f& V1, const Geom::Vec3f& P2, const Geom::Vec3f& V2, Geom::Vec3f& Q1, Geom::Vec3f& Q2);


template double Geom::squaredDistancePoint2TrianglePlane(const Geom::Vec3d& P, const Geom::Vec3d& A, const Geom::Vec3d& B, const Geom::Vec3d& C);
template double Geom::distancePoint2TrianglePlane(const Geom::Vec3d& P, const Geom::Vec3d& A, const Geom::Vec3d& B, const Geom::Vec3d& C);
template double Geom::squaredDistancePoint2Triangle(const Geom::Vec3d& P, const Geom::Vec3d& A, const Geom::Vec3d& B, const Geom::Vec3d& C);
template double Geom::squaredDistanceLine2Point(const Geom::Vec3d& A, const Geom::Vec3d& AB, double AB1, const Geom::Vec3d& P);
template double Geom::squaredDistanceLine2Point(const Geom::Vec3d& A, const Geom::Vec3d& B, const Geom::Vec3d& P);
template double Geom::squaredDistanceLine2Line(const Geom::Vec3d& A, const Geom::Vec3d& AB, double AB2, const Geom::Vec3d& P, const Geom::Vec3d& Q);
template double Geom::squaredDistanceLine2Seg(const Geom::Vec3d& A, const Geom::Vec3d& AB, double AB2, const Geom::Vec3d& P, const Geom::Vec3d& Q);
template double Geom::squaredDistanceSeg2Point(const Geom::Vec3d& A, const Geom::Vec3d& AB, double AB2, const Geom::Vec3d& P);
template bool Geom::lineLineClosestPoints(const Geom::Vec3d& P1, const Geom::Vec3d& V1, const Geom::Vec3d& P2, const Geom::Vec3d& V2, Geom::Vec3d& Q1, Geom::Vec3d& Q2);


int test_distances()
{

	return 0;
}
