#include "agent.h"
#include "simulator.h"

  Agent::Agent(Simulator* sim, size_t agentNo) : agentNeighbors_(), maxNeighbors_(0), maxSpeed_(0.0f), neighborDist_(0.0f), newVelocity_(), obstacleNeighbors_(), orcaLines_(), position_(), prefVelocity_(), radius_(0.0f), sim_(sim), timeHorizon_(0.0f), timeHorizonObst_(0.0f), velocity_(), agentNo_(agentNo), goalNo_(agentNo), newCells(false), treated(false)
  {
	rangeSq = sqr(timeHorizonObst_ * maxSpeed_ + radius_);
  }

  Agent::Agent(Simulator* sim, const VEC_A& position, size_t agentNo) : agentNeighbors_(), maxNeighbors_(sim->defaultAgent_->maxNeighbors_), maxSpeed_(sim->defaultAgent_->maxSpeed_), neighborDist_(sim->defaultAgent_->neighborDist_), newVelocity_(sim->defaultAgent_->velocity_), obstacleNeighbors_(), orcaLines_(), position_(position), prefVelocity_(), radius_(sim->defaultAgent_->radius_), sim_(sim), timeHorizon_(sim->defaultAgent_->timeHorizon_), timeHorizonObst_(sim->defaultAgent_->timeHorizonObst_), velocity_(sim->defaultAgent_->velocity_), agentNo_(agentNo), goalNo_(agentNo), newCells(false), treated(false)
  {
	nearestDart = sim->envMap.getBelongingCell(VEC3(position[0],position[1],0));
	part = new Algo::MovingObjects::ParticleCell2D<PFP>(sim->envMap.map,nearestDart,position,sim->envMap.position,sim->envMap.closeMark);
	rangeSq = sqr(timeHorizonObst_ * maxSpeed_ + radius_);
  }

  Agent::Agent(Simulator* sim, const VEC_A& position, size_t agentNo, Dart d) : agentNeighbors_(), maxNeighbors_(sim->defaultAgent_->maxNeighbors_), maxSpeed_(sim->defaultAgent_->maxSpeed_), neighborDist_(sim->defaultAgent_->neighborDist_), newVelocity_(sim->defaultAgent_->velocity_), obstacleNeighbors_(), orcaLines_(), position_(position), prefVelocity_(), radius_(sim->defaultAgent_->radius_), sim_(sim), timeHorizon_(sim->defaultAgent_->timeHorizon_), timeHorizonObst_(sim->defaultAgent_->timeHorizonObst_), velocity_(sim->defaultAgent_->velocity_), agentNo_(agentNo), goalNo_(agentNo), newCells(false), treated(false)
  {
	nearestDart = d;
	part = new Algo::MovingObjects::ParticleCell2D<PFP>(sim->envMap.map,nearestDart,position,sim->envMap.position,sim->envMap.closeMark);
	rangeSq = sqr(timeHorizonObst_ * maxSpeed_ + radius_);
  }

  Agent::Agent(Simulator* sim, const VEC_A& position, float neighborDist, size_t maxNeighbors, float timeHorizon, float timeHorizonObst, float radius, const VEC_A& velocity, float maxSpeed, size_t agentNo) : agentNeighbors_(), maxNeighbors_(maxNeighbors), maxSpeed_(maxSpeed), neighborDist_(neighborDist), newVelocity_(velocity), obstacleNeighbors_(), orcaLines_(), position_(position), prefVelocity_(), radius_(radius), sim_(sim), timeHorizon_(timeHorizon), timeHorizonObst_(timeHorizonObst), velocity_(velocity), agentNo_(agentNo), goalNo_(agentNo), newCells(false), treated(false)
  {
	nearestDart = sim->envMap.getBelongingCell(VEC3(position[0],position[1],0));
	part = new Algo::MovingObjects::ParticleCell2D<PFP>(sim->envMap.map,nearestDart,position,sim->envMap.position,sim->envMap.closeMark);
	rangeSq = sqr(timeHorizonObst_ * maxSpeed_ + radius_);
  }

  Agent::~Agent()
  {
  }

