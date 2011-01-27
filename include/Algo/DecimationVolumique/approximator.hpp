#include "Algo/DecimationVolumique/selector.h"
#include "Algo/Geometry/centroid.h"

namespace CGoGN
{

namespace Algo
{

namespace DecimationVolumique
{

/************************************************************************************
 *							      Centroid		                                    *
 ************************************************************************************/
template <typename PFP>
void Approximator_Centroid<PFP>::init(CGoGN::Algo::DecimationVolumique::Selector<PFP>* selector)
{

}

template <typename PFP>
void Approximator_Centroid<PFP>::approximate(CGoGN::Algo::DecimationVolumique::Operator<PFP> *op)
{
	Dart d = op->getEdge();
	VEC3 a = this->m_position[d];

	std::cout << "d=" << d << " a=" << a << std::endl;

	// get some darts
	Dart dd = this->m_map.phi2(d) ;

	switch(op->getType())
	{
		case Algo::DecimationVolumique::O_CVolume : {
			a = Algo::Geometry::volumeCentroid<PFP>(this->m_map, d, this->m_position);
			break;
		}
		case Algo::DecimationVolumique::O_CFace : {
			a = Algo::Geometry::faceCentroid<PFP>(this->m_map, d, this->m_position);
			break;
		}
		case Algo::DecimationVolumique::O_CEdge : {
			// get the contracted edge vertices positions
			VEC3 v2 = this->m_position[dd] ;

			// Compute the approximated position
			a =  (a + v2) / REAL(2) ;
			break;
		}
		default : {
			std::cerr << "Approximate : reducer does not exist" << std::endl;
			break;
		}
	}

	this->m_approx = a;
}


} //end namespace DecimationVolumique
} //end namespace Algo
} //end namespace CGoGN
