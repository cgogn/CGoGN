// #define DEBUG

template <typename PFP>
void ParticleCell2D<PFP>::display()
{
// 	CGoGNout << "pos : " << this->m_position << CGoGNendl;
// 	CGoGNout << "d : " << this->d << CGoGNendl;
// 	CGoGNout << "state : " << this->state << CGoGNendl;
}

template <typename PFP>
typename PFP::VEC3 ParticleCell2D<PFP>::pointInFace(Dart d)
{
	const VEC3& p1(m_positions[d]);
	Dart dd=m.phi1(d);
	const VEC3& p2(m_positions[dd]);
	dd=m.phi1(dd);
	VEC3& p3(m_positions[dd]);
	
	while(Geom::testOrientation2D(p3,p1,p2)==Geom::ALIGNED) {
		dd = m.phi1(dd);
		p3 = m_positions[dd];
	}

	CGoGNout << "pointInFace " << (p1+p3)*0.5f << CGoGNendl;

	return (p1+p3)*0.5f;
}

template <typename PFP>
Geom::Orientation2D ParticleCell2D<PFP>::getOrientationEdge(const VEC3& point, Dart d)
{
	const VEC3& endPoint = m_positions[m.phi2(d)];
	const VEC3& vertexPoint = m_positions[d];

	return Geom::testOrientation2D(point,vertexPoint,endPoint);
}

template <typename PFP>
typename PFP::VEC3 ParticleCell2D<PFP>::intersectLineEdge(const VEC3& pA, const VEC3& pB, Dart d)
{
	VEC3 q1 = m_positions[d];
	VEC3 q2 = m_positions[m.phi2(d)];
	VEC3 Inter;

	Geom::intersection2DSegmentSegment(pA,pB,q1,q2,Inter);

// 	if(VEC3(Inter-pA).norm()>VEC3(pA-pB).norm()) {
// 		CGoGNout << "probleme : " << pA << "/" << pB << "/" << q1 << "/" << q2 << "/" << Inter << CGoGNendl;
// 		CGoGNout << "isPointOnHalf " << Algo::Geometry::isPointOnHalfEdge<PFP>(m,d,m_positions,Inter) << CGoGNendl;
// 		CGoGNout << "isPointOnHalf " << Algo::Geometry::isPointOnHalfEdge<PFP>(m,m.phi1(d),m_positions,Inter) << CGoGNendl;
// 	}

	return Inter;
}

template <typename PFP>
Geom::Orientation2D ParticleCell2D<PFP>::getOrientationFace(VEC3 point, VEC3 sourcePoint, Dart d)
{
	const VEC3& dPoint = m_positions[d];
	return Geom::testOrientation2D(point,sourcePoint,dPoint);
}

template <typename PFP>
void ParticleCell2D<PFP>::vertexState(const VEC3& current)
{
	#ifdef DEBUG
	CGoGNout << "vertexState" << d << CGoGNendl;
	#endif
	assert(std::isfinite(current[0]) && std::isfinite(current[1]) && std::isfinite(current[2]));

	crossCell = CROSS_OTHER;

	if(Algo::Geometry::isPointOnVertex<PFP>(m,d,m_positions,current))
	{
		state = VERTEX_ORBIT;
		return;
	}
	else
	{
		//orientation step
		if(m_positions[d][0] == m_positions[m.phi1(d)][0] && m_positions[d][1] == m_positions[m.phi1(d)][1])
			d = m.alpha1(d);
		if(getOrientationEdge(current,m.alpha1(d)) != Geom::RIGHT)
		{
			Dart dd_vert = d;
			do
			{
				d = m.alpha1(d);
				if(m_positions[d][0] == m_positions[m.phi1(d)][0] && m_positions[d][1] == m_positions[m.phi1(d)][1])
					d = m.alpha1(d);
			} while(getOrientationEdge(current, m.alpha1(d)) != Geom::RIGHT && dd_vert != d);

			if(dd_vert == d)
			{
				//orbit with 2 edges : point on one edge
				if(m.alpha1(m.alpha1(d)) == d)
				{
					if(!Algo::Geometry::isPointOnHalfEdge<PFP>(m,d,m_positions,current))
						d = m.alpha1(d);
				}
				else
				{
					state = VERTEX_ORBIT;
					return;
				}
			}
		}
		else
		{
			Dart dd_vert = m.alpha1(d);
			while(getOrientationEdge(current, d) == Geom::RIGHT && dd_vert != d)
			{
				d = m.alpha_1(d);
				if(m_positions[d][0] == m_positions[m.phi1(d)][0] && m_positions[d][1] == m_positions[m.phi1(d)][1])
					d = m.alpha_1(d);
			}
		}

		//displacement step
		if(getOrientationEdge(current, d) == Geom::ALIGNED && Algo::Geometry::isPointOnHalfEdge<PFP>(m, d, m_positions, current))
			edgeState(current);
		else
		{
			d = m.phi1(d);
			faceState(current);
		}
	}
}

