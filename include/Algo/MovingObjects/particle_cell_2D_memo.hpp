template <typename PFP>
void ParticleCell2DMemo<PFP>::move(const VEC3& goal)
{
	this->crossCell = NO_CROSS ;
	if (!Geom::arePointsEquals(goal, this->getPosition()))
	{
		switch (this->getState())
		{
			case VERTEX :
				vertexState(goal) ;
				break ;
			case EDGE :
				edgeState(goal) ;
				break ;
			case FACE :
				faceState(goal) ;
				break ;
		}
	}
	else
		this->ParticleBase<PFP>::move(goal) ;
}

template <typename PFP>
void ParticleCell2DMemo<PFP>::vertexState(const VEC3& current)
{
#ifdef DEBUG
	CGoGNout << "vertexState" << d << CGoGNendl ;
#endif
	assert(std::isfinite(current[0]) && std::isfinite(current[1]) && std::isfinite(current[2])) ;

	this->crossCell = CROSS_OTHER ;

	if (Algo::Geometry::isPointOnVertex < PFP > (this->m, this->d, this->positionAttribut, current))
	{
		this->setState(VERTEX) ;
		this->ParticleBase<PFP>::move(current) ;
		return ;
	}
	else
	{
		//orientation step
		if (this->positionAttribut[this->d][0] == this->positionAttribut[this->m.phi1(this->d)][0]
		    && this->positionAttribut[this->d][1]
		        == this->positionAttribut[this->m.phi1(this->d)][1]) this->d = this->m.phi2_1(
		    this->d) ;
		if (this->getOrientationEdge(current, this->m.phi2_1(this->d)) != Geom::RIGHT)
		{
			Dart dd_vert = this->d ;
			do
			{
				this->d = this->m.phi2_1(this->d) ;
				if (this->positionAttribut[this->d][0]
				    == this->positionAttribut[this->m.phi1(this->d)][0]
				    && this->positionAttribut[this->d][1]
				        == this->positionAttribut[this->m.phi1(this->d)][1]) this->d = this->m.phi2_1(this->d) ;
			} while (this->getOrientationEdge(current, this->m.phi2_1(this->d)) != Geom::RIGHT
			    && dd_vert != this->d) ;

			if (dd_vert == this->d)
			{
				//orbit with 2 edges : point on one edge
				if (this->m.phi2_1(this->m.phi2_1(this->d)) == this->d)
				{
					if (!Algo::Geometry::isPointOnHalfEdge < PFP
					    > (this->m, this->d, this->positionAttribut, current)) this->d = this->m.phi2_1(
					    this->d) ;
				}
				else
				{
					this->ParticleBase<PFP>::move(current) ;
					this->setState(VERTEX) ;
					return ;
				}
			}
		}
		else
		{
			Dart dd_vert = this->m.phi2_1(this->d) ;
			while (this->getOrientationEdge(current, this->d) == Geom::RIGHT && dd_vert != this->d)
			{
				this->d = this->m.phi12(this->d) ;
				if (this->positionAttribut[this->d][0]
				    == this->positionAttribut[this->m.phi1(this->d)][0]
				    && this->positionAttribut[this->d][1]
				        == this->positionAttribut[this->m.phi1(this->d)][1]) this->d = this->m.phi12(
				    this->d) ;
			}
		}

		//displacement step
		if (detect_vertex) memo_cross.push_back(this->d) ;
		if (this->getOrientationEdge(current, this->d) == Geom::ALIGNED
		    && Algo::Geometry::isPointOnHalfEdge < PFP
		        > (this->m, this->d, this->positionAttribut, current))
			edgeState(current) ;
		else
		{
			this->d = this->m.phi1(this->d) ;
			faceState(current) ;
		}
	}
}

template <typename PFP>
void ParticleCell2DMemo<PFP>::edgeState(const VEC3& current, Geom::Orientation2D sideOfEdge)
{
#ifdef DEBUG
	CGoGNout << "edgeState" << d << CGoGNendl ;
#endif
	if (detect_edge) memo_cross.push_back(this->d) ;
	assert(std::isfinite(current[0]) && std::isfinite(current[1]) && std::isfinite(current[2])) ;
// 	assert(Algo::Geometry::isPointOnEdge<PFP>(m,d,m_positions,m_position));

	if (this->crossCell == NO_CROSS)
	{
		this->crossCell = CROSS_EDGE ;
		this->lastCrossed = this->d ;
	}
	else
		this->crossCell = CROSS_OTHER ;

	if (sideOfEdge == Geom::ALIGNED) sideOfEdge = this->getOrientationEdge(current, this->d) ;

	switch (sideOfEdge)
	{
		case Geom::LEFT :
			this->d = this->m.phi1(this->d) ;
			faceState(current) ;
			return ;
		case Geom::RIGHT :
			this->d = this->m.phi1(this->m.phi2(this->d)) ;
			faceState(current) ;
			return ;
		default :
			this->setState(EDGE) ;
			break ;
	}

	if (!Algo::Geometry::isPointOnHalfEdge < PFP
	    > (this->m, this->d, this->positionAttribut, current))
	{
		this->ParticleBase<PFP>::move(this->positionAttribut[this->d]) ;
		vertexState(current) ;
		return ;
	}
	else if (!Algo::Geometry::isPointOnHalfEdge < PFP
	    > (this->m, this->m.phi2(this->d), this->positionAttribut, current))
	{
		this->d = this->m.phi2(this->d) ;
		this->ParticleBase<PFP>::move(this->positionAttribut[this->d]) ;
		vertexState(current) ;
		return ;
	}

	this->ParticleBase<PFP>::move(current) ;
}

