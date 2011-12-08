#ifndef PARTBASE_H
#define PARTBASE_H

#include "Geometry/vector_gen.h"

namespace CGoGN
{

namespace Algo
{

namespace MovingObjects
{

typedef Geom::Vec3f VEC3;

/* A particle base defines a position with a displacement function */

class ParticleBase
{
	public :
		VEC3 m_position;

		ParticleBase()
		{
			m_position.zero();
		}

		ParticleBase(VEC3 position)
		{
			m_position = position;
		}

		/**
		* @param newPosition new position to reach
		*/
		virtual bool move(VEC3 position)
		{
			m_position = position;
			return true;
		}

		virtual unsigned int getState()
		{
			return 0;
		}

		VEC3 getPosition() { return m_position; }
};

//namespace
}

}

}


#endif
