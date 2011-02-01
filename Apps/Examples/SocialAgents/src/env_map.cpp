#include "env_map.h"
#include "Geometry/inclusion.h"
#include "utils.h"
#include "agent.h"
#include "Algo/MovingObjects/particle_cell_2D_memo.h"

#include "Algo/Modelisation/subdivision.h"
#include "Algo/Geometry/normal.h"

using namespace CGoGN;

EnvMap::EnvMap() : closeMark(map)
{
	position = map.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "position");
	normal = map.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "normals");
	agentvect = map.addAttribute<PFP::AGENTVECT>(FACE_ORBIT, "agent");

	dSimplify = Dart(0) ; // map.begin();
	dSubdivide = Dart(0) ; // map.begin();
}

bool EnvMap::simplifiableDart(Dart d)
{
	VEC3 p1 = position[map.phi_1(map.phi2(d))];
	VEC3 p2 = position[map.phi1(d)];
	VEC3 p3 = position[map.phi1(map.phi1(d))];

	if(Geom::testOrientation2D<PFP::VEC3>(p2,p1,p3)!=Geom::LEFT) {
		return true;
	}

	return false;
}

bool EnvMap::simplifiable(Dart d)
{
	if(!closeMark.isMarked(map.phi2(d)) && position[d][2]==0.0f) {
		VEC3 n1 =  Algo::Geometry::triangleNormal<PFP>(map,d,position);
		VEC3 n2 =  Algo::Geometry::triangleNormal<PFP>(map,map.phi2(d),position);
		if(n1[0]==0.0f && n1[1]==0.0f && n1[2]==1.0f && n1[0]==n2[0] && n1[0]==n2[0] && n1[0]==n2[0]) {
			Dart d2 = map.phi2(d);
			VEC3 p1 = position[map.phi_1(d)];
			VEC3 p2 = position[d];

			VEC3 p3 = position[d2];
			VEC3 p4 = position[map.phi_1(d2)];

			VEC3 p5 = position[map.phi1(map.phi1(d))];
			VEC3 p6 = position[map.phi1(map.phi1(d2))];
			if(
				(p1[0]!=p6[0] || p1[1]!=p6[1] || p1[2]!=p6[2]) && (p5[0]!=p4[0] || p5[1]!=p4[1] || p5[2]!=p4[2]) &&
				 Geom::testOrientation2D<PFP::VEC3>(p6,p1,p2)!=Geom::RIGHT
				&& Geom::testOrientation2D<PFP::VEC3>(p5,p4,p3)!=Geom::RIGHT
			)
				return true;
		}
	}

	return false;
}

unsigned int EnvMap::getNbFaces()
{
	unsigned int count=0;
	for(unsigned int it = agentvect.begin();  it != agentvect.end() ; agentvect.next(it))
		++count;

	return count;	
}

bool EnvMap::noAgentSurroundingVertex(Dart d)
{
  Dart dd = d;
  do {
     if(agentvect[dd].size()>0)
	return false;
     dd = map.alpha1(dd);
  } while(dd!=d);
  
  return true;
}

bool EnvMap::simplifyVertex(Dart& d)
{

	bool simplifOk=true;
	Dart dd = d;
	do {
		simplifOk = simplifiable(dd);
		dd = map.alpha1(dd);
	} while(simplifOk && dd!=d);

	if(simplifOk) {
		Dart d2 = map.phi2(map.phi1(dd));
		if(d2!=map.phi2(d2)) {
			bool finished=false;
			do {
				Dart d1 = map.alpha1(dd);
				if (d1==dd)			// last edge is pending edge inside of face
					finished=true;
				map.deleteFace(dd);
				dd = d1;
			} while (!finished);

			map.closeHole(d2);

			return true;
		}
	}

	return false;
}

