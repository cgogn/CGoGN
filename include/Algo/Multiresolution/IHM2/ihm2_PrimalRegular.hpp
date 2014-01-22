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

namespace MR
{

namespace Primal
{

namespace Regular
{

template <typename PFP>
IHM2<PFP>::IHM2(typename PFP::MAP& map) :
    m_map(map),
    shareVertexEmbeddings(true)
{

}

//if true : tri and quad else quad
template <typename PFP>
void IHM2<PFP>::addNewLevel(bool triQuad)
{
    unsigned int cur = m_map.getCurrentLevel() ; //pushLevel

	m_map.setCurrentLevel(m_map.getMaxLevel() + 1) ;

    // cut edges
    TraversorE<typename PFP::MAP> travE(m_map) ;
    for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
    {
        Dart dd = m_map.phi2(d) ;

        m_map.cutEdge(d) ;
        unsigned int eId = m_map.getEdgeId(d) ;
        m_map.setEdgeId(m_map.phi1(d), eId) ;
        m_map.setEdgeId(m_map.phi1(dd), eId) ;

        travE.skip(d) ;
        travE.skip(m_map.phi1(d)) ;
    }

    // split faces
    TraversorF<typename PFP::MAP> travF(m_map) ;
    for (Dart d = travF.begin(); d != travF.end(); d = travF.next())
    {
        Dart old = d ;

        if(m_map.getDartLevel(old) == m_map.getMaxLevel())
            old = m_map.phi1(old) ;

        m_map.decCurrentLevel();
        unsigned int degree = m_map.faceDegree(old) ;
        m_map.incCurrentLevel();

        if((degree == 3) && triQuad)								// if subdividing a triangle
        {
            Dart dd = m_map.phi1(old) ;
            Dart e = m_map.phi1(m_map.phi1(dd)) ;
            m_map.splitFace(dd, e) ;					// insert a new edge
            travF.skip(dd) ;
            unsigned int id = m_map.getNewEdgeId() ;
            m_map.setEdgeId(m_map.phi_1(dd), id) ;		// set the edge id of the inserted
            m_map.setEdgeId(m_map.phi_1(e), id) ;		// edge to the next available id

            dd = e ;
            e = m_map.phi1(m_map.phi1(dd)) ;
            m_map.splitFace(dd, e) ;
            travF.skip(dd) ;
            id = m_map.getNewEdgeId() ;
            m_map.setEdgeId(m_map.phi_1(dd), id) ;
            m_map.setEdgeId(m_map.phi_1(e), id) ;

            dd = e ;
            e = m_map.phi1(m_map.phi1(dd)) ;
            m_map.splitFace(dd, e) ;
            travF.skip(dd) ;
            id = m_map.getNewEdgeId() ;
            m_map.setEdgeId(m_map.phi_1(dd), id) ;
            m_map.setEdgeId(m_map.phi_1(e), id) ;

            travF.skip(e) ;
        }
        else											// if subdividing a polygonal face
        {
            Dart dd = m_map.phi1(old) ;
            Dart next = m_map.phi1(m_map.phi1(dd)) ;
            m_map.splitFace(dd, next) ;		// insert a first edge

            Dart ne = m_map.phi2(m_map.phi_1(dd)) ;
            Dart ne2 = m_map.phi2(ne) ;
            m_map.cutEdge(ne) ;				// cut the new edge to insert the central vertex
            travF.skip(dd) ;
            unsigned int id = m_map.getNewEdgeId() ;
            m_map.setEdgeId(ne, id) ;
            m_map.setEdgeId(m_map.phi2(ne), id) ;			// set the edge id of the inserted
            id = m_map.getNewEdgeId() ;
            m_map.setEdgeId(ne2, id) ;					// edges to the next available ids
            m_map.setEdgeId(m_map.phi2(ne2), id) ;

            dd = m_map.phi1(m_map.phi1(next)) ;
            while(dd != ne)				// turn around the face and insert new edges
            {							// linked to the central vertex
                Dart tmp = m_map.phi1(ne) ;
                m_map.splitFace(tmp, dd) ;
                travF.skip(tmp) ;
                Dart nne = m_map.phi2(m_map.phi_1(dd)) ;
                id = m_map.getNewEdgeId() ;
                m_map.setEdgeId(nne, id) ;
                m_map.setEdgeId(m_map.phi2(nne), id) ;
                dd = m_map.phi1(m_map.phi1(dd)) ;
            }
            travF.skip(ne) ;
        }
    }

    m_map.setCurrentLevel(cur) ;
}

template <typename PFP>
void IHM2<PFP>::addLevelFront()
{
	DartMarker md(m_map);

	std::vector<Dart> visitedVertices;
	visitedVertices.reserve(1024);

	//look for an irregular vertex

	TraversorV<typename PFP::MAP> tv(m_map);
	bool found = false;
	for(Dart d = tv.begin() ; !found && d != tv.end() ; d = tv.next())
	{
		if(m_map.vertexDegree(d) != 6)
		{
			found = true;
			visitedVertices.push_back(d);
		}
	}

	std::cout << "d = " << visitedVertices[0] << std::endl;

	for(unsigned int i = 0 ; i < visitedVertices.size() ; ++i)
	{
		Dart d = visitedVertices[i];

		Traversor2VE<typename PFP::MAP> tve(m_map, d);
		for(Dart eit = tve.begin() ; eit != tve.end() ; eit = tve.next())
		{
			//coarse all faces around the vertex
			if(!md.isMarked(eit))
			{
				unsigned int degree = m_map.faceDegree(eit);

				if(degree == 3)
				{
					Dart fit1 = m_map.phi2(m_map.phi1(eit));
					Dart fit2 = m_map.phi1(fit1);
					Dart fit3 = m_map.phi1(fit2);

					unsigned int id = m_map.getNewEdgeId() ;
					m_map.setEdgeId(fit1, id) ;
					id = m_map.getNewEdgeId() ;
					m_map.setEdgeId(fit2, id) ;
					id = m_map.getNewEdgeId() ;
					m_map.setEdgeId(fit3, id) ;

					md.markOrbit<FACE>(fit1);
					md.markOrbit<FACE>(m_map.phi2(fit2));
					md.markOrbit<FACE>(m_map.phi2(fit3));
				}
				else
				{

				}

				visitedVertices.push_back(m_map.phi1(m_map.phi1(eit)));
				visitedVertices.push_back(m_map.phi_1(m_map.phi_1(eit)));
				md.markOrbit<FACE>(eit);

			}
		}
	}
}


template <typename PFP>
void IHM2<PFP>::analysis()
{
    assert(m_map.getCurrentLevel() > 0 || !"analysis : called on level 0") ;

    m_map.decCurrentLevel() ;

    for(unsigned int i = 0; i < analysisFilters.size(); ++i)
        (*analysisFilters[i])() ;

}

template <typename PFP>
void IHM2<PFP>::synthesis()
{
    assert(m_map.getCurrentLevel() < m_map.getMaxLevel() || !"synthesis : called on max level") ;

    for(unsigned int i = 0; i < synthesisFilters.size(); ++i)
        (*synthesisFilters[i])() ;

    m_map.incCurrentLevel() ;
}

} // namespace Regular

} // namespace Primal

} // namespace MR

} // namespace Algo

} // namespace CGoGN
