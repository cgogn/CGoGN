// #define DEBUG

template <typename PFP>
void ParticleCell2D<PFP>::display()
{
// 	std::cout << "pos : " << this->m_position << std::endl;
// 	std::cout << "d : " << this->d << std::endl;
// 	std::cout << "state : " << this->state << std::endl;
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

	std::cout << "pointInFace " << (p1+p3)*0.5f << std::endl;

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
// 		std::cout << "probleme : " << pA << "/" << pB << "/" << q1 << "/" << q2 << "/" << Inter << std::endl;
// 		std::cout << "isPointOnHalf " << Algo::Geometry::isPointOnHalfEdge<PFP>(m,d,m_positions,Inter) << std::endl;
// 		std::cout << "isPointOnHalf " << Algo::Geometry::isPointOnHalfEdge<PFP>(m,m.phi1(d),m_positions,Inter) << std::endl;
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
	std::cout << "vertexState" << d << std::endl;
	#endif
	assert(std::isfinite(current[0]) && std::isfinite(current[1]) && std::isfinite(current[2]));

	changeCell = true;
	if(Algo::Geometry::isPointOnVertex<PFP>(m,d,m_positions,current)) {
		state = VERTEX_ORBIT;
		return;
	}
	else {
		//orientation step
		if(m_positions[d][0]==m_positions[m.phi1(d)][0] && m_positions[d][1]==m_positions[m.phi1(d)][1])
			d=m.alpha1(d);
		if(getOrientationEdge(current,m.alpha1(d))!=Geom::RIGHT) {
			Dart dd_vert = d;
			do {
				d = m.alpha1(d);
				if(m_positions[d][0]==m_positions[m.phi1(d)][0] && m_positions[d][1]==m_positions[m.phi1(d)][1])
					d=m.alpha1(d);
			}while(getOrientationEdge(current,m.alpha1(d))!=Geom::RIGHT && dd_vert!=d);

			if(dd_vert==d) {
				//orbit with 2 edges : point on one edge
				if(m.alpha1(m.alpha1(d))==d) {
				 if(!Algo::Geometry::isPointOnHalfEdge<PFP>(m,d,m_positions,current)) {
					d = m.alpha1(d);
				 }
				}
				else {
    					state = VERTEX_ORBIT;
					return;
				}
			}
		}
		else {
			Dart dd_vert = m.alpha1(d);
			while(getOrientationEdge(current,d)==Geom::RIGHT && dd_vert!=d) {
				d = m.alpha_1(d);

				if(m_positions[d][0]==m_positions[m.phi1(d)][0] && m_positions[d][1]==m_positions[m.phi1(d)][1]) {
					d=m.alpha_1(d);
				}
			}
		}

		//displacement step
// 		if(!obstacle.isMarked(d)) {
			if(getOrientationEdge(current,d)==Geom::ALIGNED && Algo::Geometry::isPointOnHalfEdge<PFP>(m,d,m_positions,current)) {
				edgeState(current);
			}
			else {
				d = m.phi1(d);
				faceState(current);
			}
// 		}
	}
}

template <typename PFP>
void ParticleCell2D<PFP>::edgeState(const VEC3& current, Geom::Orientation2D sideOfEdge)
{
	#ifdef DEBUG
	std::cout << "edgeState" <<  d << std::endl;
	#endif

	assert(std::isfinite(current[0]) && std::isfinite(current[1]) && std::isfinite(current[2]));
// 	assert(Algo::Geometry::isPointOnEdge<PFP>(m,d,m_positions,m_position));

	changeCell = true;
	if(sideOfEdge==Geom::ALIGNED)
		sideOfEdge = getOrientationEdge(current,d);

	switch(sideOfEdge) {
		case Geom::LEFT :
// 							if(!obstacle.isMarked(d)) {
								d = m.phi1(d);
								faceState(current);
								return;
// 							}
		case Geom::RIGHT:
// 							if(!obstacle.isMarked(m.phi2(d))) {
								d = m.phi1(m.phi2(d));
								faceState(current);
								return;
// 							}
		default :   state = EDGE_ORBIT;
	}

	if(!Algo::Geometry::isPointOnHalfEdge<PFP>(m,d,m_positions,current)) {
		prevPos = m_position;
		m_position = m_positions[d];
		vertexState(current);
		return;
	}
	else if(!Algo::Geometry::isPointOnHalfEdge<PFP>(m,m.phi2(d),m_positions,current)) {
		d = m.phi2(d);
		prevPos = m_position;
		m_position = m_positions[d];
		vertexState(current);
		return;
	}
}