void Agent::setGoalNo(size_t goal)
{
	goalNo_ = goal;
}

  /* Search for the best new velocity. */
void Agent::computeNewVelocity()
{
	orcaLines_.clear();

	/* Create obstacle ORCA lines. */
	for (size_t i = 0; i < obstacleNeighbors_.size(); ++i) {
	float invTimeHorizonObst = 1.0f / timeHorizonObst_;

// // 	const Obstacle* obstacle1 = sim_->obstacles_[obstacleNeighbors_[i].second];
// // 	const Obstacle* obstacle2 = sim_->obstacles_[obstacle1->nextObstacleNo_];
	Dart obst1 = obstacleNeighbors_[i].second;
	Dart obst2 = sim_->envMap.map.phi1(obst1);

	VEC3 obst1Pos = sim_->envMap.position[obst1];
	VEC3 obst2Pos = sim_->envMap.position[obst2];

	const VEC3 relativePosition1(obst1Pos - position_);
	const VEC3 relativePosition2(obst2Pos - position_);
	const VEC3 obstacleVector(obst2Pos - obst1Pos);

	/*
	* Check if velocity obstacle of obstacle is already taken care of by
	* previously constructed obstacle ORCA lines.
	*/
	bool alreadyCovered = false;

	for (size_t j = 0; j < orcaLines_.size(); ++j) {
		if (det2D(invTimeHorizonObst * relativePosition1 - orcaLines_[j].point, orcaLines_[j].direction) - invTimeHorizonObst * radius_ >= -RVO_EPSILON && det2D(invTimeHorizonObst * relativePosition2 - orcaLines_[j].point, orcaLines_[j].direction) - invTimeHorizonObst * radius_ >=  -RVO_EPSILON) {
// 		std::cout << "already covered (?) " << obst1 << std::endl;
		alreadyCovered = true;
		break;
		}
	}

	if (alreadyCovered) {
		continue;
	}

// 	std::cout << "MUST AVOIIID OBSTAAACLE " << obst1 << std::endl;

	/*
	* Compute legs. When obliquely viewed, both legs can come from a single
	* vertex. Legs can never point into neighboring edge when convex vertex,
	* take cutoff-line of neighboring edge instead. If velocity projected on
	* "foreign" leg, no constraint is added. Legs extend cut-off line when
	* nonconvex vertex.
	*/
	const float distSq1 = absSq(relativePosition1);
	const float distSq2 = absSq(relativePosition2);

	const float radiusSq = sqr(radius_);

	VEC3 leftLegDirection, rightLegDirection;

	const float s = (-1.0f*relativePosition1 * obstacleVector) / absSq(obstacleVector);
	const float distSqLine = absSq(-1.0f*relativePosition1 - s * obstacleVector);

	if (s < 0 && distSq1 <= radiusSq) {
		/* Collision with left vertex. */

		invTimeHorizonObst = 1.0f / sim_->timeStep_;
		obst2 = obst1;
		leftLegDirection = normalize(VEC3(-1.0f*relativePosition1[1], relativePosition1[0],0));
		rightLegDirection = -1.0f*leftLegDirection;
	} else if (s > 1 && distSq2 <= radiusSq) {
		/* Collision with right vertex. */

		invTimeHorizonObst = 1.0f / sim_->timeStep_;
		obst1 = obst2;
		leftLegDirection = normalize(VEC3(-1.0*relativePosition2[1], relativePosition2[0],0));
		rightLegDirection = -1.0f*leftLegDirection;
	} else if (s >= 0 && s < 1 && distSqLine <= radiusSq) {
		/* Collision with obstacle segment. */
		invTimeHorizonObst = 1.0f / sim_->timeStep_;
		leftLegDirection = -1.0f*normalize(obstacleVector);
		rightLegDirection = -1.0f*leftLegDirection;
	} else if (s < 0 && distSqLine <= radiusSq) {
		/*
		* No collision, but obstacle viewed obliquely so that left vertex
		* defines velocity obstacle.
		*/

		obst2 = obst1;

		const float leg1 = std::sqrt(distSq1 - radiusSq);

		leftLegDirection = VEC3(relativePosition1[0] * leg1 - relativePosition1[1] * radius_, relativePosition1[0] * radius_ + relativePosition1[1] * leg1,0) / distSq1;
		rightLegDirection = VEC3(relativePosition1[0] * leg1 + relativePosition1[1] * radius_, -relativePosition1[0] * radius_ + relativePosition1[1] * leg1,0) / distSq1;
	} else if (s > 1 && distSqLine <= radiusSq) {
		/*
		* No collision, but obstacle viewed obliquely so that
		* right vertex defines velocity obstacle.
		*/

		obst1 = obst2;


		const float leg2 = std::sqrt(distSq2 - radiusSq);

		leftLegDirection = VEC3(relativePosition2[0] * leg2 - relativePosition2[1] * radius_, relativePosition2[0] * radius_ + relativePosition2[1] * leg2,0) / distSq2;
		rightLegDirection = VEC3(relativePosition2[0] * leg2 + relativePosition2[1] * radius_, -relativePosition2[0] * radius_ + relativePosition2[1] * leg2,0) / distSq2;
	} else {
		/* Usual situation. */
// 		std::cout << "usual (?)" << std::endl;

		const float leg1 = std::sqrt(distSq1 - radiusSq);
		leftLegDirection = VEC3(relativePosition1[0] * leg1 - relativePosition1[1] * radius_, relativePosition1[0] * radius_ + relativePosition1[1] * leg1,0) / distSq1;

		const float leg2 = std::sqrt(distSq2 - radiusSq);
		rightLegDirection = VEC3(relativePosition2[0] * leg2 + relativePosition2[1] * radius_, -relativePosition2[0] * radius_ + relativePosition2[1] * leg2,0) / distSq2;
	}

// 	const Obstacle* const leftNeighbor = sim_->obstacles_[obstacle1->prevObstacleNo_];
	Dart leftObst1 = sim_->envMap.map.phi_1(obst1);
	VEC3 leftObstPos = sim_->envMap.position[leftObst1];

// 	const Obstacle* const rightNeighbor = sim_->obstacles_[obstacle2->nextObstacleNo_];
	Dart rightObst2 = sim_->envMap.map.phi1(obst2);
	VEC3 rightObstPos = sim_->envMap.position[rightObst2];

	bool isLeftLegForeign = false;
	bool isRightLegForeign = false;

	if (/*obstacle1->isConvex_ && */det2D(leftLegDirection, leftObstPos - obst1Pos) > 0.0f) {
		/* Leg points into obstacle. */
		leftLegDirection = normalize(leftObstPos - obst1Pos);
		isLeftLegForeign = true;
	}

	if (/*obstacle2->isConvex_ &&*/ det2D(rightLegDirection, rightObstPos - obst2Pos) < 0.0f) {
		/* Leg points into obstacle. */
		rightLegDirection = normalize(rightObstPos - obst2Pos);
		isRightLegForeign = true;
	}

	/* Compute cut-off centers. */
	const VEC3 leftCutoff = invTimeHorizonObst * (obst1Pos - position_);
	const VEC3 rightCutoff = invTimeHorizonObst * (obst2Pos - position_);
	const VEC3 cutoffVec = rightCutoff - leftCutoff;

	/* Project current velocity on velocity obstacle. */
	Line line;

	const float t = (obst1 == obst2 ? 0.5f : ((velocity_ - leftCutoff) * cutoffVec) / absSq(cutoffVec));
	const float tLeft = ((velocity_ - leftCutoff) * leftLegDirection);
	const float tRight = ((velocity_ - rightCutoff) * rightLegDirection);

	if ((t < 0.0f && tLeft < 0.0f) || (obst1 == obst2 && tLeft < 0.0f && tRight < 0.0f)) {
		/* Project on left cut-off circle. */
		const VEC3 unitW = normalize(velocity_ - leftCutoff);

		line.direction = VEC3(unitW[1], -unitW[0],0);
		line.point = leftCutoff + radius_ * invTimeHorizonObst * unitW;
	} else if (t > 1.0f && tRight < 0.0f) {
		/* Project on right cut-off circle. */
		const VEC3 unitW = normalize(velocity_ - rightCutoff);

		line.direction = VEC3(unitW[1], -unitW[0],0);
		line.point = rightCutoff + radius_ * invTimeHorizonObst * unitW;
	} else {
		/*
		* Project on left leg, right leg, or cut-off line, whichever is closest
		* to velocity.
		*/

		const float distSqCutoff = ((t < 0.0f || t > 1.0f || obst1 == obst2) ? std::numeric_limits<float>::infinity() : absSq(velocity_ - (leftCutoff + t * cutoffVec)));
		const float distSqLeft = ((tLeft < 0.0f) ? std::numeric_limits<float>::infinity() : absSq(velocity_ - (leftCutoff + tLeft * leftLegDirection)));
		const float distSqRight = ((tRight < 0.0f) ? std::numeric_limits<float>::infinity() : absSq(velocity_ - (rightCutoff + tRight * rightLegDirection)));

		if (distSqCutoff <= distSqLeft && distSqCutoff <= distSqRight) {
		/* Project on cut-off line. */
		line.direction = -1.0f*normalize(cutoffVec);
		line.point = leftCutoff + radius_ * invTimeHorizonObst * VEC3(-line.direction[1], line.direction[0],0);
		} else if (distSqLeft <= distSqRight) {
		/* Project on left leg. */
		if (isLeftLegForeign) {
			continue;
		} else {
			line.direction = leftLegDirection;
			line.point = leftCutoff + radius_ * invTimeHorizonObst * VEC3(-line.direction[1], line.direction[0],0);
		}
		} else {
		/* Project on right leg. */
		if (isRightLegForeign) {
			continue;
		} else {
			line.direction = -1.0f*rightLegDirection;
			line.point = rightCutoff + radius_ * invTimeHorizonObst * VEC3(-line.direction[1], line.direction[0],0);
		}
		}
	}

	orcaLines_.push_back(line);
	}

    const size_t numObstLines = orcaLines_.size();


    const float invTimeHorizon = 1.0f / timeHorizon_;

    /* Create agent ORCA lines. */
    for (size_t i = 0; i < agentNeighbors_.size(); ++i) {
      const Agent* const other = sim_->agents_[agentNeighbors_[i].second];

      const VEC_A relativePosition(other->position_ - position_);
      const VEC_A relativeVelocity(velocity_ - other->velocity_);
      const float distSq = absSq(relativePosition);
      const float combinedRadius = radius_ + other->radius_;
      const float combinedRadiusSq = sqr(combinedRadius);

      Line line;
      VEC_A u;

      if (distSq > combinedRadiusSq) {
// 		std::cout << "no collision" << std::endl;
        /* No collision. */
        const VEC_A w = relativeVelocity - invTimeHorizon * relativePosition;
        const float wLengthSq = absSq(w);

        const float dotProduct1 = w * relativePosition;

        if (dotProduct1 < 0.0f && sqr(dotProduct1) > combinedRadiusSq * wLengthSq) {
          /* Project on cut-off circle. */
          const float wLength = std::sqrt(wLengthSq);
          const VEC_A unitW = w / wLength;

          line.direction = VEC_A(unitW[1], -unitW[0], 0);
          u = (combinedRadius * invTimeHorizon - wLength) * unitW;
        } else {
          /* Project on legs. */
          const float leg = std::sqrt(distSq - combinedRadiusSq);

          if (det2D(relativePosition, w) > 0.0f) {
            /* Project on left leg. */
            line.direction = VEC_A(relativePosition[0] * leg - relativePosition[1] * combinedRadius, relativePosition[0] * combinedRadius + relativePosition[1] * leg,0) / distSq;
          } else {
            /* Project on right leg. */
            line.direction = -1.0f*VEC_A(relativePosition[0] * leg + relativePosition[1] * combinedRadius, -relativePosition[0] * combinedRadius + relativePosition[1] * leg,0) / distSq;
          }

          const float dotProduct2 = relativeVelocity * line.direction;

          u = dotProduct2 * line.direction - relativeVelocity;
        }
      } else {
        /* Collision. */
        const float dist = std::sqrt(distSq);
        const VEC_A unitRelativePosition = relativePosition / dist;

        line.direction = VEC_A(-unitRelativePosition[1], unitRelativePosition[0],0);

        const VEC_A point = ((dist - combinedRadius) / sim_->timeStep_) * unitRelativePosition;
        const float dotProduct = (relativeVelocity - point) * line.direction;

        u = point + dotProduct * line.direction - relativeVelocity;
      }

      line.point = velocity_ + 0.5f * u;

      orcaLines_.push_back(line);
    }

    if (!linearProgram2(orcaLines_, orcaLines_.size(), maxSpeed_, prefVelocity_, false, newVelocity_)) {
      linearProgram3(orcaLines_, numObstLines, maxSpeed_, newVelocity_);
    }
}