bool EnvMap::collapsableEdge(Dart& d,Dart& dStop)
{
	bool simplification;
	bool simplifiable=false;

	if(map.alpha1(map.alpha1(d))!=d) {
		Dart dd=d;
		do {
			simplification=false;
			dd = map.phi1(dd);
			if(map.alpha1(map.alpha1(dd))==dd) {
				bool obstacle=closeMark.isMarked(dd) || closeMark.isMarked(map.phi2(dd));

				if(!obstacle) {
						simplification=true;
						simplifiable=true;
				}
				else
					simplifiable=false;
			}
		} while(simplification);

		dStop = dd;
	}

	return simplifiable;
}

bool EnvMap::simplifyEdge(Dart& d)
{
	Dart dNext;
	if(collapsableEdge(d,dNext) && simplifiableDart(map.phi2(d)) && simplifiableDart(map.phi_1(dNext))) {
		Dart dd=map.phi1(d);
		VEC3 nextPos;
		while(map.phi1(dd)!=dNext) {
			map.collapseEdge(dd);
			dd=map.phi1(d);
		}
		nextPos = position[map.phi1(dd)];
		map.collapseEdge(dd);
		position[map.phi1(d)] = nextPos;

		map.mergeFaces(d);
		return true;
	}
	return false;
}

void EnvMap::simplify()
{

// 	for(unsigned int i=0;i<10;++i) {
		bool nomodif;
		do {
// 			map.check();

			nomodif=true;
			for(Dart d= map.begin() ; d!= map.end(); map.next(d)) {
// 				std::cout << "dart " << d << std::endl;
// 				simplifyVertex(d);
				nomodif = !simplifyVertex(d) && nomodif;
			}

			for(Dart d= map.begin() ; d!= map.end(); map.next(d)) {
				simplifyEdge(d);
			}
		} while(!nomodif);
	
		map.check();

// 		for(Dart d= map.begin() ; d!= map.end(); map.next(d)) {
// 			if(simplifiable(d)) {
// 				map.mergeFaces(d);
// 			}
// 		}
// 		std::cout << "i" << i << std::endl;
// 	}
}

Dart EnvMap::getDart(unsigned int i)
{
	Dart d;
	for(d=map.begin(); d !=map.end() ; map.next(d)) {
		if(d==i)
			return d;
	}

	std::cout << "Dart not found" << std::endl;
	return d;
}

Dart EnvMap::getBelongingCell(const PFP::VEC3& pos)
{
	Dart d;
	for(d=map.begin(); d!=map.end() ; map.next(d)) {
		if(!closeMark.isMarked(d) && Algo::Geometry::isPointInConvexFace2D<PFP>(map,d,position,pos,true)) {
			return d;
		}
	}

	std::cout << "pos not in map" << std::endl;

	return map.begin();
}

void EnvMap::insertObstacleOfFace(PFP::AGENTS agents,const Dart d)
{
	Dart dd =d;
	do {
		if(closeMark.isMarked(dd) /*&& (position[map.phi2(dd)][2]==0.0 || position[map.phi1(map.phi2(dd))][2] ==0.0f)*/) {
			for(PFP::AGENTS::iterator it=agents.begin(); it!= agents.end(); ++it) {
// 				if(leftOf((*it)->part->m_position,position[dd],position[map.phi1(dd)])<0.0f) {
					(*it)->insertObstacleNeighbor(dd);
// 				}
			}
//			return;
		}
		dd = map.phi1(dd);
	} while (dd!=d);
}

