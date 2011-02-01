#include "Algo/Geometry/centroid.h"
#include <limits>
#include <iostream>

namespace CGoGN
{

namespace PathFinder
{


template <typename PFP>
float pathCostSqr(typename PFP::MAP& map,const typename PFP::TVEC3& position, Dart startPos, Dart stopPos)
{
	return VEC3(Algo::Geometry::faceCentroid<PFP>(map,stopPos,position)-Algo::Geometry::faceCentroid<PFP>(map,startPos,position)).norm2();
}

template <typename PFP>
std::vector<Dart> pathFindAStar(typename PFP::MAP& map,const typename PFP::TVEC3& position, Dart startPos, Dart stopPos, FunctorType& bad)
{
	std::vector<Dart> path;

	AutoAttributeHandler< Dart > tablePred(map,FACE_ORBIT);
	std::vector<std::pair<float,Dart> > vToDev;

	Dart toDev = stopPos;
	vToDev.push_back(std::make_pair(0, toDev));
	tablePred[toDev] = toDev;
	do {
		//dev cell
		//get all vertex-adjacent cells 
		Dart dd = toDev;
		do {
			Dart ddd = map.alpha1(dd);
			do {
				ddd = map.alpha1(ddd);
				if(ddd!=dd) {
					if(!map.foreach_dart_of_face(ddd,bad) && tablePred[ddd]==EMBNULL) {
						//evaluate their cost and push them in the vector to dev
						if(tablePred[ddd]==EMBNULL)
							tablePred[ddd]=toDev;
						std::vector<std::pair<float,Dart> >::iterator it=vToDev.begin();
						float costDDD=pathCostSqr<PFP>(map,position,startPos,ddd)+(vToDev.begin())->first;
						while(it!=vToDev.end() && (*it).first<costDDD)
							++it;
						vToDev.insert(it, std::make_pair(costDDD, ddd));
					}
				}
			} while(ddd!=dd);
			dd = map.phi1(dd);
		} while(dd!=toDev);

		//remove developped cell and get new cell to dev
		if(vToDev.size()>0) {
			toDev = (vToDev.begin())->second;
			vToDev.erase(vToDev.begin());
		}
	} while(vToDev.size()>0 && !map.sameOrientedFace(startPos,toDev));

	//if path found : from start to stop -> push all predecessors
	if(map.sameOrientedFace(startPos,toDev))
	{
		std::cout << "found" << std::endl;
		Dart toPush=startPos;
		std::cout << tablePred[startPos] << std::endl;
		do {
			path.push_back(toPush);
			toPush = tablePred[toPush];
		} while(!map.sameOrientedFace(toPush,stopPos));
	}

	return path;
}

//namespace
}

}
