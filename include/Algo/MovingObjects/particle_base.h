#ifndef PARTBASE_H
#define PARTBASE_H

//#include "Geometry/vector_gen.h"

namespace CGoGN
{

namespace Algo
{

namespace MovingObjects
{

/* A particle base defines a position with a displacement function */

template <typename PFP>
class ParticleBase
{
private:
	typename PFP::VEC3 m_position;
	unsigned int m_state;

public :
	ParticleBase(typename PFP::VEC3 position) :
		m_position(position),
		m_state(FACE)
	{
	}

	void setState(unsigned int state) {
		m_state = state;
	}

	unsigned int getState()
	{
		return m_state;
	}

	bool move(typename PFP::VEC3 position)
	{
		m_position = position;
		return true;
	}

	typename PFP::VEC3 getPosition() { return m_position; }
};

} // namespace MovingObjects

} // namespace Algo

} // namespace CGoGN

#endif
