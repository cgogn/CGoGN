
#include "Algo/MC/windowing.h"
#include "Geometry/vector_gen.h"

using namespace CGoGN;

template class Algo::Surface::MC::WindowingEqual<unsigned char>;
template class Algo::Surface::MC::WindowingEqual<unsigned short>;
template class Algo::Surface::MC::WindowingEqual<unsigned int>;
template class Algo::Surface::MC::WindowingEqual<unsigned long long>;
template class Algo::Surface::MC::WindowingEqual<char>;
template class Algo::Surface::MC::WindowingEqual<short>;
template class Algo::Surface::MC::WindowingEqual<int>;
template class Algo::Surface::MC::WindowingEqual<long long>;
template class Algo::Surface::MC::WindowingEqual<float>;
template class Algo::Surface::MC::WindowingEqual<double>;
template class Algo::Surface::MC::WindowingEqual<Geom::Vec3f>;
template class Algo::Surface::MC::WindowingEqual<Geom::Vec4d>;


template class Algo::Surface::MC::WindowingDiff<unsigned char>;
template class Algo::Surface::MC::WindowingDiff<unsigned short>;
template class Algo::Surface::MC::WindowingDiff<unsigned int>;
template class Algo::Surface::MC::WindowingDiff<unsigned long long>;
template class Algo::Surface::MC::WindowingDiff<char>;
template class Algo::Surface::MC::WindowingDiff<short>;
template class Algo::Surface::MC::WindowingDiff<int>;
template class Algo::Surface::MC::WindowingDiff<long long>;
template class Algo::Surface::MC::WindowingDiff<float>;
template class Algo::Surface::MC::WindowingDiff<double>;
template class Algo::Surface::MC::WindowingDiff<Geom::Vec3f>;
template class Algo::Surface::MC::WindowingDiff<Geom::Vec4d>;


template class Algo::Surface::MC::WindowingGreater<unsigned char>;
template class Algo::Surface::MC::WindowingGreater<unsigned short>;
template class Algo::Surface::MC::WindowingGreater<unsigned int>;
template class Algo::Surface::MC::WindowingGreater<unsigned long long>;
template class Algo::Surface::MC::WindowingGreater<char>;
template class Algo::Surface::MC::WindowingGreater<short>;
template class Algo::Surface::MC::WindowingGreater<int>;
template class Algo::Surface::MC::WindowingGreater<long long>;
template class Algo::Surface::MC::WindowingGreater<float>;
template class Algo::Surface::MC::WindowingGreater<double>;

template class Algo::Surface::MC::WindowingLess<unsigned char>;
template class Algo::Surface::MC::WindowingLess<unsigned short>;
template class Algo::Surface::MC::WindowingLess<unsigned int>;
template class Algo::Surface::MC::WindowingLess<unsigned long long>;
template class Algo::Surface::MC::WindowingLess<char>;
template class Algo::Surface::MC::WindowingLess<short>;
template class Algo::Surface::MC::WindowingLess<int>;
template class Algo::Surface::MC::WindowingLess<long long>;
template class Algo::Surface::MC::WindowingLess<float>;
template class Algo::Surface::MC::WindowingLess<double>;

template class Algo::Surface::MC::WindowingInterval<unsigned char>;
template class Algo::Surface::MC::WindowingInterval<unsigned short>;
template class Algo::Surface::MC::WindowingInterval<unsigned int>;
template class Algo::Surface::MC::WindowingInterval<unsigned long long>;
template class Algo::Surface::MC::WindowingInterval<char>;
template class Algo::Surface::MC::WindowingInterval<short>;
template class Algo::Surface::MC::WindowingInterval<int>;
template class Algo::Surface::MC::WindowingInterval<long long>;
template class Algo::Surface::MC::WindowingInterval<float>;
template class Algo::Surface::MC::WindowingInterval<double>;


int test_windowing()
{
	return 0;
}
