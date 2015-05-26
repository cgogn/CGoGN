#include <iostream>

#include <Geometry/matrix.h>

using namespace CGoGN;

// template instantiation not possible (static assert pb);

int test_vector()
{
	Geom::Vec3f v1(3.0f,4.0f,5.0f);
	Geom::Vec3f v2(13.0f,14.0f,15.0f);

	Geom::Vec3f v3 = v1+v2;
	Geom::Vec3f v4 = v1^v2;
	float sc = v1*v2;

	v1 *= 10.0f;
	v2 *= 100.0f;
	
	return 0;
}

