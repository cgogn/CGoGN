
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Container/containerBrowser.h"

namespace CGoGN
{


template class DartContainerBrowserSelector<EmbeddedMap2>;
template class DartContainerBrowserSelector<EmbeddedMap3>;
template class DartContainerBrowserSelector<EmbeddedGMap2>;


template class ContainerBrowserCellMarked<EmbeddedMap2, VERTEX>;
template class ContainerBrowserCellMarked<EmbeddedMap2, EDGE>;
template class ContainerBrowserCellMarked<EmbeddedMap2, FACE>;

template class ContainerBrowserCellMarked<EmbeddedGMap2, VERTEX>;
template class ContainerBrowserCellMarked<EmbeddedGMap2, EDGE>;
template class ContainerBrowserCellMarked<EmbeddedGMap2, FACE>;

template class ContainerBrowserCellMarked<EmbeddedMap3, VERTEX>;
template class ContainerBrowserCellMarked<EmbeddedMap3, EDGE>;
template class ContainerBrowserCellMarked<EmbeddedMap3, FACE>;
template class ContainerBrowserCellMarked<EmbeddedMap3, VOLUME>;

}

int test_containerBrowser()
{

	return 0;
}