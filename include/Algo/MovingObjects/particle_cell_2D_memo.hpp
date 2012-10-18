template <typename PFP>
void ParticleCell2DMemo<PFP>::move(const VEC3& goal)
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

	this->display() ;
}

template <typename PFP>
void ParticleCell2DMemo<PFP>::vertexState(const VEC3& current)
{
#ifdef DEBUG
	CGoGNout << "vertexState" << this->d << CGoGNendl ;
#endif
	if (Algo::Geometry::isPointOnVertex < PFP > (this->m, this->d, this->positionAttribut, current))
	{
		this->setState(VERTEX) ;
		return ;
	}
	else
	{
		//orientation step
		if (this->getOrientationEdge(current, this->m.phi2_1(this->d)) != Geom::RIGHT)
		{

			Dart dd_vert = this->d ;
			do
			{
				this->d = this->m.phi2_1(this->d) ;
			} while (this->getOrientationEdge(current, this->m.phi2_1(this->d)) != Geom::RIGHT
			    && dd_vert != this->d) ;

			if (dd_vert == this->d)
			{
				this->setState(VERTEX) ;
				return ;
			}
		}
		else
		{
			while (this->getOrientationEdge(current, this->d) == Geom::RIGHT)
			{
				this->d = this->m.phi12(this->d) ;
			}
		}

		//displacement step
		memo_cross.push_back(this->d) ;
// 		if(!this->m.isMarked(this->d,this->obstacle)) {
		if (this->getOrientationEdge(current, this->d) == Geom::ALIGNED)
			edgeState(current) ;
		else
			faceState(current) ;
	}
// 	}
}

template <typename PFP>
void ParticleCell2DMemo<PFP>::edgeState(const VEC3& current, Geom::Orientation2D sideOfEdge)
{
#ifdef DEBUG
	CGoGNout << "edgeState" << this->d << CGoGNendl ;
#endif
	memo_cross.push_back(this->d) ;

	if (sideOfEdge == Geom::ALIGNED) sideOfEdge = this->getOrientationEdge(current, this->d) ;

	switch (sideOfEdge)
	{
		case Geom::LEFT :
			if (!this->obstacle.isMarked(this->d))
			{
				this->d = this->m.phi1(this->d) ;
				faceState(current) ;
				return ;
			}
		case Geom::RIGHT :
			if (!this->obstacle.isMarked(this->m.phi2(this->d)))
			{
				this->d = this->m.phi1(this->m.phi2(this->d)) ;
				faceState(current) ;
				return ;
			}
		default :
			this->setState(EDGE) ;
	}

	if (!Algo::Geometry::isPointOnHalfEdge<PFP> (this->m, this->d, this->positionAttribut, current))
	{
		this->ParticleBase<PFP>::move(this->positionAttribut[this->d]) ;
		vertexState(current) ;
		return ;
	}
	else if (!Algo::Geometry::isPointOnHalfEdge<PFP> (this->m, this->m.phi2(this->d), this->positionAttribut, current))
	{
		this->d = this->m.phi2(this->d) ;
		this->ParticleBase<PFP>::move(this->positionAttribut[this->d]) ;
		vertexState(current) ;
		return ;
	}

}

template <typename PFP>
void ParticleCell2DMemo<PFP>::faceState(const VEC3& current)
{
#ifdef DEBUG
	CGoGNout << "faceState" << this->d << CGoGNendl ;
#endif
	memo_cross.push_back(this->d) ;
	Dart dd = this->d ;

	//orientation step
	if (this->getOrientationFace(current, this->getPosition(), this->m.phi1(this->d)) != Geom::RIGHT)
	{
		this->d = this->m.phi1(this->d) ;
		while (this->getOrientationFace(current, this->getPosition(), this->m.phi1(this->d)) != Geom::RIGHT
		    && dd != this->d)
			this->d = this->m.phi1(this->d) ;

		//source and position to reach are the same : verify if no edge is crossed this->due to numerical approximation
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
					default :
						this->ParticleBase<PFP>::move(this->intersectLineEdge(current, this->getPosition(), this->d)) ;
						if (!this->obstacle.isMarked(this->m.phi2(this->d)))
							edgeState(current) ;
						else
							this->setState(EDGE) ;
						return ;
				}
			} while (this->d != dd) ;
			this->ParticleBase<PFP>::move(current) ;
			this->setState(FACE) ;
			return ;
		}
	}
	else
	{
		while (this->getOrientationFace(current, this->getPosition(), this->d) == Geom::RIGHT
		    && this->m.phi_1(this->d) != dd)
			this->d = this->m.phi_1(this->d) ;

		//in case of numerical incoherence
		if (this->m.phi_1(this->d) == dd)
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
						this->ParticleBase<PFP>::move(current) ;
						edgeState(current) ;
						return ;
					default :
						this->ParticleBase<PFP>::move(this->intersectLineEdge(current, this->getPosition(), this->d)) ;
						if (!this->obstacle.isMarked(this->m.phi2(this->d)))
							edgeState(current) ;
						else
							this->setState(EDGE) ;
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
		case Geom::ALIGNED :
			this->ParticleBase<PFP>::move(current) ;
			this->setState(EDGE) ;
			break ;
		default :
			this->ParticleBase<PFP>::move(this->intersectLineEdge(current, this->getPosition(), this->d)) ;
			if (!this->obstacle.isMarked(this->m.phi2(this->d)))
				edgeState(current) ;
			else
				this->setState(EDGE) ;
	}
}
