#include "cellSelector.h"

namespace CGoGN
{

namespace SCHNApps
{

unsigned int CellSelectorGen::selectorCount = 0;

CellSelectorGen::CellSelectorGen(const QString& name) :
	m_name(name),
	m_isMutuallyExclusive(false),
	m_selectionChanged(false)
{
	++selectorCount;
}

} // namespace SCHNApps

} // namespace CGoGN