template <typename PFP>
void ParticleCell2D<PFP>::faceState(const VEC3& current)
{
	#ifdef DEBUG
	std::cout << "faceState" <<  d << std::endl;
	#endif

 	assert(std::isfinite(m_position[0]) && std::isfinite(m_position[1]) && std::isfinite(m_position[2]));
 	assert(std::isfinite(current[0]) && std::isfinite(current[1]) && std::isfinite(current[2]));
// 	assert(Algo::Geometry::isPointInConvexFace2D<PFP>(m,d,m_positions,m_position,true));

	Dart dd=d;
	float wsoe = getOrientationFace(current,m_position,m.phi1(d));

	//orientation step
	if(wsoe!=Geom::RIGHT) {
		d = m.phi1(d);
		wsoe = getOrientationFace(current,m_position,m.phi1(d));
		while(wsoe!=Geom::RIGHT && dd!=d) {
			d = m.phi1(d);
			wsoe = getOrientationFace(current,m_position,m.phi1(d));
		}

 		//source and position to reach are the same : verify if no edge is crossed due to numerical approximation
		if(dd==d) {
			do {
				switch (getOrientationEdge(current,d)) {
				case Geom::LEFT : 	d=m.phi1(d);
									break;
				case Geom::ALIGNED :m_position = current;
									edgeState(current);
									return;
				case Geom::RIGHT :
									std::cout << "smthg went bad " << m_position << " " << current << std::endl;
									std::cout << "d1 " << m_positions[d] << " d2 " << m_positions[m.phi1(d)] << std::endl;
									m_position = intersectLineEdge(current,m_position,d);
									std::cout << " " << m_position << std::endl;

// 									if(!obstacle.isMarked(m.phi2(d)))
										edgeState(current,Geom::RIGHT);
// 									else 
// 										state = EDGE_ORBIT;
									return;
				}
			} while(d!=dd);
			prevPos = m_position;
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
		//take the orientation with d1 : in case we are going through a vertex
		wsoe = getOrientationFace(current,m_position,d);
	}
	else {
		wsoe = getOrientationFace(current,m_position,d);
		while(wsoe==Geom::RIGHT && m.phi_1(d)!=dd) {
			d = m.phi_1(d);
			wsoe = getOrientationFace(current,m_position,d);
		}

		//in case of numerical incoherence
		if(m.phi_1(d)==dd) {
			d = m.phi_1(d);
			do {
				switch (getOrientationEdge(current,d)) {
				case Geom::LEFT : 			d=m.phi1(d);
									break;
				case Geom::ALIGNED :			
// 									std::cout << "pic" << std::endl;
									m_position = current;
									edgeState(current);
									return;
				case Geom::RIGHT :
									std::cout << "smthg went bad(2) " << m_position << std::endl;
									m_position = intersectLineEdge(current,m_position,d);
// 									std::cout << " " << m_position << std::endl;
// 									if(!obstacle.isMarked(m.phi2(d)))
										edgeState(current,Geom::RIGHT);
// 									else 
// 										state = EDGE_ORBIT;
									return;
				}
			} while(d!=dd);

			m_position = current;
			state = FACE_ORBIT;
			return;
		}
	}

	prevPos = m_position;

	//displacement step
	switch (getOrientationEdge(current,d)) {
	case Geom::LEFT :			m_position = current;
		 				state = FACE_ORBIT;;
		 				break;
// 	case Geom::ALIGNED :			if(wsoe==Geom::ALIGNED) {
// 							m_position = m_positions[d];
// 							vertexState(current);
// 						}
// 						else {
// 							std::cout << "poc" << std::endl;
// 							m_position = current;
// 		 					state = EDGE_ORBIT;
// 						}
// 		 				break;
	default :
						if(wsoe==Geom::ALIGNED) {
							m_position = m_positions[d];
							vertexState(current);
						}
						else {
// 							std::cout << "wsoe : " << wsoe << std::endl;
// 							std::cout << "current " << current << " " << m_position << std::endl;
// 							std::cout << "d " << d << "d1 " << m_positions[d] << " d2 " << m_positions[m.phi2(d)] << std::endl;
							m_position = intersectLineEdge(current,m_position,d);
// 							std::cout << " inter : " << m_position << std::endl;
// 							if(!obstacle.isMarked(m.phi2(d)))
								edgeState(current,Geom::RIGHT);
// 							else
// 								state = EDGE_ORBIT;
						}
	}
}
