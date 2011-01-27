#include "simulator.h"
#include <cstdlib>

// #ifdef HAVE_CONFIG_H
// #include "config.h"
// #endif
// 
// #if HAVE_OPENMP || _OPENMP
// #include <omp.h>
// #endif

Simulator::Simulator() : agents_(), defaultAgent_(0), globalTime_(0.0f), timeStep_(1.0f)
{
// 	CGoGN::CityGenerator::generateGrid<PFP,PFP::TVEC3>(envMap.map,envMap.position,100,100,50.0f,envMap.closeMark);

	envMap.sideSize = 70.0f;
 	CGoGN::CityGenerator::generateSmallCity<PFP,PFP::TVEC3>(envMap.map,envMap.position,envMap.closeMark,envMap.sideSize);

//	CGoGN::CityGenerator::generateAbsolutSpiralOfDeath<PFP>(envMap.map,envMap.position,envMap.closeMark,2000,0.25,110,100);


//	std::cout << "simplify" << std::endl;
// 	envMap.simplify();
 	envMap.map.init();
	std::cout << "setup scenario" << std::endl;
//	setupHelicoidScenario(1,50);
	setupScenario();

// 	Dart dStop=envMap.map.begin();
// 	for(unsigned int i=0;i<5000;++i) envMap.map.next(dStop);
// 	FunctorIsMarked fum(envMap.closeMark);
// 	path = CGoGN::PathFinder::pathFindAStar<PFP>(envMap.map,envMap.position,envMap.map.begin(),dStop,fum);
}

Simulator::Simulator(float timeStep, float neighborDist, size_t maxNeighbors, float timeHorizon, float timeHorizonObst, float radius, float maxSpeed, const VEC3& velocity) : agents_(), defaultAgent_(0), globalTime_(0.0f), timeStep_(timeStep)
{
	defaultAgent_ = new Agent(this,0);

	defaultAgent_->maxNeighbors_ = maxNeighbors;
	defaultAgent_->maxSpeed_ = maxSpeed;
	defaultAgent_->neighborDist_ = neighborDist;
	defaultAgent_->newVelocity_ = Agent::VEC_A(velocity[0],velocity[1],0);
	defaultAgent_->radius_ = radius;
	defaultAgent_->timeHorizon_ = timeHorizon;
	defaultAgent_->timeHorizonObst_ = timeHorizonObst;
	defaultAgent_->velocity_ = Agent::VEC_A(velocity[0],velocity[1],0);
}

Simulator::~Simulator()
{
	if (defaultAgent_ != 0) {
	delete defaultAgent_;
	}

	for (size_t i = 0; i < agents_.size(); ++i) {
	delete agents_[i];
	}
}

size_t Simulator::addAgent(VEC3 position)
{
	if (defaultAgent_ == 0) {
	return RVO_ERROR;
	}

	Agent* agent = new Agent(this, position,agents_.size());
	agents_.push_back(agent);

	return agents_.size() - 1;
}

size_t Simulator::addAgent(VEC3 position, Dart d)
{
	if (defaultAgent_ == 0) {
	return RVO_ERROR;
	}

	Agent* agent = new Agent(this, position,agents_.size(),d);
	agents_.push_back(agent);

	return agents_.size() - 1;
}

size_t Simulator::addAgent(VEC3 position, float neighborDist, size_t maxNeighbors, float timeHorizon, float timeHorizonObst, float radius, float maxSpeed, const VEC3& velocity)
{
	Agent* agent = new Agent(this, Agent::VEC_A(position[0],position[1],0), neighborDist, maxNeighbors, timeHorizon, timeHorizonObst, radius, Agent::VEC_A(velocity[0],velocity[1],0), maxSpeed,agents_.size());
	agents_.push_back(agent);

	return agents_.size() - 1;
}

void Simulator::doStep()
{
// 	envMap.map.check();
	if(globalTime_==0) {

	}

	for (int i = 0; i < static_cast<int>(agents_.size()); ++i) {
		envMap.linkAgentWithCells(agents_[i]);
	}

// #pragma omp parallel for
	for (int i = 0; i < static_cast<int>(agents_.size()); ++i) {
		agents_[i]->computeNewVelocity();
	}


// #pragma omp parallel for
	for (int i = 0; i < static_cast<int>(agents_.size()); ++i) {
		envMap.popAgentInCell(agents_[i],agents_[i]->part->d);
		agents_[i]->update();
		envMap.pushAgentInCell(agents_[i],agents_[i]->part->d);
		agents_[i]->treated=false;
	}

	envMap.updateMap();

	globalTime_ += timeStep_;
}

VEC3 Simulator::getAgentPrefVelocity(size_t agentNo) const
{
	return  VEC3(agents_[agentNo]->prefVelocity_[0],agents_[agentNo]->prefVelocity_[1],0);
}