void EnvMap::getAllFacesOfAgents(Dart d)
{
	for(PFP::AGENTS::iterator it=agentvect[d].begin(); it!= agentvect[d].end(); ++it) {
		(*it)->treated=true;
		(*it)->obstacleNeighbors_.clear();
		(*it)->agentNeighbors_.clear();
	}

	insertObstacleOfFace(agentvect[d],d);
	addNeighborAgents(agentvect[d],agentvect[d]);

	Dart dd = d;
	do {
		Dart ddd = map.alpha1(dd);
		do {
			ddd = map.alpha1(ddd);
			if(ddd!=dd) {
// 				if(!closeMark.isMarked(dd))
					insertObstacleOfFace(agentvect[d],ddd);
				addNeighborAgents(agentvect[d],agentvect[ddd]);
			}
		} while(ddd!=dd);

		dd = map.phi1(dd);
	}while(dd!=d);



// 	pushAgentInCell(agent,d);
// 	l_contact_Dart.push_back(d);
// 	cm.mark(d);

// 	insertObstacleOfFace(agent,d);
// 
// 	Dart dd = d;	
// 	do {
// 		Dart ddd = map.alpha1(dd);
// 		do {
// 			ddd = map.alpha1(ddd);
// 			if(ddd!=dd) {
// // 				pushAgentInCell(agent,ddd);
// // 				l_contact_Dart.push_back(d);
// // 				cm.mark(ddd);
// 				insertObstacleOfFace(agent,ddd);
// 			}
// 		} while(ddd!=dd);
// 
// 		dd = map.phi1(dd);
// 	}while(dd!=d);

// 	pushAgentInCell(agent,d,l_contact_Dart);
// 
// 	insertObstacleOfFace(agent,d);
// 
// 	Dart dd = d;	
// 	do {
// 		Dart ddd = map.alpha1(dd);
// 		do {
// 			ddd = map.alpha1(ddd);
// 			if(ddd!=dd) {
// 				pushAgentInCell(agent,ddd,l_contact_Dart);
// 				insertObstacleOfFace(agent,ddd);
// 			}
// 		} while(ddd!=dd);
// 
// 		dd = map.phi1(dd);
// 	}while(dd!=d);
}

void EnvMap::pushAgentInCell(Agent * agent, Dart d)
{
	agentvect[d].push_back(agent);
}

void EnvMap::popAgentInCell(Agent * agent, Dart d)
{
	for(PFP::AGENTVECT::iterator it = agentvect[d].begin(); it != agentvect[d].end() ; ++it)
		if(*it==agent) {
			agentvect[d].erase(it);
			return;
		}

	std::cout << "ERROR : cannot popAgentInCell" << std::endl;
}


void EnvMap::linkAgentWithCells(Agent* agent)
{
	if(!agent->treated) {
		getAllFacesOfAgents(agent->part->d);
	}
}

void EnvMap::addNeighborAgents(PFP::AGENTS agentsFrom,PFP::AGENTS agentsTo)
{
	for(PFP::AGENTS::iterator itA = agentsFrom.begin() ; itA != agentsFrom.end() ; ++itA) {
		float range= sqr((*itA)->neighborDist_);
		for(PFP::AGENTS::iterator itB = agentsTo.begin() ; itB != agentsTo.end() ; ++itB) {
			size_t neighbour = (*itB)->agentNo_;
			(*itA)->insertAgentNeighbor(neighbour,range);
		}
	}
}

void EnvMap::updateMap()
{
 	simplifyFaces() ;
	subdivideFaces() ;
	map.setCurrentLevel(map.getMaxLevel()) ;
}

void EnvMap::subdivideFaces()
{
	CellMarker m(map, FACE_CELL) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(!m.isMarked(d))
		{
			m.mark(d) ;
			if(!closeMark.isMarked(d) && agentvect[d].size() > 3)
			{
				if(!map.faceIsSubdivided(d))
				{
					d = map.faceOldestDart(d) ;

					std::vector<Agent*> agents ;
					agents.swap(agentvect[d]) ;

					unsigned int cur = map.getCurrentLevel() ;
					unsigned int fLevel = map.faceLevel(d) ;

					map.setCurrentLevel(fLevel) ;
					std::vector<Dart> marked ;
					Dart fit = d ;
					do
					{
						if(closeMark.isMarked(map.phi2(fit)))
							marked.push_back(fit) ;
						fit = map.phi1(fit) ;
					} while(fit != d) ;

					Algo::IHM::subdivideFace<PFP>(map, d, position) ;

					map.setCurrentLevel(fLevel + 1) ;
					for(std::vector<Dart>::iterator it = marked.begin(); it != marked.end(); ++it)
						closeMark.mark(map.phi2(*it)) ;

					map.setCurrentLevel(map.getMaxLevel()) ;
					for(PFP::AGENTS::iterator it = agents.begin(); it != agents.end(); ++it)
					{
						resetAgentInFace(*it) ;
						agentvect[(*it)->part->d].push_back(*it) ;
					}

					map.setCurrentLevel(cur) ;
				}
			}
		}
	}

