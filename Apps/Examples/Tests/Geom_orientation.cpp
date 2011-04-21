#include <iostream>
#include <Geometry/orientation.h>

using namespace CGoGN;

int main()
{
	typedef Geom::Vec3f VEC3;
	CGoGNout << "Check Geometry/orientation.h" << CGoGNendl;
	CGoGNout << "Check Status : PARTIAL" << CGoGNendl;
	
	// OrientationLine testOrientationLines<VEC3>(const VEC3& a,  const VEC3& b, const VEC3& c, const VEC3& d);
	
	// Orientation3D testOrientation3D<VEC3>(const VEC3& P, const VEC3& A, const VEC3& B, const VEC3& C);
	
	// Orientation3D testOrientation3D<VEC3>(const VEC3& P, const VEC3& N, const VEC3& PP);
	
	CGoGNout << "Check testOrientation2D : Start" << CGoGNendl;
	VEC3 p1(0,0,0);
	VEC3 p2(5,0,0);
	VEC3 p3(5,5,0);
	VEC3 p4(5,10,0);
	if(Geom::testOrientation2D<VEC3>(p3,p2,p1)!= Geom::RIGHT) {
		CGoGNout << "ERROR : testOrientation2D : testRight" << CGoGNendl;
	}
	
	if(Geom::testOrientation2D<VEC3>(p1,p2,p3)!= Geom::LEFT) {
		CGoGNout << "ERROR : testOrientation2D : testLeft" << CGoGNendl;
	}
	
	if(Geom::testOrientation2D<VEC3>(p4,p2,p3)!= Geom::ALIGNED 
		|| Geom::testOrientation2D<VEC3>(p2,p2,p3)!= Geom::ALIGNED
		|| Geom::testOrientation2D<VEC3>(p3,p2,p3)!= Geom::ALIGNED
		|| Geom::testOrientation2D<VEC3>(p2,p2,p2)!= Geom::ALIGNED
		) {
		CGoGNout << "ERROR : testOrientation2D : testAligned" << CGoGNendl;
	}
	
	CGoGNout << "Check testOrientation2D : Done" << CGoGNendl;

	// bool isTetrahedronWellOriented<VEC3>(const VEC3 points[4], bool CCW = true) ;

	return 0;
}