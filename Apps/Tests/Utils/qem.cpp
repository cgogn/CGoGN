#include "Utils/qem.h"


template class CGoGN::Utils::Quadric<float>;
template class CGoGN::Utils::Quadric<double>;

template class CGoGN::Utils::QuadricNd<float,5>;
template class CGoGN::Utils::QuadricNd<double,6>;


template class CGoGN::Utils::QuadricHF<float>;
template class CGoGN::Utils::QuadricHF<double>;


int test_qem()
{

	return 0;
}
