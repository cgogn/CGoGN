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
bool chechXmlNode(xmlNodePtr node, const std::string& name);

template <typename PFP>
bool importSVG(typename PFP::MAP& map, const std::string& filename, std::vector<std::string>& attrNames);



}

}

}

#include "Algo/Import/importSvg.hpp"

#endif
