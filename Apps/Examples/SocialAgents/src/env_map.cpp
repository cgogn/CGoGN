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
		bool nomodif;
		do {
// 			map.check();

			nomodif=true;
			for(Dart d= map.begin() ; d!= map.end(); map.next(d)) {
				nomodif = !simplifyVertex(d) && nomodif;
			}

			for(Dart d= map.begin() ; d!= map.end(); map.next(d)) {
				simplifyEdge(d);
			}
		} while(!nomodif);
	
		map.check();
}

Dart EnvMap::getDart(unsigned int i)
{
	for(Dart d=map.begin(); d != map.end(); map.next(d)) {
		if(d==i)
			return d;
	}

	std::cout << "Dart not found" << std::endl;
	return map.end();
}

Dart EnvMap::getBelongingCell(const PFP::VEC3& pos)
{
	for(Dart d=map.begin(); d!=map.end() ; map.next(d)) {
		if(!closeMark.isMarked(d) && Algo::Geometry::isPointInConvexFace2D<PFP>(map,d,position,pos,true)) {
			return d;
		}
	}

	std::cout << "ERROR : pos not in map" << std::endl;

	return map.begin();
}

void EnvMap::insertObstacleOfFace(PFP::AGENTS agents,Dart d)
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
				insertObstacleOfFace(agentvect[d],ddd);
				addNeighborAgents(agentvect[d],agentvect[ddd]);
			}
		} while(ddd!=dd);

		dd = map.phi1(dd);
	}while(dd!=d);
}

void EnvMap::pushFaceToSubdivide(const Dart& d)
{
	for(std::list<Dart>::iterator it = filledFaces.begin(); it != filledFaces.end() ; ++it) {
		if(agentvect[d]==agentvect[*it])
			return;
	}
	filledFaces.push_back(d);
}

void EnvMap::popFaceToSubdivide(const Dart& d)
{
	for(std::list<Dart>::iterator it = filledFaces.begin(); it != filledFaces.end() ; ++it)
		if(agentvect[d]==agentvect[*it])
			filledFaces.erase(it);
}

void EnvMap::pushAgentInCell(Agent * agent, Dart d)
{
	agentvect[d].push_back(agent);
	if(agentvect[d].size()>maxNbAgentInFace) {
		pushFaceToSubdivide(d);
	}
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
// 	simplifyFaces();
	subdivideFaces() ;
	map.setCurrentLevel(map.getMaxLevel()) ;
}

void EnvMap::subdivideFaces()
{
	for(std::list<Dart>::iterator itF = filledFaces.begin(); itF != filledFaces.end(); ++itF)
	{
		Dart d = *itF;
			if(agentvect[d].size() > maxNbAgentInFace)
			{
				if(!map.faceIsSubdivided(d))
				{
					itF = filledFaces.erase(itF);

					d = map.faceOldestDart(d) ;

					std::vector<Agent*> agents ;
					agents.swap(agentvect[d]) ;
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
						pushAgentInCell(*it,(*it)->part->d);
					}
				}
			}
			else
				itF = filledFaces.erase(itF);
	}
}

void EnvMap::resetAgentInFace(Agent * agent)
{
// 	agent->part->state = VERTEX_ORBIT;
// 	agent->part->m_position = position[agent->part->d];

// 	agent->part->m_position = agent->part->pointInFace(agent->part->d);
	agent->part->m_position = Algo::Geometry::faceCentroid<PFP>(map,agent->part->d,position);
	agent->part->state = FACE_ORBIT;
	agent->part->move(agent->position_) ;
}

void EnvMap::addObstacle(std::vector<VEC3> points)
{
	std::cout << "test" << std::endl;
	std::vector<VEC3>::iterator it = points.begin();
	VEC3 p1 = *it;
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