void Agent::insertAgentNeighbor(size_t agentNo, float& rangeSq)
{
    const Agent* const other = sim_->agents_[agentNo];

    if (this != other) {
      const float distSq = absSq(position_ - other->position_);

      if (distSq < rangeSq) {
// 		std::cout << "ajout agent voisin" << std::endl;
        if (agentNeighbors_.size() < maxNeighbors_) {
          agentNeighbors_.push_back(std::make_pair(0.0f,0));
        }
        size_t i = agentNeighbors_.size() - 1;
        while (i != 0 && (distSq < agentNeighbors_[i-1].first
// 				|| (distSq==agentNeighbors_[i-1].first && ((other->position_[1] < position_[1]) || (other->position_[1]==position_[1] && (other->position_[0] < position_[0]))))
		)) {
          agentNeighbors_[i] = agentNeighbors_[i-1];
          --i;
        }
        agentNeighbors_[i] = std::make_pair(distSq, agentNo);

        if (agentNeighbors_.size() == maxNeighbors_) {
          rangeSq = agentNeighbors_.back().first;
        }
      }
    }
}

void Agent::insertObstacleNeighbor(Dart d)
{
	const float distSq = distSqPointLineSegment(sim_->envMap.position[d],sim_->envMap.position[sim_->envMap.map.phi1(d)],position_);

	if (distSq < rangeSq) {
		obstacleNeighbors_.push_back(std::make_pair(0.0f,0));
		
		size_t i = obstacleNeighbors_.size() - 1;
		while (i != 0 && distSq < obstacleNeighbors_[i-1].first) {
			obstacleNeighbors_[i] = obstacleNeighbors_[i-1];
			--i;
		}
		obstacleNeighbors_[i] = std::make_pair(distSq, d);
	}
}

