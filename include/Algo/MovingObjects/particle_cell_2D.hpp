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
	const VEC3& p1(positionAttribut[d]) ;
	Dart dd = m.phi1(d) ;
	const VEC3& p2(positionAttribut[dd]) ;
	dd = m.phi1(dd) ;
	VEC3& p3(positionAttribut[dd]) ;

	while (Geom::testOrientation2D(p3, p1, p2) == Geom::ALIGNED)
	{
		dd = m.phi1(dd) ;
		p3 = positionAttribut[dd] ;
	}

	CGoGNout << "pointInFace " << (p1 + p3) * 0.5f << CGoGNendl ;

	return (p1 + p3) * 0.5f ;
}

template <typename PFP>
Geom::Orientation2D ParticleCell2D<PFP>::getOrientationEdge(const VEC3& point, Dart d)
{
	const VEC3& endPoint = positionAttribut[m.phi2(d)] ;
	const VEC3& vertexPoint = positionAttribut[d] ;

	return Geom::testOrientation2D(point, vertexPoint, endPoint) ;
}

template <typename PFP>
typename PFP::VEC3 ParticleCell2D<PFP>::intersectLineEdge(const VEC3& pA, const VEC3& pB, Dart d)
{
	VEC3 q1 = positionAttribut[d] ;
	VEC3 q2 = positionAttribut[m.phi2(d)] ;
	VEC3 Inter ;

	Geom::intersection2DSegmentSegment(pA, pB, q1, q2, Inter) ;

	return Inter ;
}

template <typename PFP>
Geom::Orientation2D ParticleCell2D<PFP>::getOrientationFace(VEC3 goal, Dart d)
{
	const VEC3& p1 = this->getPosition() ;
	const VEC3& p2 = positionAttribut[d] ;
	return Geom::testOrientation2D(goal, p1, p2) ;
}

template <typename PFP>
void ParticleCell2D<PFP>::vertexState(const VEC3& goal)
{
#ifdef DEBUG
	CGoGNout << "vertexState" << d << CGoGNendl ;
#endif
	assert(goal.isFinite()) ;

	crossCell = CROSS_OTHER ;

	if (Algo::Geometry::isPointOnVertex < PFP > (m, d, positionAttribut, goal))
	{
		this->setState(VERTEX) ;
		this->ParticleBase < PFP > ::move(goal) ;
		return ;
	}
	else
	{
		//orientation step
		if(positionAttribut[d][0] == positionAttribut[m.phi1(d)][0] && positionAttribut[d][1] == positionAttribut[m.phi1(d)][1])
			d = m.phi2_1(d);
		if(getOrientationEdge(goal,m.phi2_1(d)) != Geom::RIGHT)
		{
			Dart dd_vert = d ;
			do
			{
				d = m.phi2_1(d);
				if(positionAttribut[d][0] == positionAttribut[m.phi1(d)][0] && positionAttribut[d][1] == positionAttribut[m.phi1(d)][1])
					d = m.phi2_1(d);
			} while(getOrientationEdge(goal, m.phi2_1(d)) != Geom::RIGHT && dd_vert != d);

			if (dd_vert == d)
			{
				//orbit with 2 edges : point on one edge
				if(m.phi2_1(m.phi2_1(d)) == d)
				{
					if(!Algo::Geometry::isPointOnHalfEdge<PFP>(m,d,positionAttribut,goal))
						d = m.phi2_1(d);
				}
				else
				{
					//checking : case with 3 orthogonal darts and point on an edge
					do
					{
						if(Algo::Geometry::isPointOnHalfEdge<PFP>(m,d,positionAttribut,goal)
								&& Algo::Geometry::isPointOnHalfEdge<PFP>(m,this->m.phi2(d),positionAttribut,goal)
								&& this->getOrientationEdge(current, this->d) == Geom::ALIGNED)
						{
							edgeState(goal) ;
							return;
						}
						this->d = this->m.phi2_1(this->d) ;
					} while (this->getOrientationEdge(goal, this->m.phi2_1(this->d)) != Geom::RIGHT && dd_vert != this->d) ;


					this->setState(VERTEX) ;
					this->ParticleBase < PFP > ::move(goal) ;
					return ;
				}
			}
		}
		else
		{
			Dart dd_vert = m.phi2_1(d);
			while(getOrientationEdge(goal, d) == Geom::RIGHT && dd_vert != d)
			{
				d = m.phi12(d);
				if(positionAttribut[d][0] == positionAttribut[m.phi1(d)][0] && positionAttribut[d][1] == positionAttribut[m.phi1(d)][1])
					d = m.phi12(d);
			}
		}

		//displacement step
		if (getOrientationEdge(goal, d) == Geom::ALIGNED
		    && Algo::Geometry::isPointOnHalfEdge < PFP > (m, d, positionAttribut, goal))
			edgeState(goal) ;
		else
		{
			d = m.phi1(d) ;
			faceState(goal) ;
		}
	}
}

