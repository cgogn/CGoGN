//#define DEBUG

namespace CGoGN
{

namespace Algo
{

namespace MovingObjects
{

template <typename PFP>
void ParticleCell2DAndHalf<PFP>::display()
{
// 	CGoGNout << "pos : " << this->m_position << CGoGNendl;
// 	CGoGNout << "d : " << this->d << CGoGNendl;
// 	CGoGNout << "state : " << this->state << CGoGNendl;
}

template <typename PFP>
typename PFP::VEC3 ParticleCell2DAndHalf<PFP>::pointInFace(Dart d)
{
	const VEC3& p1(m_positions[d]) ;
	Dart dd = m.phi1(d) ;
	const VEC3& p2(m_positions[dd]) ;
	dd = m.phi1(dd) ;
	VEC3& p3(m_positions[dd]) ;

	VEC3 v1(p2 - p1) ;

	while ((v1 ^ VEC3(p3 - p1)).norm2() == 0.0f)
	{
		dd = m.phi1(dd) ;
		p3 = m_positions[dd] ;
	}

	CGoGNout << "pointInFace " << (p1 + p3) * 0.5f << CGoGNendl ;

	return (p1 + p3) * 0.5f ;
}

template <typename PFP>
Geom::Orientation3D ParticleCell2DAndHalf<PFP>::getOrientationEdge(const VEC3& point, Dart d)
{
	const VEC3& endPoint = m_positions[m.phi2(d)];
	const VEC3& vertexPoint = m_positions[d];

	const VEC3& n1 = Algo::Geometry::faceNormal<PFP>(m, d, m_positions);

	//orientation relative to the plane orthogonal to the face going through the edge
	return Geom::testOrientation3D(point, vertexPoint, endPoint, vertexPoint+n1);
}

template <typename PFP>
typename PFP::VEC3 ParticleCell2DAndHalf<PFP>::intersectLineEdge(const VEC3& pA, const VEC3& pB, Dart d)
{
	const VEC3& q1 = m_positions[d];
	const VEC3& q2 = m_positions[m.phi2(d)];
	VEC3 Inter;

	VEC3 n1 = Algo::Geometry::faceNormal<PFP>(m,d,m_positions);
	VEC3 n = (q2 - q1) ^ n1 ;

	Geom::intersectionLinePlane(pA, pB - pA, q1, n, Inter) ;

	Geom::Plane3D<float> pl = Algo::Geometry::facePlane<PFP>(m,d,m_positions);
	pl.project(Inter);

	return Inter;
}

template <typename PFP>
Geom::Orientation3D ParticleCell2DAndHalf<PFP>::getOrientationFace(VEC3 point, VEC3 sourcePoint, Dart d)
{
	const VEC3& dPoint = m_positions[d];

	VEC3 n1 = Algo::Geometry::faceNormal<PFP>(m, d, m_positions);

	return Geom::testOrientation3D(point, sourcePoint, dPoint+n1, dPoint);
}

template <typename PFP>
void ParticleCell2DAndHalf<PFP>::vertexState(VEC3 current)
{
	#ifdef DEBUG
	CGoGNout << "vertexState" << d << CGoGNendl;
	#endif
	assert(std::isfinite(current[0]) && std::isfinite(current[1]) && std::isfinite(current[2]));

	crossCell = CROSS_OTHER;

	if(Algo::Geometry::isPointOnVertex<PFP>(m,d,m_positions,current))
	{
		state = VERTEX;
		m_position = current;
		return;
	}
	else
	{
		//orientation step
		if(m_positions[d][0] == m_positions[m.phi1(d)][0] && m_positions[d][1] == m_positions[m.phi1(d)][1])
			d = m.alpha1(d);
		if(getOrientationEdge(current,m.alpha1(d)) != Geom::UNDER)
		{
			Dart dd_vert = d;
			do
			{
				d = m.alpha1(d);
				if(m_positions[d][0] == m_positions[m.phi1(d)][0] && m_positions[d][1] == m_positions[m.phi1(d)][1])
					d = m.alpha1(d);
			} while(getOrientationEdge(current, m.alpha1(d)) != Geom::UNDER && dd_vert != d);

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
					m_position = current;
					state = VERTEX;
					return;
				}
			}
		}
		else
		{
			Dart dd_vert = m.alpha1(d);
			while(getOrientationEdge(current, d) == Geom::OVER && dd_vert != d)
			{
				d = m.alpha_1(d);
				if(m_positions[d][0] == m_positions[m.phi1(d)][0] && m_positions[d][1] == m_positions[m.phi1(d)][1])
					d = m.alpha_1(d);
			}
		}

		//displacement step
		if(getOrientationEdge(current, d) == Geom::ON && Algo::Geometry::isPointOnHalfEdge<PFP>(m, d, m_positions, current))
			edgeState(current);
		else
		{
			d = m.phi1(d);
			faceState(current);
		}
	}
}

template <typename PFP>
void ParticleCell2DAndHalf<PFP>::edgeState(VEC3 current, Geom::Orientation3D sideOfEdge)
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