void Agent::update()
{
	velocity_[0] = newVelocity_[0];
	velocity_[1] = newVelocity_[1];
	position_ += velocity_ * sim_->timeStep_;

	if(position_[0]!=part->m_position[0] || position_[1]!=part->m_position[1]) {
		part->move(VEC3(position_[0],position_[1],0));
		nearestDart = part->getCell();
	}
}

bool linearProgram1(const std::vector<Line>& lines, size_t lineNo, float radius, const Agent::Agent::VEC_A& optVelocity, bool directionOpt, Agent::VEC_A& result)
{
    const float discriminant = sqr(radius) - sqr(det2D(lines[lineNo].direction, lines[lineNo].point));

    if (discriminant < 0.0f) {
      /* Max speed circle fully invalidates line lineNo. */
      return false;
    }

    const float sqrtDiscriminant = std::sqrt(discriminant);

    float tLeft = -(lines[lineNo].direction * lines[lineNo].point) - sqrtDiscriminant;

    float tRight = -(lines[lineNo].direction * lines[lineNo].point) + sqrtDiscriminant;

    for (size_t i = 0; i < lineNo; ++i) {
      const float determinant = det2D(lines[lineNo].direction, lines[i].direction);

      if (std::fabs(determinant) <= RVO_EPSILON) {
        /* Lines lineNo and i are (almost) parallel. */
        if (det2D(lines[i].direction, lines[lineNo].point - lines[i].point) < 0.0f) {
          /* Line i fully invalidates line lineNo. */
          return false;
        } else {
          /* Line i does not impose constraint on line lineNo. */
          continue;
        }
      }

      const float t = det2D(lines[i].direction, lines[lineNo].point - lines[i].point) / determinant;

      if (determinant > 0.0f) {
        /* Line i bounds line lineNo on the right. */
        tRight = std::min(tRight, t);
      } else {
        /* Line i bounds line lineNo on the left. */
        tLeft = std::max(tLeft, t);
      }

      if (tLeft > tRight) {
        return false;
      }
    }

    if (directionOpt) {
      /* Optimize direction. */
      if (optVelocity * lines[lineNo].direction > 0.0f) {
        /* Take right extreme. */
        result = lines[lineNo].point + tRight * lines[lineNo].direction;
      } else {
        /* Take left extreme. */
        result = lines[lineNo].point + tLeft * lines[lineNo].direction;
      }
    } else {
      /* Optimize closest point. */
      const float t = lines[lineNo].direction * (optVelocity - lines[lineNo].point);

      if (t < tLeft) {
        result = lines[lineNo].point + tLeft * lines[lineNo].direction;
      } else if (t > tRight) {
        result = lines[lineNo].point + tRight * lines[lineNo].direction;
      } else {
        result = lines[lineNo].point + t * lines[lineNo].direction;
      }
    }

    return true;
}

