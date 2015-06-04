#include "Utils/textures.h"


template class CGoGN::Utils::Filter<2>;
template class CGoGN::Utils::Filter<3>;


template class CGoGN::Utils::ImageData<2, unsigned char>;
template class CGoGN::Utils::Image<2, unsigned char>;
template class CGoGN::Utils::Texture<2, unsigned char>;

template class CGoGN::Utils::ImageData<2, int>;
template class CGoGN::Utils::Image<2, int>;
template class CGoGN::Utils::Texture<2, int>;

template class CGoGN::Utils::ImageData<2, double>;
template class CGoGN::Utils::Image<2, double>;
template class CGoGN::Utils::Texture<2, double>;


int test_texture()
{

	return 0;
}