template <typename PFP>
void ParticleCell2D<PFP>::edgeState(const VEC3& goal, Geom::Orientation2D sideOfEdge)
{
#ifdef DEBUG
	CGoGNout << "edgeState" << d << CGoGNendl ;
#endif

	assert(goal.isFinite()) ;
// 	assert(Algo::Geometry::isPointOnEdge<PFP>(m,d,m_positions,m_position));

	if (crossCell == NO_CROSS)
	{
		crossCell = CROSS_EDGE ;
		lastCrossed = d ;
	}
	else
		crossCell = CROSS_OTHER ;

	if (sideOfEdge == Geom::ALIGNED) sideOfEdge = getOrientationEdge(goal, d) ;

	switch (sideOfEdge)
	{
		case Geom::LEFT :
			d = m.phi1(d) ;
			faceState(goal) ;
			return ;
		case Geom::RIGHT :
			d = m.phi1(m.phi2(d)) ;
			faceState(goal) ;
			return ;
		default :
			this->setState(EDGE) ;
			break ;
	}

	if (!Algo::Geometry::isPointOnHalfEdge < PFP > (m, d, positionAttribut, goal))
	{
		this->ParticleBase < PFP > ::move(positionAttribut[d]) ;
		vertexState(goal) ;
		return ;
	}
	else if (!Algo::Geometry::isPointOnHalfEdge < PFP > (m, m.phi2(d), positionAttribut, goal))
	{
		d = m.phi2(d) ;
		this->ParticleBase < PFP > ::move(positionAttribut[d]) ;
		vertexState(goal) ;
		return ;
	}

	this->ParticleBase < PFP > ::move(goal) ;
}

template <typename PFP>
Dart ParticleCell2D<PFP>::faceOrientationState(const VEC3& toward)
{
#ifdef DEBUG
	CGoGNout << "faceOrientationState" << d << CGoGNendl ;
#endif

	assert(this->getPosition().isnormal());
	assert(toward.isnormal());

	Dart res = d ;
	Dart dd = d ;
	float wsoe = getOrientationFace(toward, m.phi1(res)) ;

	// orientation step
	if (wsoe != Geom::RIGHT)
	{
		res = m.phi1(res) ;
		wsoe = getOrientationFace(toward, m.phi1(res)) ;
		while (wsoe != Geom::RIGHT && dd != res)
		{
			res = m.phi1(res) ;
			wsoe = getOrientationFace(toward, m.phi1(res)) ;
		}

		// source and position to reach are the same : verify if no edge is crossed due to numerical approximation
		if (dd == res)
		{
			do
			{
				switch (getOrientationEdge(toward, res))
				{
					case Geom::LEFT :
						res = m.phi1(res) ;
						break ;
					case Geom::ALIGNED :
						return res ;
					case Geom::RIGHT :
						return res ;
				}
			} while (res != dd) ;
			return res ;
		}
	}
	else
	{
		wsoe = getOrientationFace(toward, d) ;
		while (wsoe == Geom::RIGHT && m.phi_1(res) != dd)
		{
			res = m.phi_1(res) ;
			wsoe = getOrientationFace(toward, res) ;
		}

		// in case of numerical incoherence
		if (m.phi_1(res) == dd && wsoe == Geom::RIGHT)
		{
			res = m.phi_1(res) ;
			do
			{
				switch (getOrientationEdge(toward, res))
				{
					case Geom::LEFT :
						res = m.phi1(res) ;
						break ;
					case Geom::ALIGNED :
						return res ;
					case Geom::RIGHT :
						return res ;
				}
			} while (res != dd) ;

			return res ;
		}
	}

	return res ;
}

