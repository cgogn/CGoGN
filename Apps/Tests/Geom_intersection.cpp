#include <iostream>
#include <Geometry/intersection.h>

using namespace CGoGN;

int main()
{
	typedef Geom::Vec3f VEC3;
	std::cout << "Check Geometry/intersection.h" << std::endl;
	std::cout << "Check Status : PARTIAL" << std::endl;

//	enum Intersection
//	{
//		NO_INTERSECTION = 0,
//		VERTEX_INTERSECTION = 1,
//		EDGE_INTERSECTION = 2,
//		FACE_INTERSECTION = 3
//	} ;

//	Intersection intersectionRayTriangle(const VEC3& P, const VEC3& Dir, const VEC3& Ta,  const VEC3& Tb, const VEC3& Tc, VEC3& Inter) ;

//	Intersection intersectionLineTriangle(const VEC3& P, const VEC3& Dir, const VEC3& Ta,  const VEC3& Tb, const VEC3& Tc, VEC3& Inter) ;

//	Intersection intersectionLineTriangle2D(const VEC3& P, const VEC3& Dir, const VEC3& Ta,  const VEC3& Tb, const VEC3& Tc, VEC3& Inter) ;

//	Intersection intersectionSegmentTriangle(const VEC3& PA, const VEC3& PB, const VEC3& Ta,  const VEC3& Tb, const VEC3& Tc, VEC3& Inter) ;

	std::cout << "Check intersectPlaneRay : Start" << std::endl;
	VEC3 p1(0,0,0);
	VEC3 p2(5,0,0);
	VEC3 p3(5,5,0);
	VEC3 p4(5,10,0);
	VEC3 p5(5,10,1);
	VEC3 p6(10,13,13);
	Geom::Plane3D<float> pl1(p1,p2,p3);
	Geom::Plane3D<float> pl2(p3,p4,p5);
	Geom::Plane3D<float> pl3(p3,p5,p6);
	VEC3 res;

	if(intersectionPlaneRay(pl1,p4,VEC3(0,1,0),res) == Geom::FACE_INTERSECTION) {
		if(p4[0]!=res[0] || p4[1]!=res[1] || p4[2]!=res[2])
			std::cout << "ERROR : intersectPlaneRay : point in plane (bad intersection point)" << std::endl;
	}
	else
		std::cout << "ERROR : intersectPlaneRay : point in plane" << std::endl;

	if(intersectionPlaneRay(pl1,p5,VEC3(0,0,-1),res) == Geom::FACE_INTERSECTION) {
		if(p4[0]!=res[0] || p4[1]!=res[1] || p4[2]!=res[2])
			std::cout << "ERROR : intersectPlaneRay : point/plane (bad intersection point)" << std::endl;
	}
	else
		std::cout << "ERROR : intersectPlaneRay : point/plane" << std::endl;

	if(intersectionPlaneRay(pl2,p1,VEC3(1,0,0),res) != Geom::FACE_INTERSECTION)
		std::cout << "ERROR : intersectPlaneRay (1)" << std::endl;

	if(intersectionPlaneRay(pl3,p1,VEC3(1,0,0),res) != Geom::FACE_INTERSECTION)
		std::cout << "ERROR : intersectPlaneRay (2)" << std::endl;

	if(intersectionPlaneRay(pl2,p1,VEC3(0,1,0),res) == Geom::FACE_INTERSECTION)
		std::cout << "ERROR : intersectPlaneRay : ray parallel to plane" << std::endl;

	if(intersectionPlaneRay(pl1,p5,VEC3(0,0,1),res) == Geom::FACE_INTERSECTION)
		std::cout << "ERROR : intersectPlaneRay : point/plane" << std::endl;

	std::cout << "Check intersectPlaneRay : Done" << std::endl;

//	Intersection intersection2DSegmentSegment(const VEC3& PA, const VEC3& PB, const VEC3& QA,  const VEC3& QB, VEC3& Inter) ;

	return 0;
}