bool linearProgram2(const std::vector<Line>& lines, size_t num, float radius, const Agent::VEC_A& optVelocity, bool directionOpt, Agent::VEC_A& result)
{
    if (directionOpt) {
      /* 
       * Optimize direction. Note that the optimization velocity is of unit
       * length in this case.
       */
      result = optVelocity * radius;
    } else if (absSq(optVelocity) > sqr(radius)) {
      /* Optimize closest point and outside circle. */
      result = normalize(optVelocity) * radius;
    } else {
      /* Optimize closest point and inside circle. */
      result = optVelocity;
    }

    for (size_t i = 0; i < num; ++i) {
      if (det2D(lines[i].direction, result - lines[i].point) < 0.0f) {
        /* Result does not satisfy constraint i. Compute new optimal result. */
        if (!linearProgram1(lines, i, radius, optVelocity, directionOpt, result)) {
          return false;
        }
      }
    }

    return true;
}

void linearProgram3(const std::vector<Line>& lines, size_t numObstLines, float radius, Agent::VEC_A& result)
{
    if (!linearProgram2(lines, numObstLines, radius, Agent::VEC_A(), false, result)) {
      /* 
       * This should not happen unless agents are initialized within obstacle
       * and time step is small.
       */
      linearProgram2(lines, numObstLines, std::numeric_limits<float>::infinity(), Agent::VEC_A(), false, result);

      return;
    }

    float distance = 0.0f;

    for (size_t i = numObstLines; i < lines.size(); ++i) {
      if (det2D(lines[i].direction, lines[i].point - result) > distance) {
        /* Result does not satisfy constraint of line i. */
        std::vector<Line> projLines(lines.begin(), lines.begin() + numObstLines);

        for (size_t j = numObstLines; j < i; ++j) {
          Line line;

          float determinant = det2D(lines[i].direction, lines[j].direction);

          if (std::fabs(determinant) <= RVO_EPSILON) {
            /* Line i and line j are (almost) parallel. */
            if (lines[i].direction * lines[j].direction > 0.0f) {
              /* Line i and line j point in the same direction. */
              continue;
            } else {
              /* Line i and line j point in opposite direction. */
              line.point = 0.5f * (lines[i].point + lines[j].point);
            }
          } else {
            line.point = lines[i].point + (det2D(lines[j].direction, lines[i].point - lines[j].point) / determinant) * lines[i].direction;
          }

          line.direction = normalize(lines[j].direction - lines[i].direction);
          projLines.push_back(line);
        }

        const Agent::VEC_A tempResult = result;

        if (!linearProgram2(projLines, projLines.size(), radius, Agent::VEC_A(-lines[i].direction[1], lines[i].direction[0],0), true, result)) {
          /* This should in principle not happen.  The result is by definition
           * already in the feasible region of this linear program. If it fails,
           * it is due to small floating point error, and the current result is
           * kept.
           */
          result = tempResult;
        }

        distance = det2D(lines[i].direction, lines[i].point - result);
      }
    }
}
