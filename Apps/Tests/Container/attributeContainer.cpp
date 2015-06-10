#include "Container/attributeContainer.h"
#include "Geometry/vector_gen.h"


namespace CGoGN
{

// char instantiation

template AttributeMultiVector<char>* AttributeContainer::addAttribute<char>(const std::string& attribName);
template void AttributeContainer::addAttribute<char>(const std::string& attribName, const std::string& typeName, unsigned int index);
template bool AttributeContainer::removeAttribute<char>(const std::string& attribName);
template bool AttributeContainer::removeAttribute<char>(unsigned int index);
template unsigned int AttributeContainer::getAttributeBlocksPointers<char>(unsigned int attrIndex, std::vector<char*>& vect_ptr, unsigned int& byteBlockSize) const;
template AttributeMultiVector<char>* AttributeContainer::getDataVector<char>(unsigned int attrIndex);
template AttributeMultiVector<char>* AttributeContainer::getDataVector<char>(const std::string& attribName);
template char& AttributeContainer::getData<char>(unsigned int attrIndex, unsigned int eltIndex) ;
template const char& AttributeContainer::getData<char>(unsigned int attrIndex, unsigned int eltIndex) const ;
template void AttributeContainer::setData<char>(unsigned int attrIndex, unsigned int eltIndex, const char& data);


// int instantiation
//
//template AttributeMultiVector<int>* AttributeContainer::addAttribute<int>(const std::string& attribName);
//template void AttributeContainer::addAttribute<int>(const std::string& attribName, const std::string& typeName, unsigned int index);
//template bool AttributeContainer::removeAttribute<int>(const std::string& attribName);
//template bool AttributeContainer::removeAttribute<int>(unsigned int index);
//template unsigned int AttributeContainer::getAttributeBlocksPointers<int>(unsigned int attrIndex, std::vector<int*>& vect_ptr, unsigned int& byteBlockSize) const;
//template AttributeMultiVector<int>* AttributeContainer::getDataVector<int>(unsigned int attrIndex);
//template AttributeMultiVector<int>* AttributeContainer::getDataVector<int>(const std::string& attribName);
//template int& AttributeContainer::getData<int>(unsigned int attrIndex, unsigned int eltIndex);
//template const int& AttributeContainer::getData<int>(unsigned int attrIndex, unsigned int eltIndex) const;
//template void AttributeContainer::setData<int>(unsigned int attrIndex, unsigned int eltIndex, const int& data);
//
//
//// double instantiation
//
//template AttributeMultiVector<double>* AttributeContainer::addAttribute<double>(const std::string& attribName);
//template void AttributeContainer::addAttribute<double>(const std::string& attribName, const std::string& typeName, unsigned int index);
//template bool AttributeContainer::removeAttribute<double>(const std::string& attribName);
//template bool AttributeContainer::removeAttribute<double>(unsigned int index);
//template unsigned int AttributeContainer::getAttributeBlocksPointers<double>(unsigned int attrIndex, std::vector<double*>& vect_ptr, unsigned int& byteBlockSize) const;
//template AttributeMultiVector<double>* AttributeContainer::getDataVector<double>(unsigned int attrIndex);
//template AttributeMultiVector<double>* AttributeContainer::getDataVector<double>(const std::string& attribName);
//template double& AttributeContainer::getData<double>(unsigned int attrIndex, unsigned int eltIndex);
//template const double& AttributeContainer::getData<double>(unsigned int attrIndex, unsigned int eltIndex) const;
//template void AttributeContainer::setData<double>(unsigned int attrIndex, unsigned int eltIndex, const double& data);
//
//
//// Geom::Vec4f instantiation
//
//template AttributeMultiVector<Geom::Vec4f>* AttributeContainer::addAttribute<Geom::Vec4f>(const std::string& attribName);
//template void AttributeContainer::addAttribute<Geom::Vec4f>(const std::string& attribName, const std::string& typeName, unsigned int index);
//template bool AttributeContainer::removeAttribute<Geom::Vec4f>(const std::string& attribName);
//template bool AttributeContainer::removeAttribute<Geom::Vec4f>(unsigned int index);
//template unsigned int AttributeContainer::getAttributeBlocksPointers<Geom::Vec4f>(unsigned int attrIndex, std::vector<Geom::Vec4f*>& vect_ptr, unsigned int& byteBlockSize) const;
//template AttributeMultiVector<Geom::Vec4f>* AttributeContainer::getDataVector<Geom::Vec4f>(unsigned int attrIndex);
//template AttributeMultiVector<Geom::Vec4f>* AttributeContainer::getDataVector<Geom::Vec4f>(const std::string& attribName);
//template Geom::Vec4f& AttributeContainer::getData<Geom::Vec4f>(unsigned int attrIndex, unsigned int eltIndex);
//template const Geom::Vec4f& AttributeContainer::getData<Geom::Vec4f>(unsigned int attrIndex, unsigned int eltIndex) const;
//template void AttributeContainer::setData<Geom::Vec4f>(unsigned int attrIndex, unsigned int eltIndex, const Geom::Vec4f& data);

}

int test_attributeContainer()
{

	return 0;
}