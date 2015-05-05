namespace CGoGN
{

namespace SCHNApps
{

template <typename PFP>
bool EdgeSelector_Radiance<PFP>::init()
{
	MAP& m = this->m_map ;

	if(m_positionApproximator.getApproximatedAttributeName() != m_position.name())
	{
		return false;
	}
	if(m_normalApproximator.getApproximatedAttributeName() != m_normal.name())
	{
		return false;
	}
	if(m_radianceApproximator.getApproximatedAttributeName() != m_radiance.name())
	{
		return false;
	}

	m_nb_coefs = SH::get_nb_coefs();

	m_integrator.Init(29) ;

	// init QEM quadrics
	for (Vertex v : allVerticesOf(m))
	{
		Utils::Quadric<REAL> q ;	// create one quadric
		m_quadric[v] = q ;				// per vertex
	}
	for (Face f : allFacesOf(m))
	{
		Dart d = f.dart;
		Dart d1 = m.phi1(d) ;	// for each triangle,
		Dart d_1 = m.phi_1(d) ;	// initialize the quadric of the triangle
		Utils::Quadric<REAL> q(m_position[d], m_position[d1], m_position[d_1]) ;
		m_quadric[d] += q ;		// and add the contribution of
		m_quadric[d1] += q ;	// this quadric to the ones
		m_quadric[d_1] += q ;	// of the 3 incident vertices
	}

	// init multimap for each Half-edge
	edges.clear() ;

	for (Edge e : allEdgesOf(m))
	{
		initEdgeInfo(e) ;	// init the edges with their optimal position
	}						// and insert them in the multimap according to their error

	cur = edges.begin() ; 	// init the current edge to the first one

	return true ;
}

template <typename PFP>
bool EdgeSelector_Radiance<PFP>::nextEdge(Dart& d)  const
{
	if(cur == edges.end() || edges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void EdgeSelector_Radiance<PFP>::updateBeforeCollapse(Dart d)
{
	MAP& m = this->m_map ;

	Dart dd = m.phi2(d);

	EdgeInfo& edgeE = edgeInfo[d];
	if(edgeE.valid)
	{
		edgeE.valid = false ;
		edges.erase(edgeE.it) ;
	}
	edgeE = edgeInfo[m.phi1(d)];
	if(edgeE.valid)
	{
		edgeE.valid = false ;
		edges.erase(edgeE.it) ;
	}
	edgeE = edgeInfo[m.phi_1(d)];
	if(edgeE.valid)
	{
		edgeE.valid = false ;
		edges.erase(edgeE.it) ;
	}
	edgeE = edgeInfo[m.phi1(dd)];
	if(edgeE.valid)
	{
		edgeE.valid = false ;
		edges.erase(edgeE.it) ;
	}
	edgeE = edgeInfo[m.phi_1(dd)];
	if(edgeE.valid)
	{
		edgeE.valid = false ;
		edges.erase(edgeE.it) ;
	}
}

template <typename PFP>
void EdgeSelector_Radiance<PFP>::recomputeQuadric(const Dart d)
{
	MAP& m = this->m_map;
	Utils::Quadric<REAL>& q = m_quadric[d];
	q.zero() ;
	Traversor2VF<MAP> tf(m, d);
	for (Dart f = tf.begin() ; f != tf.end() ; f = tf.next())
	{
		q += Utils::Quadric<REAL>(m_position[f], m_position[m.phi1(f)], m_position[m.phi_1(f)]) ;
	}
}

template <typename PFP>
void EdgeSelector_Radiance<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;

	CellMarkerStore<MAP, EDGE> em(m);

	initEdgeInfo(d2);
	initEdgeInfo(dd2);
	em.mark(d2);
	em.mark(dd2);

	recomputeQuadric(d2);

	Traversor2VVaE<MAP> tv(m, d2);
	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		recomputeQuadric(v);
		Traversor2VE<MAP> tve(m, v);
		for (Dart e = tve.begin() ; e != tve.end() ; e = tve.next())
		{
			if (!em.isMarked(e))
			{
				updateEdgeInfo(e);
				em.mark(e);
			}
		}
	}

	cur = edges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void EdgeSelector_Radiance<PFP>::initEdgeInfo(Dart d)
{
	MAP& m = this->m_map ;
	EdgeInfo einfo ;

	if(m.edgeCanCollapse(d))
		computeEdgeInfo(d, einfo) ;
	else
		einfo.valid = false ;

	edgeInfo[d] = einfo ;
}

template <typename PFP>
void EdgeSelector_Radiance<PFP>::updateEdgeInfo(Dart d)
{
	MAP& m = this->m_map ;
	EdgeInfo& einfo = edgeInfo[d] ;

	if (einfo.valid)
		edges.erase(einfo.it);

	if (m.edgeCanCollapse(d))
		computeEdgeInfo(d, einfo);
	else
		einfo.valid = false;
}

template <typename PFP>
void EdgeSelector_Radiance<PFP>::computeEdgeInfo(Dart d, EdgeInfo& einfo)
{
	MAP& m = this->m_map ;
	Dart dd = m.phi1(d) ;

	// compute all approximated attributes
	m_positionApproximator.approximate(d) ;
	m_normalApproximator.approximate(d) ;
	m_radianceApproximator.approximate(d) ;

	// Get all approximated attributes
	// New position
	const VEC3& newPos = m_positionApproximator.getApprox(d) ;
	const VEC3& newN = m_normalApproximator.getApprox(d) ;
	const SH& newRad = m_radianceApproximator.getApprox(d) ;

	const Dart& v0 = dd ;
	const Dart& v1 = d ;

	// Compute errors
	Utils::Quadric<REAL> quadGeom ;
	quadGeom += m_quadric[v0] ;	// compute the sum of the
	quadGeom += m_quadric[v1] ;	// two vertices quadrics
	const REAL geom = quadGeom(newPos) ;

	const REAL rad = computeRadianceError(d, newPos, newN, newRad);

	const REAL t = 0.01f ;
	const REAL err = t*geom + (1-t)*rad ;

	// Check if errated values appear
	if (err < -1e-6)
		einfo.valid = false ;
	else
	{
		einfo.it = this->edges.insert(std::make_pair(std::max(err, REAL(0)), d)) ;
		einfo.valid = true ;
	}
}

template <typename PFP>
typename PFP::REAL EdgeSelector_Radiance<PFP>::computeRadianceError(Dart d, const VEC3& p, const VEC3& n, const SH& r)
{
	MAP& m = this->m_map ;

	Dart d2 = m.phi2(d);

	const VEC3& p0 = m_position[d2];
	VEC3& n0 = m_normal[d2];
	const SH& r0 = m_radiance[d2];

	const VEC3& p1 = m_position[d];
	VEC3& n1 = m_normal[d];
	const SH& r1 = m_radiance[d];

	REAL error(0);

	Dart it1 = m.phi2(m.phi_1(d2));
	Dart it2 = m.phi2(m.phi_1(it1));
	while (it2 != d2)
	{
		const VEC3& p2 = m_position[m.phi1(it1)];
		const SH& r2 = m_radiance[m.phi1(it1)];

		const VEC3& p3 = m_position[m.phi1(it2)];
		const SH& r3 = m_radiance[m.phi1(it2)];

		VEC3 e = p0 - p;
		VEC3 e2 = p2 - p;
		VEC3 e3 = p3 - p;
		REAL tArea = Geom::triangleArea(p, p2, p3);
		REAL alpha2 = ((e3 * e3) * (e2 * e) - (e2 * e3) * (e3 * e)) / (4. * tArea * tArea) ;
		REAL alpha3 = ((e2 * e2) * (e3 * e) - (e2 * e3) * (e2 * e)) / (4. * tArea * tArea) ;
		REAL alpha1 = 1. - alpha2 - alpha3;

		SH diffRad(r0);
		diffRad -= (r * alpha1 + r2 * alpha2 + r3 * alpha3);

		double integral;
		double area;
		m_integrator.Compute(&integral, &area, EdgeSelector_Radiance<PFP>::SHEvalCartesian_Error, &diffRad, EdgeSelector_Radiance<PFP>::isInHemisphere, n0.data());

		error += tArea * integral / area;

		it1 = it2;
		it2 = m.phi2(m.phi_1(it1));
	}

	it1 = m.phi2(m.phi_1(d));
	it2 = m.phi2(m.phi_1(it1));
	while (it2 != d)
	{
		const VEC3& p2 = m_position[m.phi1(it1)];
		const SH& r2 = m_radiance[m.phi1(it1)];

		const VEC3& p3 = m_position[m.phi1(it2)];
		const SH& r3 = m_radiance[m.phi1(it2)];

		VEC3 e = p1 - p;
		VEC3 e2 = p2 - p;
		VEC3 e3 = p3 - p;
		REAL tArea = Geom::triangleArea(p, p2, p3);
		REAL alpha2 = ((e3 * e3) * (e2 * e) - (e2 * e3) * (e3 * e)) / (4. * tArea * tArea) ;
		REAL alpha3 = ((e2 * e2) * (e3 * e) - (e2 * e3) * (e2 * e)) / (4. * tArea * tArea) ;
		REAL alpha1 = 1. - alpha2 - alpha3;

		SH diffRad(r1);
		diffRad -= (r * alpha1 + r2 * alpha2 + r3 * alpha3);

		double integral;
		double area;
		m_integrator.Compute(&integral, &area, EdgeSelector_Radiance<PFP>::SHEvalCartesian_Error, &diffRad, EdgeSelector_Radiance<PFP>::isInHemisphere, n1.data());

		error += tArea * integral / area;

		it1 = it2;
		it2 = m.phi2(m.phi_1(it1));
	}

	return error;
}

} // namespace SCHNApps

} // namespace CGoGN