void Simulator::setAgentDefaults(float neighborDist, size_t maxNeighbors, float timeHorizon, float timeHorizonObst, float radius, float maxSpeed, const VEC3& velocity)
{
	if (defaultAgent_ == 0) {
	defaultAgent_ = new Agent(this,0);
	}
	
	defaultAgent_->maxNeighbors_ = maxNeighbors;
	defaultAgent_->maxSpeed_ = maxSpeed;
	defaultAgent_->neighborDist_ = neighborDist;
	defaultAgent_->newVelocity_ = Agent::VEC_A(velocity[0],velocity[1],0);
	defaultAgent_->radius_ = radius;
	defaultAgent_->timeHorizon_ = timeHorizon;
	defaultAgent_->timeHorizonObst_ = timeHorizonObst;
	defaultAgent_->velocity_ = Agent::VEC_A(velocity[0],velocity[1],0);
}

void Simulator::setAgentPrefVelocity(size_t agentNo, const VEC3& prefVelocity)
{
	agents_[agentNo]->prefVelocity_ = Agent::VEC_A(prefVelocity[0],prefVelocity[1],0);
}

void Simulator::setTimeStep(float timeStep)
{
	timeStep_ = timeStep;
}

/** Added functions */

std::vector<PFP::VEC3> Simulator::getAgentsPosition()
{
	std::vector<PFP::VEC3> v;
	for (size_t i = 0; i < agents_.size() ; ++i) {
		VEC3 rvo_v = agents_[i]->position_;
		v.push_back(PFP::VEC3(rvo_v[0],rvo_v[1],0));
	}

	return v;
}

void Simulator::setPreferredVelocities(PFP::VEC3 posToReach)
{
  /* 
   * Set the preferred velocity to be a vector of unit magnitude (speed) in the
   * direction of the goal.
   */
// 	#pragma omp parallel for
    for (int i = 0; i < static_cast<int>(agents_.size()); ++i) {
		VEC3 goalVector = goals[i] - agents_[i]->position_;

		if (absSq(goalVector) > 1.0f) {
		goalVector = normalize(goalVector);
		}

		this->setAgentPrefVelocity(i, goalVector);
		
		/*
		* Perturb a little to avoid deadlocks due to perfect symmetry.
		*/
		float angle = std::rand() * 2.0f * M_PI / RAND_MAX;
		float dist = std::rand() * 0.0001f / RAND_MAX;
		
		this->setAgentPrefVelocity(i, this->getAgentPrefVelocity(i) +
								 VEC3(dist*std::cos(angle), dist*std::sin(angle),0.0f));
  }
}

void Simulator::setPreferredPathVelocities()
{
    for (int i = 0; i < static_cast<int>(agents_.size()); ++i) {
// 		std::cout << "i " << i << " pathPointer : " << pathPointer[i].first << " " << pathPointer[i].second << std::endl;
// 		std::cout << " point to reach : " << (*pathToFollow[pathPointer[i].first])[pathPointer[i].second] << std::endl;
		VEC3 goalVector((*pathToFollow[pathPointer[i].first])[pathPointer[i].second]);
		goalVector -= agents_[i]->position_;

		if (absSq(goalVector) > 100.0f) {
// 		goalVector = normalize(goalVector);
		}
		else {
			pathPointer[i].second = (pathPointer[i].second+1)%pathToFollow[pathPointer[i].first]->size();
			goalVector = (*pathToFollow[pathPointer[i].first])[pathPointer[i].second] - agents_[i]->position_;
// 			goalVector = normalize(goalVector);
		}

		this->setAgentPrefVelocity(i, goalVector);
		
		/*
		* Perturb a little to avoid deadlocks due to perfect symmetry.
		*/
		float angle = std::rand() * 2.0f * M_PI / RAND_MAX;
		float dist = std::rand() * 0.0001f / RAND_MAX;
		
		this->setAgentPrefVelocity(i, this->getAgentPrefVelocity(i) +
								 VEC3(dist*std::cos(angle), dist*std::sin(angle),0.0f));

  }
}