//	for(unsigned int i=0;i<8;++i) {
//		if(/*!fm.isMarked(d) && */!closeMark.isMarked(dSubdivide) && !closeMark.isMarked(map.phi2(dSubdivide))) {
//			VEC3 n = Algo::Geometry::triangleNormal<PFP>(map,dSubdivide,position);
//			if(n[2]==1.0f) {
//				if(checkForSubdivision(dSubdivide)) {
//					map.next(dSubdivide);
//					if(dSubdivide==map.end()) {
//						dSubdivide = map.begin();
//					}
//					return;
//				}
//			}
//		}
//		map.next(dSubdivide);
//
//		if(dSubdivide==map.end()) {
//			dSubdivide = map.begin();
//		}
//	}
}

void EnvMap::simplifyFaces()
{
	assert(map.getCurrentLevel() == map.getMaxLevel()) ;
	unsigned int cur = map.getCurrentLevel() ;

	if(cur == 0)
		return ;

	CellMarker m(map, FACE_CELL) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(!m.isMarked(d))
		{
			m.mark(d) ;
			unsigned int fLevel = map.faceLevel(d) ;
			if(!closeMark.isMarked(d) && fLevel > 0)
			{
				Dart old = map.faceOldestDart(d) ;
				map.setCurrentLevel(cur - 1) ;
				Dart fit = old ;
				do
				{
					map.setCurrentLevel(cur) ;

					map.setCurrentLevel(cur - 1) ;
					fit = phi1(fit) ;
				} while(fit != d) ;
				map.setCurrentLevel(cur) ;
			}
		}
	}

//// 	for(Dart d = map.begin(); d!= map.end() ; map.next(d)) {
//	for(unsigned int i=0;i<2;++i) {
//// 	std::cout << "subdividedDarts size " << subdividedDarts.size() << std::endl;
//// 	for(std::vector<Dart>::iterator it = subdividedDarts.begin(); it != subdividedDarts.end() ; ++it) {
//// 	      Dart d=*it;
//// 	      if(map.phi1(d)!=map.phi_1(d))
//// 	      do {
//// 		Dart dNext = dSimplify;
//// 		map.next(dNext);
//
//		if(!closeMark.isMarked(dSimplify) && !closeMark.isMarked(map.phi2(dSimplify)) && agentvect[dSimplify].size()==0) {
//// 			if(noAgentSurroundingVertex(dSimplify) && simplifyVertex(dSimplify)) {
//// 				std::cout << "erase some darts 1" << std::endl;
//// // 				subdividedDarts.erase(it);
//// 				map.next(dSimplify);
//// 				return;
//// 			}
//// 			else 	{
//					if(agentvect[map.phi2(dSimplify)].size()==0) {
//						if(simplifyEdge(dSimplify)) {
//							std::cout << "erase some darts 2" << std::endl;
//// 							subdividedDarts.erase(it);
//// 							dSimplify = dNext;
//							map.next(dSimplify);
//							return;
//						}
//						else if(simplifiable(dSimplify)) {
//							std::cout << "erase some darts 3" << std::endl;
//							map.mergeFaces(dSimplify);
//// 							subdividedDarts.erase(it);
//// 							dSimplify = dNext;
//							map.next(dSimplify);
//							return;
//						}
//					}
//// 			}
//		}
//// 		 d=map.phi1(d);
//// 	      }while(d!=*it);
//		map.next(dSimplify);
//// 		dSimplify = dNext;
//
//		if(dSimplify==map.end()) {
//			dSimplify = map.begin();
//		}
//	}
}

