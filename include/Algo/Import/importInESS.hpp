/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include "Algo/Modelisation/polyhedron.h"
#include <vector>

namespace CGoGN
{
namespace Algo
{
namespace Import 
{

template <typename PFP>
typename PFP::VEC3 stringToEmb(std::string s)
{
	if(s[0] == '(')
		s.erase(0,2);
	if(s[s.size()-1] == ')')
		s.erase(s.size()-2,2);

	std::stringstream oss(s);

	int x,y,z;
	oss >> x;
	oss >> y;
	oss >> z;

	typename PFP::VEC3 coord(x,y,z);

	CGoGNout << coord << CGoGNendl;

	return coord;
}

template <typename PFP>
bool importInESS(typename PFP::MAP& map, const std::string& filename, std::vector<std::string>& attrNames)
{
	typedef typename PFP::VEC3 VEC3;

	AttributeHandler<VEC3> position = map.template addAttribute<VEC3>(VERTEX_ORBIT, "position") ;
	attrNames.push_back(position.name()) ;

	AttributeContainer& container = map.getAttributeContainer(VERTEX_CELL) ;

	// open file
	std::ifstream fp(filename.c_str(), std::ios::in);
	if (!fp.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl;
		return false;
	}

	std::string line;
	size_t posData, posCoord;
	std::string bg;
	int count = 0;

	//two columns cutted
	std::getline(fp, line);
	posData = line.find("\t");
	++count;

	// First column
	//Bounding box : first coord & second coord
	bg = line.substr(0, posData);
	posCoord = bg.find(") (");
	stringToEmb<PFP>(bg.substr(0, posCoord));
	stringToEmb<PFP>(bg.substr(posCoord+3));

	while ( std::getline( fp, line ) )
	{
		posData = line.find("\t");

		// First column
		//Bounding box : first coord & second coord
		bg = line.substr(0, posData);
		posCoord = bg.find(") (");
		stringToEmb<PFP>(bg.substr(0,posCoord));
		stringToEmb<PFP>(bg.substr(posCoord+3));

		//Second column
		bg = line.substr(posData+1);
		posCoord = bg.find(") (");
		stringToEmb<PFP>(bg.substr(0, posCoord));
		stringToEmb<PFP>(bg.substr(posCoord+3));

	   ++count;
	}

	fp.close();
	return true;
}

} // namespace Import

} // namespace Algo

} // namespace CGoGN