template <typename PFP>
void ParticleCell2DMemo<PFP>::faceState(const VEC3& current)
{
#ifdef DEBUG
	CGoGNout << "faceState" << d << CGoGNendl ;
#endif
	if (detect_face) memo_cross.push_back(this->d) ;
	assert(
	    std::isfinite(this->getPosition()[0]) && std::isfinite(this->getPosition()[1])
	        && std::isfinite(this->getPosition()[2])) ;
	assert(std::isfinite(current[0]) && std::isfinite(current[1]) && std::isfinite(current[2])) ;
// 	assert(Algo::Geometry::isPointInConvexFace2D<PFP>(m,d,m_positions,m_position,true));

	Dart dd = this->d ;
	float wsoe = this->getOrientationFace(current, this->m.phi1(this->d)) ;

// orientation step
	if (wsoe != Geom::RIGHT)
	{
		this->d = this->m.phi1(this->d) ;
		wsoe = this->getOrientationFace(current, this->m.phi1(this->d)) ;
		while (wsoe != Geom::RIGHT && dd != this->d)
		{
			this->d = this->m.phi1(this->d) ;
			wsoe = this->getOrientationFace(current, this->m.phi1(this->d)) ;
		}

		// source and position to reach are the same : verify if no edge is crossed due to numerical approximation
		if (dd == this->d)
		{
			do
			{
				switch (this->getOrientationEdge(current, this->d))
				{
					case Geom::LEFT :
						this->d = this->m.phi1(this->d) ;
						break ;
					case Geom::ALIGNED :
						this->ParticleBase<PFP>::move(current) ;
						edgeState(current) ;
						return ;
					case Geom::RIGHT :
//									CGoGNout << "smthg went bad " << m_position << " " << current << CGoGNendl;
//									CGoGNout << "d1 " << m_positions[d] << " d2 " << m_positions[m.phi1(d)] << CGoGNendl;
						this->ParticleBase<PFP>::move(this->intersectLineEdge(current, this->getPosition(), this->d)) ;
//									CGoGNout << " " << m_position << CGoGNendl;

						edgeState(current, Geom::RIGHT) ;
						return ;
				}
			} while (this->d != dd) ;
			this->ParticleBase<PFP>::move(current);
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
		wsoe = this->getOrientationFace(current, this->d) ;
	}
	else
	{
		wsoe = this->getOrientationFace(current, this->d) ;
		while (wsoe == Geom::RIGHT && this->m.phi_1(this->d) != dd)
		{
			this->d = this->m.phi_1(this->d) ;
			wsoe = this->getOrientationFace(current, this->d) ;
		}

		// in case of numerical incoherence
		if (this->m.phi_1(this->d) == dd && wsoe == Geom::RIGHT)
		{
			this->d = this->m.phi_1(this->d) ;
			do
			{
				switch (this->getOrientationEdge(current, this->d))
				{
					case Geom::LEFT :
						this->d = this->m.phi1(this->d) ;
						break ;
					case Geom::ALIGNED :
// 					CGoGNout << "pic" << CGoGNendl;
						this->ParticleBase<PFP>::move(current) ;
						edgeState(current) ;
						return ;
					case Geom::RIGHT :
//					CGoGNout << "smthg went bad(2) " << m_position << CGoGNendl;
						this->ParticleBase<PFP>::move(this->intersectLineEdge(current, this->getPosition(), this->d)) ;
// 					CGoGNout << " " << m_position << CGoGNendl;
						edgeState(current, Geom::RIGHT) ;
						return ;
				}
			} while (this->d != dd) ;

			this->ParticleBase<PFP>::move(current) ;
			this->setState(FACE) ;
			return ;
		}
	}

//displacement step
	switch (this->getOrientationEdge(current, this->d))
	{
		case Geom::LEFT :
			this->ParticleBase<PFP>::move(current) ;
			this->setState(FACE) ;
			;
			break ;
// 	case Geom::ALIGNED :
//		if(wsoe==Geom::ALIGNED) {
// 			m_position = m_positions[d];
// 			vertexState(current);
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

				this->d = this->m.phi1(this->d) ; //to check
				this->ParticleBase<PFP>::move(this->positionAttribut[this->d]) ;
				vertexState(current) ;
			}
			else
			{
				this->ParticleBase<PFP>::move(this->intersectLineEdge(current, this->getPosition(), this->d)) ;
				edgeState(current, Geom::RIGHT) ;
			}
	}

}
