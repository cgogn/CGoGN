#include "cellSelector.h"

namespace CGoGN
{

namespace SCHNApps
{

unsigned int CellSelectorGen::selectorCount = 0;

CellSelectorGen::CellSelectorGen(const QString& name) :
	m_name(name),
	m_selectionChanged(false),
	m_isMutuallyExclusive(false)
{
	++selectorCount;
}

} // namespace SCHNApps

} // namespace CGoGN
