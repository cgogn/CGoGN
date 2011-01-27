#ifndef PATH_FINDER
#define PATH_FINDER

namespace CGoGN
{

namespace PathFinder
{

	template <typename PFP>
	float pathCostSqr(typename PFP::MAP& map,const typename PFP::TVEC3& position, Dart startPos, Dart stopPos);

	template <typename PFP>
	std::vector<Dart> pathFindAStar(typename PFP::MAP& map,const typename PFP::TVEC3& position, Dart startPos, Dart stopPos, FunctorSelect& bad = FunctorType());

}

}

#include "path_finder.hpp"
#endif