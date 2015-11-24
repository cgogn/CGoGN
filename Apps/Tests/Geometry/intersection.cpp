#include <iostream>

#include <Geometry/intersection.h>

using namespace CGoGN;

/*****************************************
*		INTERSECTION INSTANTIATION
*****************************************/

template Geom::Intersection Geom::intersectionLinePlane(const Geom::Vec3f& P, const Geom::Vec3f& Dir, const Geom::Vec3f& PlaneP, const Geom::Vec3f& NormP, Geom::Vec3f& Inter);
template Geom::Intersection Geom::intersectionLinePlane(const Geom::Vec3f& P, const Geom::Vec3f& Dir, const  Geom::Plane3D<float>& Plane, Geom::Vec3f& Inter);
template Geom::Intersection Geom::intersectionRayTriangle(const Geom::Vec3f& P, const Geom::Vec3f& Dir, const Geom::Vec3f& Ta, const Geom::Vec3f& Tb, const Geom::Vec3f& Tc, Geom::Vec3f& Inter);
template Geom::Intersection Geom::intersectionRayTriangleOpt(const Geom::Vec3f& P, const Geom::Vec3f& Dir, const Geom::Vec3f& Ta, const Geom::Vec3f& Tb, const Geom::Vec3f& Tc, Geom::Vec3f& Inter);
template Geom::Intersection Geom::intersectionRayTriangleOpt(const Geom::Vec3f& P, const Geom::Vec3f& Dir, const Geom::Vec3f& Ta, const Geom::Vec3f& Tb, const Geom::Vec3f& Tc);
template Geom::Intersection Geom::intersectionLineTriangle(const Geom::Vec3f& P, const Geom::Vec3f& Dir, const Geom::Vec3f& Ta, const Geom::Vec3f& Tb, const Geom::Vec3f& Tc, Geom::Vec3f& Inter);
template Geom::Intersection Geom::intersectionLineTriangle2D(const Geom::Vec3f& P, const Geom::Vec3f& Dir, const Geom::Vec3f& Ta, const Geom::Vec3f& Tb, const Geom::Vec3f& Tc, Geom::Vec3f& Inter);
template Geom::Intersection Geom::intersectionSegmentTriangle(const Geom::Vec3f& PA, const Geom::Vec3f& PB, const Geom::Vec3f& Ta, const Geom::Vec3f& Tb, const Geom::Vec3f& Tc, Geom::Vec3f& Inter);
template Geom::Intersection Geom::intersectionPlaneRay(const Geom::Plane3D<float>& pl, const Geom::Vec3f& p1, const Geom::Vec3f& dir, Geom::Vec3f& Inter);
template Geom::Intersection Geom::intersection2DSegmentSegment(const Geom::Vec3f& PA, const Geom::Vec3f& PB, const Geom::Vec3f& QA, const Geom::Vec3f& QB, Geom::Vec3f& Inter);
template Geom::Intersection Geom::intersectionSegmentPlan(const Geom::Vec3f& PA, const Geom::Vec3f& PB, const Geom::Vec3f& PlaneP, const Geom::Vec3f& NormP);
template Geom::Intersection Geom::intersectionSegmentPlan(const Geom::Vec3f& PA, const Geom::Vec3f& PB, const Geom::Vec3f& PlaneP, const Geom::Vec3f& NormP, Geom::Vec3f& Inter);
template Geom::Intersection Geom::intersectionTrianglePlan(const Geom::Vec3f& Ta, const Geom::Vec3f& Tb, const Geom::Vec3f& Tc, const Geom::Vec3f& PlaneP, const Geom::Vec3f& NormP);
template Geom::Intersection Geom::intersectionSegmentHalfPlan(const Geom::Vec3f& PA, const Geom::Vec3f& PB, const Geom::Vec3f& P, const Geom::Vec3f& DirP, const Geom::Vec3f& OrientP);
template Geom::Intersection Geom::intersectionTriangleHalfPlan(const Geom::Vec3f& Ta, const Geom::Vec3f& Tb, const Geom::Vec3f& Tc, const Geom::Vec3f& P, const Geom::Vec3f& DirP, const Geom::Vec3f& OrientP);
template bool Geom::interLineSeg(const Geom::Vec3f& A, const Geom::Vec3f& AB, float AB2, const Geom::Vec3f& P, const Geom::Vec3f& Q, Geom::Vec3f& inter);

