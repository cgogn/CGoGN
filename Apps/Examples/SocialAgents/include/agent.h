#ifndef M_AGENT_H
#define M_AGENT_H

// #include "Definitions.h"
#include <iostream>

#include "utils.h"
#include "env_map.h"
#include "Algo/MovingObjects/particle_cell_2D.h"

class Simulator;

  class Agent
  {
  public:
// 	typedef RVO::Vector2 VEC_A;
	typedef VEC3 VEC_A;

    explicit Agent(Simulator* sim, size_t agentNo);

    Agent(Simulator* sim, const VEC_A& position, size_t agentNo);

    Agent(Simulator* sim, const VEC_A& position, size_t agentNo, Dart d);

    Agent(Simulator* sim, const VEC_A& position, float neighborDist,
          size_t maxNeighbors, float timeHorizon, float timeHorizonObst,
          float radius, const VEC_A& velocity, float maxSpeed, size_t agentNo);

    ~Agent();

  	void setGoalNo(size_t goal);

    void computeNewVelocity();

    void insertAgentNeighbor(size_t agentNo, float& rangeSq);

//     void insertObstacleNeighbor(size_t obstacleNo, float rangeSq);
	 void insertObstacleNeighbor(Dart d);

    void update();

    std::vector<std::pair<float, size_t> > agentNeighbors_;
    size_t maxNeighbors_;
    float maxSpeed_;
    float neighborDist_;
//     RVO::Vector2 newVelocity_;
		VEC_A newVelocity_;
//     std::vector<std::pair<float, size_t> > obstacleNeighbors_;
		std::vector<std::pair<float, Dart> > obstacleNeighbors_;
    std::vector<Line> orcaLines_;
//     RVO::Vector2 position_;
//     RVO::Vector2 prefVelocity_;
		VEC_A position_;
		VEC_A prefVelocity_;
    float radius_;
    Simulator* sim_;
    float timeHorizon_;
    float timeHorizonObst_;
//     RVO::Vector2 velocity_;
		VEC_A velocity_;

		size_t agentNo_;
		size_t goalNo_;
		Dart nearestDart;
// 		std::vector<Dart> includingFaces;
		CGoGN::Algo::MovingObjects::ParticleCell2D<PFP> * part;
		float rangeSq;
		bool newCells;
		bool treated;
  };

  bool linearProgram1(const std::vector<Line>& lines, size_t lineNo,
                      float radius, const Agent::VEC_A& optVelocity,
                      bool directionOpt, Agent::VEC_A& result);

  bool linearProgram2(const std::vector<Line>& lines, size_t num, float radius,
                      const Agent::VEC_A& optVelocity, bool directionOpt,
                      Agent::VEC_A& result);

  void linearProgram3(const std::vector<Line>& lines, size_t numObstLines,
                      float radius, Agent::VEC_A& result);

#endif
