namespace CGoGN
{

namespace Algo
{

namespace DecimationVolumique
{

/************************************************************************************
 *							Collapse Edge Operator	                                *
 ************************************************************************************/
template <typename PFP>
unsigned int CollapseEdgeOperator<PFP>::perform(MAP& m, typename PFP::TVEC3& position)
{
	unsigned int nbCell = 0;

	//calcul du nombre de cellule supprime
	position[this->m_edge] = this->m_approximator->getApproximation();
	nbCell = m.collapseEdge(this->m_edge,true,true);

	return nbCell;
}

template <typename PFP>
bool CollapseEdgeOperator<PFP>::canPerform(MAP &m ,Dart d, typename PFP::TVEC3& position)
{
	bool canCollapse = true;

	Dart e = d;
	do
	{
		//std::cout << "e=" << e ;
		//at border
		if(m.isBoundaryVolume(e))
		{
			canCollapse = false;
			//std::cout << " at border";
		}
		//l'un de ces 2 voisins existe
		else if(m.phi3(m.phi2(m.phi1(e))) != m.phi2(m.phi1(e)) && m.phi3(m.phi2(m.phi_1(e))) != m.phi2(m.phi_1(e)))
		{
			//l'un de ces 2 voisins est au bord
			if(m.isBoundaryVolume(m.phi3(m.phi2(m.phi1(e)))) || m.isBoundaryVolume(m.phi3(m.phi2(m.phi_1(e)))))
			{
				canCollapse = false;
				//std::cout << " neighboors  at border";
			}
		}
		else
		{
			//Edge Criteria Valide
			if(m.edgeDegree(m.phi1(m.phi2(m.phi_1(e)))) < 3)
				canCollapse = false;
			else
			{
				//Is inverted
				Dart a = m.phi3(m.phi2(m.phi1(e)));
				Dart b = m.phi1(m.phi3(m.phi2(m.phi_1(e))));

				//tetrahedre du haut
				typename PFP::VEC3 p1 = this->m_approximator->getApproximation();
				typename PFP::VEC3 p2, p3, p4;

				typename PFP::VEC3::DATA_TYPE v1;
				typename PFP::VEC3::DATA_TYPE v2;

				p2 = position[m.phi1(a)];
				p3 = position[m.phi_1(a)];
				p4 = position[m.phi_1(m.phi2(a))];

				v1 = Geom::tetraSignedVolume<typename PFP::VEC3>(p1, p2, p3, p4);
				if (v1 < 0)
					canCollapse = false;

				//std::cout << " v1 = " << v1;

				//tetrahedre du bas
				p2 = position[m.phi1(b)];
				p3 = position[m.phi_1(b)];
				p4 = position[m.phi_1(m.phi2(b))];

				v2 = Geom::tetraSignedVolume<typename PFP::VEC3>(p1, p2, p3, p4);

				if (v2 < 0)
					canCollapse = false;

				//std::cout << " v2 = " << v2;
			}
		}

		//std::cout << std::endl;

		e = m.alpha2(e);
	}while ( e != d && canCollapse);

	//std::cout << std::endl << std::endl;

	//std::cout << "is collapsable ? " << canCollapse << std::endl;


	return canCollapse;
}


} //end namespace DecimationVolumique
} //end namespace Algo
} //end namespace CGoGN