template Geom::Intersection Geom::intersectionLinePlane(const Geom::Vec3d& P, const Geom::Vec3d& Dir, const Geom::Vec3d& PlaneP, const Geom::Vec3d& NormP, Geom::Vec3d& Inter);
template Geom::Intersection Geom::intersectionLinePlane(const Geom::Vec3d& P, const Geom::Vec3d& Dir, const  Geom::Plane3D<double>& Plane, Geom::Vec3d& Inter);
template Geom::Intersection Geom::intersectionRayTriangle(const Geom::Vec3d& P, const Geom::Vec3d& Dir, const Geom::Vec3d& Ta, const Geom::Vec3d& Tb, const Geom::Vec3d& Tc, Geom::Vec3d& Inter);
template Geom::Intersection Geom::intersectionRayTriangleOpt(const Geom::Vec3d& P, const Geom::Vec3d& Dir, const Geom::Vec3d& Ta, const Geom::Vec3d& Tb, const Geom::Vec3d& Tc, Geom::Vec3d& Inter);
template Geom::Intersection Geom::intersectionRayTriangleOpt(const Geom::Vec3d& P, const Geom::Vec3d& Dir, const Geom::Vec3d& Ta, const Geom::Vec3d& Tb, const Geom::Vec3d& Tc);
template Geom::Intersection Geom::intersectionLineTriangle(const Geom::Vec3d& P, const Geom::Vec3d& Dir, const Geom::Vec3d& Ta, const Geom::Vec3d& Tb, const Geom::Vec3d& Tc, Geom::Vec3d& Inter);
template Geom::Intersection Geom::intersectionLineTriangle2D(const Geom::Vec3d& P, const Geom::Vec3d& Dir, const Geom::Vec3d& Ta, const Geom::Vec3d& Tb, const Geom::Vec3d& Tc, Geom::Vec3d& Inter);
template Geom::Intersection Geom::intersectionSegmentTriangle(const Geom::Vec3d& PA, const Geom::Vec3d& PB, const Geom::Vec3d& Ta, const Geom::Vec3d& Tb, const Geom::Vec3d& Tc, Geom::Vec3d& Inter);
template Geom::Intersection Geom::intersectionPlaneRay(const Geom::Plane3D<double>& pl, const Geom::Vec3d& p1, const Geom::Vec3d& dir, Geom::Vec3d& Inter);
template Geom::Intersection Geom::intersection2DSegmentSegment(const Geom::Vec3d& PA, const Geom::Vec3d& PB, const Geom::Vec3d& QA, const Geom::Vec3d& QB, Geom::Vec3d& Inter);
template Geom::Intersection Geom::intersectionSegmentPlan(const Geom::Vec3d& PA, const Geom::Vec3d& PB, const Geom::Vec3d& PlaneP, const Geom::Vec3d& NormP);
template Geom::Intersection Geom::intersectionSegmentPlan(const Geom::Vec3d& PA, const Geom::Vec3d& PB, const Geom::Vec3d& PlaneP, const Geom::Vec3d& NormP, Geom::Vec3d& Inter);
template Geom::Intersection Geom::intersectionTrianglePlan(const Geom::Vec3d& Ta, const Geom::Vec3d& Tb, const Geom::Vec3d& Tc, const Geom::Vec3d& PlaneP, const Geom::Vec3d& NormP);
template Geom::Intersection Geom::intersectionSegmentHalfPlan(const Geom::Vec3d& PA, const Geom::Vec3d& PB, const Geom::Vec3d& P, const Geom::Vec3d& DirP, const Geom::Vec3d& OrientP);
template Geom::Intersection Geom::intersectionTriangleHalfPlan(const Geom::Vec3d& Ta, const Geom::Vec3d& Tb, const Geom::Vec3d& Tc, const Geom::Vec3d& P, const Geom::Vec3d& DirP, const Geom::Vec3d& OrientP);
template bool Geom::interLineSeg(const Geom::Vec3d& A, const Geom::Vec3d& AB, double AB2, const Geom::Vec3d& P, const Geom::Vec3d& Q, Geom::Vec3d& inter);


int test_intersection()
{
	return 0;
}
