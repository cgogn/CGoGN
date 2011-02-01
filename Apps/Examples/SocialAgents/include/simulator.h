#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <limits>
#include <vector>

#include "utils.h"

#include "env_map.h"
#include "env_generator.h"
#include "agent.h"

#include "path_finder.h"

class Simulator
{
	public:
	Simulator();
	
	Simulator(float timeStep, float neighborDist, size_t maxNeighbors,
			float timeHorizon, float timeHorizonObst, float radius,
			float maxSpeed, const VEC3& velocity = VEC3(0,0,0));
	~Simulator();
	
	size_t addAgent(VEC3 position);

	size_t addAgent(VEC3 position, Dart d);
	
	size_t addAgent(VEC3 position, float neighborDist,
			size_t maxNeighbors, float timeHorizon,
			float timeHorizonObst, float radius, float maxSpeed,
			const VEC3& velocity = VEC3());
	
	void doStep();
	VEC3 getAgentPrefVelocity(size_t agentNo) const;
	
	float getGlobalTime() const;
	
	void setAgentDefaults(float neighborDist, size_t maxNeighbors,
				float timeHorizon, float timeHorizonObst,
				float radius, float maxSpeed, 
				const VEC3& velocity = VEC3());
	
	void setAgentPrefVelocity(size_t agentNo, const VEC3& prefVelocity);
	
	void setTimeStep(float timeStep);
	
	std::vector<Agent*> agents_;
	Agent* defaultAgent_;
	float globalTime_;
	EnvMap envMap;
	float timeStep_;
	
		std::vector<PFP::VEC3 > socialAgents;

		void setPreferredNextCellVelocities();

// 		std::vector<Dart> path;
	
		/** Added function */
		std::vector<PFP::VEC3> getAgentsPosition();

		//just one goal
		std::vector<PFP::VEC3> goals;

		void setPreferredVelocities(PFP::VEC3 posToReach);

		//for a path of goals
		std::vector<std::pair<int,int> > pathPointer;
		std::vector<std::vector<PFP::VEC3> * > pathToFollow;

		void setPreferredPathVelocities();

		//scenarii
		void setupScenario();
		void setupTobogganScenario(float rMax,float rMin);
		void setupHelicoidScenario(float rMax,float rMin);
};

#endif