template <typename PFP>
void ParticleCell2D<PFP>::edgeState(const VEC3& current, Geom::Orientation2D sideOfEdge)
{
	#ifdef DEBUG
	CGoGNout << "edgeState" <<  d << CGoGNendl;
	#endif

	assert(std::isfinite(current[0]) && std::isfinite(current[1]) && std::isfinite(current[2]));
// 	assert(Algo::Geometry::isPointOnEdge<PFP>(m,d,m_positions,m_position));

	if(crossCell == NO_CROSS)
	{
		crossCell = CROSS_EDGE;
		lastCrossed = d;
	}
	else
		crossCell = CROSS_OTHER;

	if(sideOfEdge == Geom::ALIGNED)
		sideOfEdge = getOrientationEdge(current, d);

	switch(sideOfEdge)
	{
		case Geom::LEFT :
			d = m.phi1(d);
			faceState(current);
			return;
		case Geom::RIGHT:
			d = m.phi1(m.phi2(d));
			faceState(current);
			return;
		default :
			state = EDGE_ORBIT;
	}

	if(!Algo::Geometry::isPointOnHalfEdge<PFP>(m, d, m_positions, current))
	{
		m_position = m_positions[d];
		vertexState(current);
		return;
	}
	else if(!Algo::Geometry::isPointOnHalfEdge<PFP>(m, m.phi2(d), m_positions, current))
	{
		d = m.phi2(d);
		m_position = m_positions[d];
		vertexState(current);
		return;
	}
}

template <typename PFP>
void ParticleCell2D<PFP>::faceState(const VEC3& current)
{
	#ifdef DEBUG
	CGoGNout << "faceState" <<  d << CGoGNendl;
	#endif

 	assert(std::isfinite(m_position[0]) && std::isfinite(m_position[1]) && std::isfinite(m_position[2]));
 	assert(std::isfinite(current[0]) && std::isfinite(current[1]) && std::isfinite(current[2]));
// 	assert(Algo::Geometry::isPointInConvexFace2D<PFP>(m,d,m_positions,m_position,true));

	Dart dd = d;
	float wsoe = getOrientationFace(current, m_position, m.phi1(d));

	// orientation step
	if(wsoe != Geom::RIGHT)
	{
		d = m.phi1(d);
		wsoe = getOrientationFace(current, m_position, m.phi1(d));
		while(wsoe != Geom::RIGHT && dd != d)
		{
			d = m.phi1(d);
			wsoe = getOrientationFace(current, m_position, m.phi1(d));
		}

 		// source and position to reach are the same : verify if no edge is crossed due to numerical approximation
		if(dd == d)
		{
			do
			{
				switch (getOrientationEdge(current, d))
				{
				case Geom::LEFT: 	d = m.phi1(d);
									break;
				case Geom::ALIGNED:	m_position = current;
									edgeState(current);
									return;
				case Geom::RIGHT:	CGoGNout << "smthg went bad " << m_position << " " << current << CGoGNendl;
									CGoGNout << "d1 " << m_positions[d] << " d2 " << m_positions[m.phi1(d)] << CGoGNendl;
									m_position = intersectLineEdge(current, m_position, d);
									CGoGNout << " " << m_position << CGoGNendl;

									edgeState(current,Geom::RIGHT);
									return;
				}
			} while(d != dd);
			m_position = current;
			state = FACE_ORBIT;

// 			m_position = Algo::Geometry::faceCentroid<PFP>(m,d,m_positions);
// 			d = m.phi1(d);
// 			m_position = pointInFace(d);
// 			faceState(current);

// 			m_position = m_positions[d];
// 			vertexState(current);
			return;
		}
		// take the orientation with d1 : in case we are going through a vertex
		wsoe = getOrientationFace(current, m_position, d);
	}
	else
	{
		wsoe = getOrientationFace(current,m_position,d);
		while(wsoe == Geom::RIGHT && m.phi_1(d) != dd)
		{
			d = m.phi_1(d);
			wsoe = getOrientationFace(current, m_position, d);
		}

		// in case of numerical incoherence
		if(m.phi_1(d) == dd && wsoe == Geom::RIGHT)
		{
			d = m.phi_1(d);
			do
			{
				switch (getOrientationEdge(current, d))
				{
				case Geom::LEFT :
					d = m.phi1(d);
					break;
				case Geom::ALIGNED :			
// 					CGoGNout << "pic" << CGoGNendl;
					m_position = current;
					edgeState(current);
					return;
				case Geom::RIGHT :
					CGoGNout << "smthg went bad(2) " << m_position << CGoGNendl;
					m_position = intersectLineEdge(current, m_position, d);
// 					CGoGNout << " " << m_position << CGoGNendl;
					edgeState(current, Geom::RIGHT);
					return;
				}
			} while(d != dd);

			m_position = current;
			state = FACE_ORBIT;
			return;
		}
	}

	//displacement step
	switch (getOrientationEdge(current, d))
	{
	case Geom::LEFT :
		m_position = current;
		state = FACE_ORBIT;;
		break;
// 	case Geom::ALIGNED :
//		if(wsoe==Geom::ALIGNED) {
// 			m_position = m_positions[d];
// 			vertexState(current);
// 		}
// 		else {
// 			CGoGNout << "poc" << CGoGNendl;
// 			m_position = current;
// 			state = EDGE_ORBIT;
// 		}
// 		break;
	default :
		if(wsoe == Geom::ALIGNED)
		{
			m_position = m_positions[d];
			vertexState(current);
		}
		else
		{
// 			CGoGNout << "wsoe : " << wsoe << CGoGNendl;
// 			CGoGNout << "current " << current << " " << m_position << CGoGNendl;
// 			CGoGNout << "d " << d << "d1 " << m_positions[d] << " d2 " << m_positions[m.phi2(d)] << CGoGNendl;
			m_position = intersectLineEdge(current, m_position, d);
// 			CGoGNout << " inter : " << m_position << CGoGNendl;
			edgeState(current, Geom::RIGHT);
		}
	}
}
