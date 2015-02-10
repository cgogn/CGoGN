namespace CGoGN
{

namespace SCHNApps
{

template <typename PFP>
bool HalfEdgeSelector_Radiance<PFP>::init()
{
	MAP& m = this->m_map ;

	assert(m_positionApproximator.getType() == Algo::Surface::Decimation::A_hQEM
		|| m_positionApproximator.getType() == Algo::Surface::Decimation::A_hHalfCollapse
		|| !"Approximator for selector (HalfEdgeSelector_Radiance) must be of a half-edge approximator") ;

	assert(m_normalApproximator.getType() == Algo::Surface::Decimation::A_hQEM
		|| m_normalApproximator.getType() == Algo::Surface::Decimation::A_hHalfCollapse
		|| !"Approximator for selector (HalfEdgeSelector_Radiance) must be of a half-edge approximator") ;

	assert(m_radianceApproximator.getType() == Algo::Surface::Decimation::A_hQEM
		|| m_radianceApproximator.getType() == Algo::Surface::Decimation::A_hHalfCollapse
		|| !"Approximator for selector (HalfEdgeSelector_Radiance) must be of a half-edge approximator") ;

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
	halfEdges.clear() ;

	for(Dart d = m.begin() ; d != m.end() ; m.next(d))
	{
		initHalfEdgeInfo(d) ;	// init the edges with their optimal position
	}							// and insert them in the multimap according to their error

	cur = halfEdges.begin() ; 	// init the current edge to the first one

	return true ;
}

template <typename PFP>
bool HalfEdgeSelector_Radiance<PFP>::nextEdge(Dart& d)  const
{
	if(cur == halfEdges.end() || halfEdges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void HalfEdgeSelector_Radiance<PFP>::updateBeforeCollapse(Dart d)
{
	MAP& m = this->m_map ;

	Dart dd = m.phi2(d);

	HalfEdgeInfo& edgeE = halfEdgeInfo[d];
	if(edgeE.valid)
	{
		edgeE.valid = false ;
		halfEdges.erase(edgeE.it) ;
	}
	edgeE = halfEdgeInfo[dd];
	if(edgeE.valid)
	{
		edgeE.valid = false ;
		halfEdges.erase(edgeE.it) ;
	}
	edgeE = halfEdgeInfo[m.phi1(d)];
	if(edgeE.valid)
	{
		edgeE.valid = false ;
		halfEdges.erase(edgeE.it) ;
	}
	edgeE = halfEdgeInfo[m.phi_1(d)];
	if(edgeE.valid)
	{
		edgeE.valid = false ;
		halfEdges.erase(edgeE.it) ;
	}
	edgeE = halfEdgeInfo[m.phi1(dd)];
	if(edgeE.valid)
	{
		edgeE.valid = false ;
		halfEdges.erase(edgeE.it) ;
	}
	edgeE = halfEdgeInfo[m.phi_1(dd)];
	if(edgeE.valid)
	{
		edgeE.valid = false ;
		halfEdges.erase(edgeE.it) ;
	}
}

template <typename PFP>
void HalfEdgeSelector_Radiance<PFP>::recomputeQuadric(const Dart d)
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
void HalfEdgeSelector_Radiance<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;

	Dart stop = m.phi2(m.phi_1(d2));

	recomputeQuadric(d2);
	Dart it = dd2;
	do
	{
		recomputeQuadric(m.phi1(it));
		it = m.phi2(m.phi_1(it));
	} while (it != stop);

	DartMarkerStore<MAP> dm(m);

	Traversor2VVaE<MAP> tv(m, d2);
	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		dm.mark(v);
		updateHalfEdgeInfo(v, true);
	}
	it = dd2;
	do
	{
		Traversor2VVaE<MAP> tv2(m, m.phi1(it));
		for (Dart v = tv2.begin() ; v != tv2.end() ; v = tv2.next())
		{
			dm.mark(v);
			updateHalfEdgeInfo(v, true);
		}
		it = m.phi2(m.phi_1(it));
	} while (it != stop);

	Traversor2VE<MAP> te(m, d2);
	for (Dart v = te.begin() ; v != te.end() ; v = te.next())
	{
		if (!dm.isMarked(v))
			updateHalfEdgeInfo(v, false);
	}
	it = dd2;
	do
	{
		Traversor2VE<MAP> te2(m, m.phi1(it));
		for (Dart v = te2.begin() ; v != te2.end() ; v = te2.next())
		{
			if (!dm.isMarked(v))
				updateHalfEdgeInfo(v, false);
		}
		it = m.phi2(m.phi_1(it));
	} while (it != stop);

	cur = halfEdges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void HalfEdgeSelector_Radiance<PFP>::initHalfEdgeInfo(Dart d)
{
	MAP& m = this->m_map ;
	HalfEdgeInfo heinfo ;
	if(m.edgeCanCollapse(d))
		computeHalfEdgeInfo(d, heinfo) ;
	else
		heinfo.valid = false ;

	halfEdgeInfo[d] = heinfo ;
}

template <typename PFP>
void HalfEdgeSelector_Radiance<PFP>::updateHalfEdgeInfo(Dart d, bool recompute)
{
	MAP& m = this->m_map ;
	HalfEdgeInfo& heinfo = halfEdgeInfo[d] ;

	if(recompute)
	{
		if (heinfo.valid)
			halfEdges.erase(heinfo.it);
		if (m.edgeCanCollapse(d))
			computeHalfEdgeInfo(d, heinfo);
		else
			heinfo.valid = false;
	}
	else
	{
		if (m.edgeCanCollapse(d))
		{									// if the edge can be collapsed now
			if (!heinfo.valid)				// but it was not before
				computeHalfEdgeInfo(d, heinfo) ;
		}
		else
		{									// if the edge cannot be collapsed now
			if (heinfo.valid)				// and it was before
			{
				halfEdges.erase(heinfo.it) ;
				heinfo.valid = false ;
			}
		}
	}
}

template <typename PFP>
void HalfEdgeSelector_Radiance<PFP>::computeHalfEdgeInfo(Dart d, HalfEdgeInfo& heinfo)
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

	// WARNING : in the following, we consider the half-edge contraction v0 --> v1
	const Dart& v0 = dd ;
	const Dart& v1 = d ;
	assert(newPos == m_position[v1]) ;

	// Compute errors
	Utils::Quadric<REAL> quadGeom ;
	quadGeom += m_quadric[v0] ;	// compute the sum of the
	quadGeom += m_quadric[v1] ;	// two vertices quadrics
	const REAL geom = quadGeom(newPos) ;

	const REAL rad = computeRadianceError(d);

	const REAL t = 0.01 ;
	const REAL err = t*geom + (1-t)*rad ;

	// Check if errated values appear
	if (err < -1e-6)
		heinfo.valid = false ;
	else
	{
		heinfo.it = this->halfEdges.insert(std::make_pair(std::max(err, REAL(0)), d)) ;
		heinfo.valid = true ;
	}
}

template <typename PFP>
typename PFP::REAL HalfEdgeSelector_Radiance<PFP>::computeRadianceError(Dart d)
{
	MAP& m = this->m_map ;

	Dart d2 = m.phi2(d);

	const VEC3& p0 = m_position[d2];
	VEC3& n0 = m_normal[d2];
	const SH& r0 = m_radiance[d2];

	const VEC3& p1 = m_position[d];
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

		VEC3 e = p0 - p1;
		VEC3 e2 = p2 - p1;
		VEC3 e3 = p3 - p1;
		REAL tArea = Geom::triangleArea(p1, p2, p3);
		REAL alpha2 = ((e3 * e3) * (e2 * e) - (e2 * e3) * (e3 * e)) / (4. * tArea * tArea) ;
		REAL alpha3 = ((e2 * e2) * (e3 * e) - (e2 * e3) * (e2 * e)) / (4. * tArea * tArea) ;
		REAL alpha1 = 1. - alpha2 - alpha3;

		SH diffRad(r0);
		diffRad -= (r1 * alpha1 + r2 * alpha2 + r3 * alpha3);

		double integral;
		double area;
		m_integrator.Compute(&integral, &area, HalfEdgeSelector_Radiance<PFP>::SHEvalCartesian_Error, &diffRad, HalfEdgeSelector_Radiance<PFP>::isInHemisphere, n0.data());

		error += tArea * integral / area;

		it1 = it2;
		it2 = m.phi2(m.phi_1(it1));
	}

	return error;
}

} // namespace SCHNApps

} // namespace CGoGN
