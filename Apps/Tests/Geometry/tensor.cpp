#include <iostream>

#include <Geometry/tensor.h>

using namespace CGoGN;

/*****************************************
*		TENSOR INSTANTIATION
*****************************************/

template class Geom::Tensor < 2, float>;
template class Geom::Tensor < 3, float>;
template class Geom::Tensor < 4, float>;

template class Geom::Tensor < 2, double>;
template class Geom::Tensor < 3, double>;
template class Geom::Tensor < 4, double>;




int test_tensor()
{
	return 0;
}
