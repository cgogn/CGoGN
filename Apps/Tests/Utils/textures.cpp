#define CGOGN_NO_STATIC_ASSERT 1

#include "Utils/textures.h"

template class CGoGN::Utils::Filter<1>;
template class CGoGN::Utils::Filter<2>;
template class CGoGN::Utils::Filter<3>;

///////////////// 1 ////////////////////////////////

template class CGoGN::Utils::ImageData<1, unsigned char>;
template class CGoGN::Utils::Image<1, unsigned char>;
template class CGoGN::Utils::Texture<1, unsigned char>;

template class CGoGN::Utils::ImageData<1, int>;
template class CGoGN::Utils::Image<1, int>;
template class CGoGN::Utils::Texture<1, int>;

template class CGoGN::Utils::ImageData<1, double>;
template class CGoGN::Utils::Image<1, double>;
template class CGoGN::Utils::Texture<1, double>;

template class CGoGN::Utils::ImageData<1, CGoGN::Geom::Vec4f>;
template class CGoGN::Utils::Image<1, CGoGN::Geom::Vec4f>;
template class CGoGN::Utils::Texture<1, CGoGN::Geom::Vec4f>;

///////////////// 2 //////////////////////////////////////

template class CGoGN::Utils::ImageData<2, unsigned char>;
template class CGoGN::Utils::Image<2, unsigned char>;
template class CGoGN::Utils::Texture<2, unsigned char>;

template class CGoGN::Utils::ImageData<2, int>;
template class CGoGN::Utils::Image<2, int>;
template class CGoGN::Utils::Texture<2, int>;

template class CGoGN::Utils::ImageData<2, double>;
template class CGoGN::Utils::Image<2, double>;
template class CGoGN::Utils::Texture<2, double>;

template class CGoGN::Utils::ImageData<2, CGoGN::Geom::Vec4f>;
template class CGoGN::Utils::Image<2, CGoGN::Geom::Vec4f>;
template class CGoGN::Utils::Texture<2, CGoGN::Geom::Vec4f>;

////////////////// 3 /////////////////////////////////////

template class CGoGN::Utils::ImageData<3, unsigned char>;
template class CGoGN::Utils::Image<3, unsigned char>;
template class CGoGN::Utils::Texture<3, unsigned char>;

template class CGoGN::Utils::ImageData<3, int>;
template class CGoGN::Utils::Image<3, int>;
template class CGoGN::Utils::Texture<3, int>;

template class CGoGN::Utils::ImageData<3, double>;
template class CGoGN::Utils::Image<3, double>;
template class CGoGN::Utils::Texture<3, double>;

template class CGoGN::Utils::ImageData<3, CGoGN::Geom::Vec4f>;
template class CGoGN::Utils::Image<3, CGoGN::Geom::Vec4f>;
template class CGoGN::Utils::Texture<3, CGoGN::Geom::Vec4f>;

int test_texture()
{

	return 0;
}