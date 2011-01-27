#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <limits>
#include <vector>

// #include "Vector2.h"
#include "Geometry/vector_gen.h"

typedef CGoGN::Geom::Vec3f VEC3;

static const float RVO_EPSILON = 0.00001f;
static const size_t RVO_ERROR = std::numeric_limits<size_t>::max();

 typedef VEC3 VEC_A;

  struct Line {
    VEC_A point;
    VEC_A direction;
  };

inline float abs(const VEC3& vector)
{
return std::sqrt(vector * vector);
}

inline float absSq(const VEC3& vector)
{
return vector[0] * vector[0] + vector[1] * vector[1];
// return vector * vector;
}

inline float det2D(const VEC_A& vector1, const VEC_A& vector2)
{
return vector1[0] * vector2[1] - vector1[1] * vector2[0];
}

inline VEC3 normalize(const VEC3& vector)
{
return vector / abs(vector);
}

inline float sqr(float a)
{
return a * a;
}

inline float distSqPointLineSegment(const VEC3& a, const VEC3& b,
									const VEC3& c)
{
VEC3 a1(a[0],a[1],0);
VEC3 b1(b[0],b[1],0);
VEC3 c1(c[0],c[1],0);

const float r = ((c1 - a1) * (b1 - a1)) / absSq(b1 - a1);

if (r < 0.0f) {
	return absSq(c1 - a1);
} else if (r > 1.0f) {
	return absSq(c1 - b1);
} else {
	return absSq(c1 - (a1 + r * (b1 - a1)));
}
}


inline void projectPointOnLine(const VEC3& p, const VEC3& p1, const VEC3& p2, VEC3& proj)
{
	VEC3 v(p-p1);
	VEC3 seg(p2-p1);

	seg.normalize();

	VEC3::DATA_TYPE dot = v*seg;

	proj = p1+seg*dot;
}

inline bool isInInterval(const VEC3& p, const VEC3& p1, const VEC3& p2)
{
	if( (VEC3(p2-p1) * VEC3(p-p1)) > 0.0f)
		if( VEC3(p1-p2) * VEC3(p-p2) > 0.0f)
			return true;

	return false;
}


inline float leftOf(const VEC_A& a, const VEC_A& b, const VEC_A& c)
{
return det2D(a - c, b - a);
}

#endif