template <typename PFP>
void ParticleCell2D<PFP>::faceState(const VEC3& goal)
{
#ifdef DEBUG
	CGoGNout << "faceState" << d << CGoGNendl ;
#endif

	assert(this->getPosition().isFinite());
	assert(goal.isFinite()) ;
// 	assert(Algo::Geometry::isPointInConvexFace2D<PFP>(m,d,m_positions,m_position,true));

	Dart dd = d ;
	float wsoe = getOrientationFace(goal, m.phi1(d)) ;

	// orientation step
	if (wsoe != Geom::RIGHT)
	{
		d = m.phi1(d) ;
		wsoe = getOrientationFace(goal, m.phi1(d)) ;
		while (wsoe != Geom::RIGHT && dd != d)
		{
			d = m.phi1(d) ;
			wsoe = getOrientationFace(goal, m.phi1(d)) ;
		}

		// source and position to reach are the same : verify if no edge is crossed due to numerical approximation
		if (dd == d)
		{
			do
			{
				switch (getOrientationEdge(goal, d))
				{
					case Geom::LEFT :
						d = m.phi1(d) ;
						break ;
					case Geom::ALIGNED :
						this->ParticleBase<PFP>::move(goal) ;
						edgeState(goal) ;
						return ;
					case Geom::RIGHT :
						this->ParticleBase<PFP>::move(intersectLineEdge(goal, this->getPosition(), d)) ;
						edgeState(goal, Geom::RIGHT) ;
						return ;
				}
			} while (d != dd) ;
			this->ParticleBase<PFP>::move(goal) ;
			this->setState(FACE) ;

// 			m_position = Algo::Geometry::faceCentroid<PFP>(m,d,m_positions);
// 			d = m.phi1(d);
// 			m_position = pointInFace(d);
// 			faceState(current);

// 			m_position = m_positions[d];
// 			vertexState(current);
			return ;
		}
		// take the orientation with d1 : in case we are going through a vertex
		wsoe = getOrientationFace(goal, d) ;
	}
	else
	{
		wsoe = getOrientationFace(goal, d) ;
		while (wsoe == Geom::RIGHT && m.phi_1(d) != dd)
		{
			d = m.phi_1(d) ;
			wsoe = getOrientationFace(goal, d) ;
		}

		// in case of numerical incoherence
		if (m.phi_1(d) == dd && wsoe == Geom::RIGHT)
		{
			d = m.phi_1(d) ;
			do
			{
				switch (getOrientationEdge(goal, d))
				{
					case Geom::LEFT :
						d = m.phi1(d) ;
						break ;
					case Geom::ALIGNED :
// 					CGoGNout << "pic" << CGoGNendl;
						this->ParticleBase<PFP>::move(goal) ;
						edgeState(goal) ;
						return ;
					case Geom::RIGHT :
//					CGoGNout << "smthg went bad(2) " << m_position << CGoGNendl;
						this->ParticleBase<PFP>::move(intersectLineEdge(goal, this->getPosition(), d)) ;
// 					CGoGNout << " " << m_position << CGoGNendl;
						edgeState(goal, Geom::RIGHT) ;
						return ;
				}
			} while (d != dd) ;

			this->ParticleBase<PFP>::move(goal) ;
			this->setState(FACE) ;
			return ;
		}
	}

	//displacement step
	switch (getOrientationEdge(goal, d))
	{
		case Geom::LEFT :
			this->ParticleBase<PFP>::move(goal) ;
			this->setState(FACE) ;
			;
			break ;
// 	case Geom::ALIGNED :
//		if(wsoe==Geom::ALIGNED) {
// 			m_position = m_positions[d];
// 			vertexState(curgoalrent);
// 		}
// 		else {
// 			CGoGNout << "poc" << CGoGNendl;
// 			m_position = current;
// 			state = EDGE;
// 		}
// 		break;
		default :
			if (wsoe == Geom::ALIGNED)
			{
				d = m.phi1(d) ; //to check
				this->ParticleBase<PFP>::move(positionAttribut[d]) ;
				vertexState(goal) ;
			}
			else
			{
				this->ParticleBase<PFP>::move(intersectLineEdge(goal, this->getPosition(), d)) ;
				edgeState(goal, Geom::RIGHT) ;
			}
			break ;
	}
}
