#include <iostream>

#define CGOGN_NO_STATIC_ASSERT 1
#include "Geometry/vector_gen.h"

using namespace CGoGN;

template class Geom::Vector<2, short>;
template class Geom::Vector<2, float>;
template class Geom::Vector<2, double>;

template class Geom::Vector<3, short>;
template class Geom::Vector<3, float>;
template class Geom::Vector<3, double>;

template class Geom::Vector<4, short>;
template class Geom::Vector<4, float>;
template class Geom::Vector<4, double>;

template class Geom::Vector<11, short>;
template class Geom::Vector<11, float>;
template class Geom::Vector<11, double>;

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

