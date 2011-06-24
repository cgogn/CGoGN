#ifndef __IMPORTSVG_H__
#define __IMPORTSVG_H__

namespace CGoGN
{

namespace Algo
{

namespace Import
{

/**
 * check if an xml node has a given name
 * @param node the xml node
 * @param name the name
 * @ return true if node has the good name
 */
bool checkXmlNode(xmlNodePtr node, const std::string& name);

template <typename PFP>
bool importSVG(typename PFP::MAP& map, const std::string& filename, typename PFP::TVEC3& position, CellMarker& polygons);



}

}

}

#include "Algo/Import/importSvg.hpp"

#endif
