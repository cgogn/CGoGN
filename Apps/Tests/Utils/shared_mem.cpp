#include "Utils/shared_mem.h"
#include "Geometry/vector_gen.h"

template class CGoGN::Utils::SharedMemSeg<int>;
template class CGoGN::Utils::SharedMemSeg<float>;
template class CGoGN::Utils::SharedMemSeg<double>;

template class CGoGN::Utils::SharedMemSeg<CGoGN::Geom::Vec4f>;


int test_shared_mem()
{

	return 0;
}


