
#include "Algo/MC/image.h"

using namespace CGoGN;

template class Algo::Surface::MC::Image<unsigned char>;
template class Algo::Surface::MC::Image<unsigned short>;
template class Algo::Surface::MC::Image<unsigned int>;
template class Algo::Surface::MC::Image<unsigned long long>;

template class Algo::Surface::MC::Image<char>;
template class Algo::Surface::MC::Image<short>;
template class Algo::Surface::MC::Image<int>;
template class Algo::Surface::MC::Image<long long>;

template class Algo::Surface::MC::Image<float>;
template class Algo::Surface::MC::Image<double>;

int test_image()
{
	return 0;
}