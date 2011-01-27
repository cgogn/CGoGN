#ifndef __DECIMATORVOL_H__
#define __DECIMATORVOL_H__

#include "Algo/DecimationVolumique/selector.h"
#include "Algo/DecimationVolumique/approximator.h"
#include "Algo/DecimationVolumique/operator.h"


namespace CGoGN
{

namespace Algo
{

namespace DecimationVolumique
{

template <typename PFP>
void decimate(
	typename PFP::MAP& the_map,
	SelectorType s,
	ApproximatorType a,
	typename PFP::TVEC3& position,
	double nbWanted
);
//	const FunctorSelect& selected = SelectorTrue()
//) ;

} //namespace DecimationVolumique

} //namespace Algo

} //namespace CGoGN

#include "Algo/DecimationVolumique/decimator.hpp"

#endif
