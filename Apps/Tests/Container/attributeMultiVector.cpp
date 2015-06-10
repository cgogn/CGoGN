#include "Container/attributeMultiVector.h"
#include "Geometry/vector_gen.h"
#include "Container/fakeAttribute.h"


namespace CGoGN
{

template class AttributeMultiVector<char>;
template class AttributeMultiVector<unsigned char>;
template class AttributeMultiVector<int>;
template class AttributeMultiVector<unsigned int>;
template class AttributeMultiVector<long int>;
template class AttributeMultiVector<unsigned long int>;
template class AttributeMultiVector<float>;
template class AttributeMultiVector<double>;
template class AttributeMultiVector<Geom::Vec3f>;
template class AttributeMultiVector< NoTypeNameAttribute<std::vector<Geom::Vec2i>::const_iterator> >;

}

int test_attributeMultiVector()
{

	return 0;
}