void Simulator::setupScenario()
{
  srand(0);
  /* Specify the global time step of the simulation. */
  setTimeStep(0.25f);
//   setTimeStep(1.0f);

  /* Specify the default parameters for agents that are subsequently added. */
//   setAgentDefaults(15.0f, 20, 10.0f, 10.0f, 1.5f, 2.0f);
  setAgentDefaults(15.0f, 10, 10.0f, 10.0f, 1.5f, 2.0f);

  /* 
   * Add agents, specifying their start position, and store their goals on the
   * opposite side of the environment.
   */
//    int i = 0;
	Dart d = envMap.map.begin();
	DartMarkerStore filled(envMap.map);

	bool found=false;

  for (int i = 0; i < 500 && d != envMap.map.end(); ++i) {
	found = false;
	VEC3 pos;
	Dart dCell;
	while(!found && d != envMap.map.end()) {
		VEC3 n = Algo::Geometry::triangleNormal<PFP>(envMap.map,d,envMap.position);
		if(!envMap.closeMark.isMarked(d) && !filled.isMarked(d) && envMap.position[d][2]==0.0f && n[2]==1.0f) {
			pos = envMap.faceCenter(d);
			dCell=d;
			found = true;
			filled.markOrbit(FACE_ORBIT,d);
			std::cout << "pos " << pos << std::endl;
		}
		envMap.map.next(d);
	}

	if(found) {
		float ecart=3.0f;
		addAgent(pos,dCell);
		addAgent(pos+VEC3(ecart,0,0),dCell);
		addAgent(pos+VEC3(-ecart,0,0),dCell);
		addAgent(pos+VEC3(ecart,ecart,0),dCell);
		addAgent(pos+VEC3(0,ecart,0),dCell);
		addAgent(pos+VEC3(0,-ecart,0),dCell);
		addAgent(pos+VEC3(-ecart,-ecart,0),dCell);
		addAgent(pos+VEC3(ecart,-ecart,0),dCell);
		addAgent(pos+VEC3(-ecart,ecart,0),dCell);
    	goals.push_back(-1.0f*pos);
    	goals.push_back(-1.0f*(pos+VEC3(ecart,0,0)));
		goals.push_back(-1.0f*(pos+VEC3(-ecart,0,0)));
		goals.push_back(-1.0f*(pos+VEC3(ecart,ecart,0)));
		goals.push_back(-1.0f*(pos+VEC3(0,ecart,0)));
		goals.push_back(-1.0f*(pos+VEC3(0,-ecart,0)));
		goals.push_back(-1.0f*(pos+VEC3(-ecart,-ecart,0)));
		goals.push_back(-1.0f*(pos+VEC3(ecart,-ecart,0)));
		goals.push_back(-1.0f*(pos+VEC3(-ecart,ecart,0)));
	}
  }

	std::cout << "nb of agents : " << goals.size() << std::endl;

	std::random_shuffle( goals.begin(), goals.end() );

	for (int i = 0; i < static_cast<int>(agents_.size()); ++i)
		envMap.pushAgentInCell(agents_[i],agents_[i]->part->d);

}

void Simulator::setupHelicoidScenario(float rMax,float rMin)
{
  srand(0);
  setTimeStep(0.25f);
  setAgentDefaults(15.0f, 10, 10.0f, 10.0f, 1.5f, 2.0f);

  /* 
   * Add agents, specifying their start position, and store their goals on the
   * opposite side of the environment.
   */
//    int i = 0;
	Dart d = envMap.map.begin();
	for(unsigned int i=0;i<50;++i)
		envMap.map.next(d);
	DartMarkerStore filled(envMap.map);

	bool found=false;
	int nbPack=3000;

  for (int i = 0; i < nbPack && d != envMap.map.end(); ++i) {
	found = false;
	VEC3 pos;
	Dart dCell;
	while(!found && d != envMap.map.end()) {
		if(!envMap.closeMark.isMarked(d) && !filled.isMarked(d)) {
			pos = envMap.faceCenter(d);
			if(i==nbPack/2)
				std::cout << "pos " << pos << std::endl;
			pos[2]=0.0f;
			dCell=d;
			found = true;
			filled.markOrbit(FACE_ORBIT,d);
		}
		envMap.map.next(d);
	}

	if(found) {
		float ecart=3.0f;
// 		addAgent(pos,dCell);
// 		addAgent(pos+VEC3(ecart,0,0),dCell);
// 		addAgent(pos+VEC3(-ecart,0,0),dCell);
// 		addAgent(pos+VEC3(ecart,ecart,0),dCell);
// 		addAgent(pos+VEC3(0,ecart,0),dCell);
// 		addAgent(pos+VEC3(0,-ecart,0),dCell);
// 		addAgent(pos+VEC3(-ecart,-ecart,0),dCell);
// 		addAgent(pos+VEC3(ecart,-ecart,0),dCell);
// 		addAgent(pos+VEC3(-ecart,ecart,0),dCell);

		addAgent(pos+VEC3(ecart,ecart,0),dCell);
		addAgent(pos+VEC3(-ecart,-ecart,0),dCell);
		addAgent(pos+VEC3(ecart,-ecart,0),dCell);
		addAgent(pos+VEC3(-ecart,ecart,0),dCell);

		unsigned int nbNewAgent =4;

		for(unsigned int j=0;j<nbNewAgent;++j) {
			pathPointer.push_back(std::make_pair<int,int>(i*nbNewAgent+j,0));
			pathToFollow.push_back(new std::vector<VEC3>());
			float coeff = VEC3(pos[0],pos[1],0).norm();
			float pas=16.0f;
			for(float p = 1.0f; p<=2.0f*pas ; p=p+1.0f) {
				if(i<nbPack/2)
					pathToFollow[i*nbNewAgent+j]->push_back(coeff*VEC3(sin(M_PI*p/pas),cos(M_PI*p/pas),0));
				else
					pathToFollow[i*nbNewAgent+j]->push_back(coeff*VEC3(cos(M_PI*p/pas),sin(M_PI*p/pas),0));
			}
		}
	}
  }

	std::cout << "nb of agents : " << agents_.size() << std::endl;

// 	std::random_shuffle( goals.begin(), goals.end() );

	for (int i = 0; i < static_cast<int>(agents_.size()); ++i)
		envMap.pushAgentInCell(agents_[i],agents_[i]->part->d);

}


