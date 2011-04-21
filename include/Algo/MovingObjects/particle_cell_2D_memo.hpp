// #define DEBUG

template <typename PFP>
void ParticleCell2DMemo<PFP>::move(const VEC3& newCurrent)
{
	this->prevPos = this->m_position;

	switch(this->state) {
	case VERTEX_ORBIT : vertexState(newCurrent); break;
	case EDGE_ORBIT : 	edgeState(newCurrent);   break;
	case FACE_ORBIT : 	faceState(newCurrent);   break;
	}

	this->display();
}

template <typename PFP>
void ParticleCell2DMemo<PFP>::vertexState(const VEC3& current)
{
	#ifdef DEBUG
	CGoGNout << "vertexState" <<  this->d << CGoGNendl;
	#endif
	if(Algo::Geometry::isPointOnVertex<PFP>(this->m, this->d, this->m_positions, current)) {
		this->state = VERTEX_ORBIT;
		return;
	}
	else {
		//orientation step
		if(getOrientationEdge(current, this->m.alpha1(this->d))!=Geom::RIGHT) {

			Dart dd_vert = this->d;
			do {
				this->d = this->m.alpha1( this->d );
			}while(getOrientationEdge(current, this->m.alpha1( this->d ))!=Geom::RIGHT && dd_vert!=this->d);

			if(dd_vert==this->d) {
    			this->state = VERTEX_ORBIT;
				return;
			}
		}
		else {
			while(getOrientationEdge(current,this->d)==Geom::RIGHT) {
				this->d = this->m.alpha_1( this->d );
			}
		}

		//displacement step
		memo_cross.push_back( this->d );
// 		if(!this->m.isMarked(this->d,this->obstacle)) {
			if(getOrientationEdge(current,this->d)==Geom::ALIGNED) 
				edgeState(current);
			else
				faceState(current);
		}
// 	}
}

template <typename PFP>
void ParticleCell2DMemo<PFP>::edgeState(const VEC3& current, Geom::Orientation2D sideOfEdge)
{
	#ifdef DEBUG
	CGoGNout << "edgeState" <<  this->d << CGoGNendl;
	#endif
	memo_cross.push_back( this->d );

	if(sideOfEdge==Geom::ALIGNED)
		sideOfEdge = getOrientationEdge(current,this->d);

	switch(sideOfEdge) {
		case Geom::LEFT :
							if(!this->obstacle.isMarked(this->d)) {
								this->d = this->m.phi1( this->d );
								faceState(current);
								return;
							}
		case Geom::RIGHT:
							if(!this->obstacle.isMarked(this->m.phi2( this->d ))) {
								this->d = this->m.phi1(this->m.phi2( this->d ));
								faceState(current);
								return;
							}
		default :  this->state = EDGE_ORBIT;
	}

	if(!Algo::Geometry::isPointOnHalfEdge<PFP>(this->m, this->d, this->m_positions,current)) {
		this->prevPos = this->m_position;
		this->m_position = this->m_positions[this->d];
		vertexState(current);
		return;
	}
	else if(!Algo::Geometry::isPointOnHalfEdge<PFP>(this->m, this->m.phi2( this->d ), this->m_positions,current)) {
		this->d = this->m.phi2( this->d );
		this->prevPos = this->m_position;
		this->m_position = this->m_positions[this->d];
		vertexState(current);
		return;
	}

}

template <typename PFP>
void ParticleCell2DMemo<PFP>::faceState(const VEC3& current)
{
	#ifdef DEBUG
	CGoGNout << "faceState" <<  this->d << CGoGNendl;
	#endif
	memo_cross.push_back( this->d );
	Dart dd=this->d;

	//orientation step
	if(getOrientationFace(current, this->m_position, this->m.phi1( this->d ))!=Geom::RIGHT) {
		this->d = this->m.phi1( this->d );
		while(getOrientationFace(current, this->m_position, this->m.phi1( this->d ))!=Geom::RIGHT && dd!=this->d)
			this->d = this->m.phi1( this->d );

 		//source and position to reach are the same : verify if no edge is crossed this->due to numerical approximation
		if(dd==this->d) {
			do {
				switch (getOrientationEdge(current,this->d)) {
				case Geom::LEFT : 	this->d=this->m.phi1( this->d );
									break;
				case Geom::ALIGNED :this->m_position = current;
									edgeState(current);
									return;
				default :			this->m_position = intersectLineEdge(current, this->m_position,this->d);
									if(!this->obstacle.isMarked(this->m.phi2( this->d )))
										edgeState(current);
									else 
										this->state = EDGE_ORBIT;
									return;
				}
			}while(this->d!=dd);
			this->prevPos = this->m_position;
			this->m_position = current;
			this->state = FACE_ORBIT;
			return;
		}
	}
	else {
		while(getOrientationFace(current, this->m_position,this->d)==Geom::RIGHT && this->m.phi_1( this->d )!=dd)
			this->d = this->m.phi_1( this->d );

		//in case of numerical incoherence
		if(this->m.phi_1( this->d )==dd) {
			this->d = this->m.phi_1( this->d );
			do {
				switch (getOrientationEdge(current,this->d)) {
				case Geom::LEFT : 	this->d=this->m.phi1( this->d );
									break;
				case Geom::ALIGNED :this->m_position = current;
									edgeState(current);
									return;
				default :			this->m_position = intersectLineEdge(current, this->m_position,this->d);
									if(!this->obstacle.isMarked(this->m.phi2( this->d )))
										edgeState(current);
									else 
										this->state = EDGE_ORBIT;
									return;
				}
			} while(this->d!=dd);

			this->m_position = current;
			this->state = FACE_ORBIT;
			return;
		}
	}

	this->prevPos = this->m_position;

	//displacement step
	switch (getOrientationEdge(current,this->d)) {
	case Geom::LEFT :	this->m_position = current;
		 				this->state = FACE_ORBIT;;
		 				break;
	case Geom::ALIGNED :this->m_position = current;
		 				this->state = EDGE_ORBIT;
		 				break;
	default :			this->m_position = intersectLineEdge(current, this->m_position,this->d);
						if(!this->obstacle.isMarked(this->m.phi2( this->d )))
							edgeState(current);
						else
							this->state = EDGE_ORBIT;
	}
}