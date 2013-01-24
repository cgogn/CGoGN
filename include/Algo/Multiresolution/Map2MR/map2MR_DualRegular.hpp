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

namespace Surface
{

namespace MR
{

namespace Dual
{

namespace Regular
{

template <typename PFP>
Map2MR<PFP>::Map2MR(typename PFP::MAP& map) :
	m_map(map),
	shareVertexEmbeddings(true)
{

}

template <typename PFP>
Map2MR<PFP>::~Map2MR()
{
	unsigned int level = m_map.getCurrentLevel();
	unsigned int maxL = m_map.getMaxLevel();

	for(unsigned int i = maxL ; i > level ; --i)
		m_map.removeLevelBack();

	for(unsigned int i = 0 ; i < level ; ++i)
		m_map.removeLevelFront();
}

template <typename PFP>
void Map2MR<PFP>::addNewLevel(bool embedNewVertices)
{
	m_map.pushLevel() ;

	m_map.addLevelBack() ;
	m_map.duplicateDarts(m_map.getMaxLevel());
	m_map.setCurrentLevel(m_map.getMaxLevel()-1) ;

	TraversorV<typename PFP::MAP> travV(m_map) ;
	for (Dart d = travV.begin(); d != travV.end(); d = travV.next())
	{
		//create new faces in the previous edges
		unsigned int count = 0;
		Dart ditv = d;
		do
		{
			Dart d2 = m_map.phi2(ditv);

			m_map.incCurrentLevel();

			if(m_map.phi2(d) == d2)
			{

				m_map.unsewFaces(d, false);
				Dart nf = m_map.newFace(4,false);
				m_map.sewFaces(nf,d,false);
				m_map.sewFaces(m_map.phi1(m_map.phi1(nf)),d2,false);
			}


			m_map.decCurrentLevel();

			++count;

			ditv = m_map.phi2(m_map.phi_1(ditv));
		}while(ditv != d);

//		// create new faces in the previous vertices
//		Dart nf = m_map.newFace(count,false);
//		do
//		{
//			m_map.incCurrentLevel();
//
//			Dart d21 = m_map.phi1(m_map.phi2(ditv));
//			m_map.sewFaces(nf,d21,false);
//
//			if(embedNewVertices)
//						m_map.template embedNewCell<VERTEX>(nf) ;
//
//			nf = m_map.phi1(nf);
//
//			m_map.decCurrentLevel();
//
//
//			ditv = m_map.phi2(m_map.phi_1(ditv));
//		}while(ditv != d);
	}

	m_map.popLevel() ;
}

template <typename PFP>
void Map2MR<PFP>::analysis()
{
	assert(m_map.getCurrentLevel() > 0 || !"analysis : called on level 0") ;

	m_map.decCurrentLevel() ;

	for(unsigned int i = 0; i < analysisFilters.size(); ++i)
		(*analysisFilters[i])() ;
}

template <typename PFP>
void Map2MR<PFP>::synthesis()
{
	assert(m_map.getCurrentLevel() < m_map.getMaxLevel() || !"synthesis : called on max level") ;

	for(unsigned int i = 0; i < synthesisFilters.size(); ++i)
		(*synthesisFilters[i])() ;

	m_map.incCurrentLevel() ;
}


} // namespace Regular

} // namespace Dual

} // namespace MR

} // namespace Surface

} // namespace Algo

} // namespace CGoGN
