#include <iostream>
#include <Geometry/vector_gen.h>
#include <Geometry/bounding_box.h>

using namespace CGoGN;


/*****************************************
*		BOUNDING BOX INSTANTIATION
*****************************************/

template class Geom::BoundingBox< Geom::Vector<3, int> >;
template class Geom::BoundingBox< Geom::Vector<3, float> >;
template class Geom::BoundingBox< Geom::Vector<3, double> >;

template class Geom::BoundingBox< Geom::Vector<4, int> >;
template class Geom::BoundingBox< Geom::Vector<4, float> >;
template class Geom::BoundingBox< Geom::Vector<4, double> >;



int test_bounding_box()
{

	return 0;
}
