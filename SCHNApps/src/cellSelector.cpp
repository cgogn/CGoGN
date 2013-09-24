#include "cellSelector.h"

namespace CGoGN
{

namespace SCHNApps
{

unsigned int CellSelectorGen::selectorCount = 0;

CellSelectorGen::CellSelectorGen(const QString& name) :
	m_name(name)
{
	++selectorCount;
}

} // namespace SCHNApps

} // namespace CGoGN
