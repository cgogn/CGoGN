#include <iostream>
#include <Geometry/basic.h>

using namespace CGoGN;

/*****************************************
*		BASIC INSTANTIATION
*****************************************/

template Geom::Vec3f Geom::lerp<Geom::Vec3f>(const Geom::Vec3f& v1, const Geom::Vec3f& v2, float s);
template Geom::Vec3d Geom::lerp<Geom::Vec3d>(const Geom::Vec3d& v1, const Geom::Vec3d& v2, double s);
template Geom::Vec4f Geom::lerp<Geom::Vec4f>(const Geom::Vec4f& v1, const Geom::Vec4f& v2, float s);
template Geom::Vec4d Geom::lerp<Geom::Vec4d>(const Geom::Vec4d& v1, const Geom::Vec4d& v2, double s);

template Geom::Vector<3, float> Geom::barycenter(const Vector<3, float>& v1, const Vector<3, float>& v2, float a, float b);
template Geom::Vector<3, double> Geom::barycenter(const Vector<3, double>& v1, const Vector<3, double>& v2, double a, double b);
template Geom::Vector<4, float> Geom::barycenter(const Vector<4, float>& v1, const Vector<4, float>& v2, float a, float b);
template Geom::Vector<4, double> Geom::barycenter(const Vector<4, double>& v1, const Vector<4, double>& v2, double a, double b);

template Geom::Vector<3, float> Geom::isobarycenter(const Vector<3, float>& v1, const Vector<3, float>& v2);
template Geom::Vector<3, double> Geom::isobarycenter(const Vector<3, double>& v1, const Vector<3, double>& v2);
template Geom::Vector<4, float> Geom::isobarycenter(const Vector<4, float>& v1, const Vector<4, float>& v2);
template Geom::Vector<4, double> Geom::isobarycenter(const Vector<4, double>& v1, const Vector<4, double>& v2);

template Geom::Vector<3, float> Geom::barycenter(const Vector<3, float>& v1, const Vector<3, float>& v2, const Vector<3, float>& v3, float a, float b, float c);
template Geom::Vector<3, double> Geom::barycenter(const Vector<3, double>& v1, const Vector<3, double>& v2, const Vector<3, double>& v3, double a, double b, double c);
template Geom::Vector<4, float> Geom::barycenter(const Vector<4, float>& v1, const Vector<4, float>& v2, const Vector<4, float>& v3, float a, float b, float c);
template Geom::Vector<4, double> Geom::barycenter(const Vector<4, double>& v1, const Vector<4, double>& v2, const Vector<4, double>& v3, double a, double b, double c);

template Geom::Vector<3, float> Geom::isobarycenter(const Vector<3, float>& v1, const Vector<3, float>& v2, const Vector<3, float>& v3);
template Geom::Vector<3, double> Geom::isobarycenter(const Vector<3, double>& v1, const Vector<3, double>& v2, const Vector<3, double>& v3);
template Geom::Vector<4, float> Geom::isobarycenter(const Vector<4, float>& v1, const Vector<4, float>& v2, const Vector<4, float>& v3);
template Geom::Vector<4, double> Geom::isobarycenter(const Vector<4, double>& v1, const Vector<4, double>& v2, const Vector<4, double>& v3);

template float Geom::angle(const Geom::Vec3f& a, const Geom::Vec3f& b);
template double Geom::angle(const Geom::Vec3d& a, const Geom::Vec3d& b);

template float Geom::triangleArea(const Geom::Vec3f& p1, const Geom::Vec3f& p2, const Geom::Vec3f& p3);
template double Geom::triangleArea(const Geom::Vec3d& p1, const Geom::Vec3d& p2, const Geom::Vec3d& p3);

template Geom::Vec3f Geom::triangleNormal(const Geom::Vec3f& p1, const Geom::Vec3f& p2, const Geom::Vec3f& p3);
template Geom::Vec3d Geom::triangleNormal(const Geom::Vec3d& p1, const Geom::Vec3d& p2, const Geom::Vec3d& p3);

template float Geom::tetraVolume(const  Geom::Vec3f& p1, const  Geom::Vec3f& p2, const  Geom::Vec3f& p3, const  Geom::Vec3f& p4);
template double Geom::tetraVolume(const  Geom::Vec3d& p1, const  Geom::Vec3d& p2, const  Geom::Vec3d& p3, const  Geom::Vec3d& p4);

template float Geom::parallelepipedVolume(const  Geom::Vec3f& p1, const  Geom::Vec3f& p2, const  Geom::Vec3f& p3);
template double Geom::parallelepipedVolume(const  Geom::Vec3d& p1, const  Geom::Vec3d& p2, const  Geom::Vec3d& p3);



int test_basic()
{
	return 0;
}