	if(sideOfEdge == Geom::ON)
		sideOfEdge = getOrientationEdge(current, d);

	switch(sideOfEdge)
	{
		case Geom::UNDER :
			d = m.phi1(d);
			faceState(current);
			return;
		case Geom::OVER:
		{
			//transform the displacement into the new entered face
			VEC3 displ = current-m_position;
			VEC3 edge = Algo::Geometry::vectorOutOfDart<PFP>(m,m.phi2(d),m_positions);

			VEC3 n1 = Algo::Geometry::faceNormal<PFP>(m,d,m_positions);
			VEC3 n2 = Algo::Geometry::faceNormal<PFP>(m,m.phi2(d),m_positions);

			float angle = acos(n1*n2);

			Geom::Matrix<4,4,float> mRot;
			mRot.identity();

			Geom::rotate(edge[0],edge[1],edge[2],angle,mRot);

			displ = Geom::transform(displ,mRot);
			current = m_position+displ;

			d = m.phi1(m.phi2(d));
			faceState(current);
			return;
		}
		default :
			state = EDGE;
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

	m_position = current;
}

template <typename PFP>
void ParticleCell2DAndHalf<PFP>::faceState(VEC3 current)
{
	#ifdef DEBUG
	CGoGNout << "faceState" <<  d << CGoGNendl;
	#endif

 	assert(std::isfinite(m_position[0]) && std::isfinite(m_position[1]) && std::isfinite(m_position[2]));
 	assert(std::isfinite(current[0]) && std::isfinite(current[1]) && std::isfinite(current[2]));
// 	assert(Algo::Geometry::isPointInConvexFace2D<PFP>(m,d,m_positions,m_position,true));

	//project current within face plane
	VEC3 n1 = Algo::Geometry::faceNormal<PFP>(m,d,m_positions);
	VEC3 n2 = current - m_position;
//	n1.normalize();
	VEC3 n3 = n1 ^ n2;
	n3.normalize();
	VEC3 n4 = n3 ^ n1;
	current = m_position + (n2 * n4) * n4;

	//track new position within map
	Dart dd = d;
	float wsoe = getOrientationFace(current, m_position, m.phi1(d));

	// orientation step
	if(wsoe != Geom::UNDER)
	{
		d = m.phi1(d);
		wsoe = getOrientationFace(current, m_position, m.phi1(d));
		while(wsoe != Geom::UNDER && dd != d)
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
				case Geom::UNDER: 	d = m.phi1(d);
									break;
				case Geom::ON:	m_position = current;
									edgeState(current);
									return;
				case Geom::OVER:
//									CGoGNout << "smthg went bad " << m_position << " " << current << CGoGNendl;
//									CGoGNout << "d1 " << m_positions[d] << " d2 " << m_positions[m.phi1(d)] << CGoGNendl;
									m_position = intersectLineEdge(current, m_position, d);
//									CGoGNout << " " << m_position << CGoGNendl;

									edgeState(current,Geom::OVER);
									return;
				}
			} while(d != dd);
			m_position = current;
			state = FACE;

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
		while(wsoe == Geom::UNDER && m.phi_1(d) != dd)
		{
			d = m.phi_1(d);
			wsoe = getOrientationFace(current, m_position, d);
		}

		// in case of numerical incoherence
		if(m.phi_1(d) == dd && wsoe == Geom::UNDER)
		{
			d = m.phi_1(d);
			do
			{
				switch (getOrientationEdge(current, d))
				{
				case Geom::UNDER :
					d = m.phi1(d);
					break;
				case Geom::ON :
// 					CGoGNout << "pic" << CGoGNendl;
					m_position = current;
					edgeState(current);
					return;
				case Geom::OVER:
//					CGoGNout << "smthg went bad(2) " << m_position << CGoGNendl;
					m_position = intersectLineEdge(current, m_position, d);
// 					CGoGNout << " " << m_position << CGoGNendl;
					edgeState(current, Geom::OVER);
					return;
				}
			} while(d != dd);

			m_position = current;
			state = FACE;
			return;
		}
	}

	//displacement step
	switch (getOrientationEdge(current, d))
	{
	case Geom::UNDER :
		m_position = current;
		state = FACE;
		break;
	default :
		if(wsoe == Geom::ON)
		{
			std::cout << __FILE__ << " to uncomment and check" << std::endl;
//			d = m.phi1(d); //to check
//			m_position = m_positions[d];
//
//			vertexState(current);
		}
//		else
		{
// 			CGoGNout << "wsoe : " << wsoe << CGoGNendl;
// 			CGoGNout << "current " << current << " " << m_position << CGoGNendl;
// 			CGoGNout << "d " << d << "d1 " << m_positions[d] << " d2 " << m_positions[m.phi2(d)] << CGoGNendl;
			m_position = intersectLineEdge(current, m_position, d);
// 			CGoGNout << " inter : " << m_position << CGoGNendl;
			edgeState(current, Geom::OVER);
		}
	}
}

} // namespace MovingObjects

} // namespace Algo

} // namespace CGoGN
