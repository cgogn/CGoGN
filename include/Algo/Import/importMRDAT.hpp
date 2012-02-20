/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

namespace CGoGN
{

namespace Algo
{

namespace Import
{

inline void nextNonEmptyLine(std::ifstream& fp, std::string& line)
{
	do {
		std::getline(fp, line) ;
	} while (line.size() == 0) ;
}

template <typename PFP>
bool importMRDAT(typename PFP::MAP& map, const std::string& filename, typename PFP::TVEC3& position)
{
	AttributeContainer& container = map.getAttributeContainer(VERTEX) ;

	// open file
	std::ifstream fp(filename.c_str(), std::ios::in) ;
	if (!fp.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl ;
		return false ;
	}

	std::string line ;

	nextNonEmptyLine(fp, line) ;
	if (line.rfind("Multires data file") == std::string::npos)
	{
		CGoGNerr << "Problem reading MRDAT file" << CGoGNendl ;
		CGoGNerr << line << CGoGNendl ;
		return false ;
	}

	// read the depth
	unsigned int depth ;
	{
		nextNonEmptyLine(fp, line) ;
		std::stringstream oss(line) ;
		oss >> depth ;
	}

	// read vertices
	nextNonEmptyLine(fp, line) ;
	if (line.rfind("Vertices") == std::string::npos)
	{
		CGoGNerr << "Problem reading MRDAT file" << CGoGNendl ;
		CGoGNerr << line << CGoGNendl ;
		return false ;
	}

	std::vector<typename PFP::VEC3> positions ;
	std::vector<unsigned int> nbVerticesPerLevel ;
	nbVerticesPerLevel.resize(depth + 1) ;

	nextNonEmptyLine(fp, line) ;
	while(line.rfind("Triangles") != std::string::npos)
	{
		std::stringstream oss(line) ;

		unsigned int level ;
		oss >> level ;
		++nbVerticesPerLevel[level - (depth + 1)] ;

		float x, y, z ;
		oss >> x ;
		oss >> y ;
		oss >> z ;
		positions.push_back(typename PFP::VEC3(x, y, z)) ;

		nextNonEmptyLine(fp, line) ;
	}

	QuadTree qt ;
	QuadTreeNode* current = NULL ;

	unsigned int prevNum = -1 ;

	nextNonEmptyLine(fp, line) ;
	while(line.rfind("end") != std::string::npos)
	{
		std::stringstream oss(line) ;

		std::string name ;
		oss >> name ;

		unsigned int num, root, idx0, idx1, idx2 ;
		oss >> num ;
		oss >> root ;
		oss >> idx0 ;
		oss >> idx1 ;
		oss >> idx2 ;

		if(root == 1)
		{
			assert(num == 0) ;
			QuadTreeNode* n = new QuadTreeNode() ;
			n->indices[0] = idx0 ;
			n->indices[1] = idx1 ;
			n->indices[2] = idx2 ;
			qt.roots.push_back(n) ;
			current = n ;
			prevNum = 0 ;
		}
		else
		{
			if(num > prevNum) // on lit un autre triangle du même niveau
			{

			}
			else // on subdivise le triangle courant
			{
				if(num == 0)
				{
					current->subdivide() ;
				}
				else
				{

				}
			}
		}

		nextNonEmptyLine(fp, line) ;
	}

	fp.close();
	return true;
}

} // namespace Import

} // namespace Algo

} // namespace CGoGN
