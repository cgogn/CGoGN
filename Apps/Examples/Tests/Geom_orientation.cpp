#include <iostream>
#include <Geometry/orientation.h>

using namespace CGoGN;

int main()
{
	typedef Geom::Vec3f VEC3;
	std::cout << "Check Geometry/orientation.h" << std::endl;
	std::cout << "Check Status : PARTIAL" << std::endl;
	
	// OrientationLine testOrientationLines<VEC3>(const VEC3& a,  const VEC3& b, const VEC3& c, const VEC3& d);
	
	// Orientation3D testOrientation3D<VEC3>(const VEC3& P, const VEC3& A, const VEC3& B, const VEC3& C);
	
	// Orientation3D testOrientation3D<VEC3>(const VEC3& P, const VEC3& N, const VEC3& PP);
	
	std::cout << "Check testOrientation2D : Start" << std::endl;
	VEC3 p1(0,0,0);
	VEC3 p2(5,0,0);
	VEC3 p3(5,5,0);
	VEC3 p4(5,10,0);
	if(Geom::testOrientation2D<VEC3>(p3,p2,p1)!= Geom::RIGHT) {
		std::cout << "ERROR : testOrientation2D : testRight" << std::endl;
	}
	
	if(Geom::testOrientation2D<VEC3>(p1,p2,p3)!= Geom::LEFT) {
		std::cout << "ERROR : testOrientation2D : testLeft" << std::endl;
	}
	
	if(Geom::testOrientation2D<VEC3>(p4,p2,p3)!= Geom::ALIGNED 
		|| Geom::testOrientation2D<VEC3>(p2,p2,p3)!= Geom::ALIGNED
		|| Geom::testOrientation2D<VEC3>(p3,p2,p3)!= Geom::ALIGNED
		|| Geom::testOrientation2D<VEC3>(p2,p2,p2)!= Geom::ALIGNED
		) {
		std::cout << "ERROR : testOrientation2D : testAligned" << std::endl;
	}
	
	std::cout << "Check testOrientation2D : Done" << std::endl;

	// bool isTetrahedronWellOriented<VEC3>(const VEC3 points[4], bool CCW = true) ;

	return 0;
}