bool EnvMap::checkForSubdivision(Dart d)
{
	if(agentvect[d].size() > 10) {
// 		if(map.phi<1111>(d)==d) {
// 			if(!closeMark.isMarked(d) && subdivideSquare(d)) {
// // 				map.check();
// // 				std::cout << "subdivided square" << std::endl;
// 				for(PFP::AGENTS::iterator it  = agentvect[d].begin() ;it != agentvect[d].end() ; ++it ) {
// 					resetAgentInFace(*it,d);
// 					Dart dd = map.phi2(map.phi1(d));
// 					Dart ddd=dd;
// 					do {
// // 						std::cout << "push 4 times" << std::endl;
// 						pushAgentInCell(*it,ddd,(*it)->includingFaces);
// 						ddd = map.alpha1(ddd);
// 					} while(dd!=map.alpha1(ddd));
// 				}
// // 				std::cout << "ok" << std::endl;
// 				return true;
// 			}
// 		}
// 		else
// 		{
			Dart dd=d;
			bool found=false;
			do {
				if(Geom::testOrientation2D<PFP::VEC3>(position[map.phi_1(dd)],position[dd],position[map.phi1(dd)])==Geom::ALIGNED && (position[map.phi1(dd)][0]!= position[dd][0] || position[map.phi1(dd)][1]!= position[dd][1]))
					found = true;
				else
					dd = map.phi1(dd);
			} while(!found && dd!=d);

			if(found && subdivideFace(dd)) {
	// 			map.check();
	// 		if(found && !map.faceIsSubdivided(d)) {
	// 			Algo::IHM::subdivideFace<PFP>(map,d,position);
	// 			std::cout << "replace agents : " << agentvect[dd].size() << std::endl;
				for(PFP::AGENTS::iterator it  = agentvect[dd].begin() ;it != agentvect[dd].end() ; ++it ) {
	// 				std::cout << "agent " << (*it)->agentNo_ << std::endl;
					resetAgentInFace(*it);
					pushAgentInCell(*it,map.alpha1(dd));
				}

				return true;
			}
// 		}
	}

	return false;
}

bool EnvMap::subdivideSquare(Dart d)
{
	std::cout << "subdivide square" << std::endl;
	VEC3 c;
	Dart dd=d;
	do {
		VEC3 mid(position[dd]+position[map.phi1(dd)]);
		mid *= 0.5f;
		map.cutEdge(dd);
		if(closeMark.isMarked(map.phi2(dd)))
			closeMark.mark(map.phi2(map.phi1(dd)));
		position[map.phi1(dd)] = mid;
		c +=mid;
		dd = map.phi1(map.phi1(dd));
	} while(dd!=d);
	c *= 0.25f;

	position[Algo::Modelisation::quadranguleFace<PFP>(map,dd)] = c;

	return true;
}

bool EnvMap::subdivideFace(Dart d)
{
	int coordChg = 0;
	int phiCount=0;

// 	std::cout << "start " << std::endl;
	VEC3 posD = position[d];

	if(posD[1]!=position[map.phi1(d)][1])
		coordChg = 1;
	
	if(fabs(position[map.phi1(d)][coordChg]-posD[coordChg])<sideSize)
		return false;

	Dart dd=d;
	do {
		dd=map.phi1(dd);
		phiCount++;
// 		std::cout << " -pos dd " << position[dd] << std::endl;
	} while(position[map.phi1(dd)][coordChg]!=position[dd][coordChg]);

	coordChg = (coordChg+1)%2;
	while(position[map.phi1(dd)][coordChg]!=position[dd][coordChg]) { 
		dd=map.phi1(dd);
// 		std::cout << " +pos dd " << position[dd] << std::endl;
	} 

	coordChg = (coordChg+1)%2;
	do {
		dd=map.phi1(dd);
// 		std::cout << " *pos dd " << position[dd] << std::endl;
	} while(dd!=d && posD[coordChg]!=position[dd][coordChg]);

// 	std::cout << "phiCount " << phiCount << std::endl;

	coordChg = (coordChg+1)%2;

	if(d!=dd && Geom::testOrientation2D<PFP::VEC3>(position[map.phi_1(dd)],position[dd],position[map.phi1(dd)])==Geom::ALIGNED) {
// 	  std::cout << " pos d " << position[d] << " dd " << position[dd] << std::endl;
//   	 std::cout << "  d " << d << " dd " << dd << std::endl;
// 	  std::cout << " coord " << coordChg << std::endl;
	  map.splitFace(d,dd);
	  
	  int nbCut = (position[d][coordChg]-position[dd][coordChg])/sideSize;
	  if(nbCut>0)
		  nbCut--;
	  else
		  nbCut++;

	  while(abs(nbCut)>0) {
		  map.cutEdge(map.phi_1(d));
		  position[map.phi_1(d)] = position[d];
		  position[map.phi_1(d)][coordChg] -= nbCut*this->sideSize;

// 		 std::cout << " cutEdge : " << position[map.phi_1(d)] << std::endl;

		  if(nbCut>0)
			  nbCut--;
		  else
			  nbCut++;
	  }

	  return true;
	}
	
	return false;
}

VEC3 EnvMap::faceCenter(Dart d)
{
	VEC3 center(0,0,0);
	unsigned int count = 0 ;
	Dart it = d ;
	do
	{
		center += position[it];
		++count ;
// 		while(map.phi1(it)!=d && Geom::testOrientation2D<PFP::VEC3>(position[map.phi1(d)],position[d],position[map.phi1(map.phi1(d))])==Geom::ALIGNED)
// 			it=map.phi1(it);

		it = map.phi1(it) ;
	} while(it != d) ;

// 	if(count<4) {
// 		std::cout << "??????" << count << std::endl;
// 	}

	center /= double(count);
	return center ;
}

void EnvMap::resetAgentInFace(Agent * agent)
{
// 	agent->part->state = VERTEX_ORBIT;
// 	agent->part->m_position = position[agent->part->d];

// 	agent->part->m_position = agent->part->pointInFace(agent->part->d);
	agent->part->m_position = Algo::Geometry::faceCentroid<PFP>(map,agent->part->d,position);
	agent->part->state = FACE_ORBIT;
	agent->part->move(agent->position_) ;
// 	agent->part->d = d;
}

void EnvMap::addObstacle(std::vector<VEC3> points)
{
	std::cout << "test" << std::endl;
	std::vector<VEC3>::iterator it = points.begin();
	VEC3 p1 = *it;
// 	std::cout << "p1 " << p1 << std::endl;
	it++;
	VEC3 p2;
	Dart d = getBelongingCell(p1);
	closeMark.markOrbit(FACE_ORBIT,d);
	Algo::MovingObjects::ParticleCell2DMemo<PFP> part(map,d,p1,position,closeMark);
// 	Algo::MovingObjects::ParticleCell2D<PFP> part(map,d,p1,position,closeMark);
	do {
		p2 = *it;

		part.move(p2);
		d = part.d;
		for(std::list<Dart>::iterator toMark = part.memo_cross.begin();toMark != part.memo_cross.end(); ++toMark)
			closeMark.markOrbit(FACE_ORBIT,*toMark);
		part.memo_cross.clear();
// 		map.markOrbit(FACE_ORBIT,d,closeMark);

		std::cout << "p1 " << p1 << "p2 " << p2 << std::endl;
		it++;
		p1 = p2;

	} while(it!=points.end());

	p2 = *(points.begin());
	std::cout << "p1" << p1 << "p2 " << p2 << std::endl;
	part.move(p2);
// 	d = part.d;
// 	map.markOrbit(FACE_ORBIT,d,closeMark);
	for(std::list<Dart>::iterator toMark = part.memo_cross.begin();toMark != part.memo_cross.end(); ++toMark)
		closeMark.markOrbit(FACE_ORBIT,*toMark);
}

