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

	unsigned int ruleId = m_nb_coefs <= 6 ? 17 : (m_nb_coefs <= 10) ? 23 : 29 ;
	m_integrator.Init(ruleId) ;

	// allocate workspaces
	m_workspace = new double[60+12*m_nb_coefs] ;

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

	const Dart& v0 = m.phi1(d) ;

	Traversor2VVaE<MAP> tv(m, v0) ;
	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		Traversor2VE<MAP> te(m, v) ;
		for (Dart he = te.begin() ; he != te.end() ; he = te.next())
		{
			HalfEdgeInfo* edgeE = &(halfEdgeInfo[he]) ;
			if(edgeE->valid)
			{
				edgeE->valid = false ;
				halfEdges.erase(edgeE->it) ;
			}
			Dart de = m.phi2(he) ;
			edgeE = &(halfEdgeInfo[de]) ;
			if(edgeE->valid)
			{
				edgeE->valid = false ;
				halfEdges.erase(edgeE->it) ;
			}
		}
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
void HalfEdgeSelector_Radiance<PFP>::updateAfterCollapse(Dart d2, Dart)
{
	MAP& m = this->m_map ;

	const Dart& v1 = d2 ;

	recomputeQuadric(v1) ;
	Traversor2VVaE<MAP> tv(m, v1) ;
	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		recomputeQuadric(v) ;
	}

	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		Traversor2VE<MAP> te(m,v) ;
		for (Dart e = te.begin() ; e != te.end() ; e = te.next())
		{
			updateHalfEdgeInfo(e) ;
			updateHalfEdgeInfo(m.phi2(e)) ;
		}
	}

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
void HalfEdgeSelector_Radiance<PFP>::updateHalfEdgeInfo(Dart d)
{
	MAP& m = this->m_map ;
	HalfEdgeInfo& heinfo = halfEdgeInfo[d] ;

	if(!heinfo.valid && m.edgeCanCollapse(d))
		computeHalfEdgeInfo(d, heinfo) ;
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
	quadGeom += m_quadric[d] ;	// compute the sum of the
	quadGeom += m_quadric[dd] ;	// two vertices quadrics
	const REAL geom = quadGeom(newPos) ;

	const REAL rad = 0;
//	const REAL rad = computeDirDerivativeLFerror(v0,v1).norm()/sqrt(3) ;
//	const REAL rad = computeGradientLFerror(v0,v1).norm()/sqrt(3) ;

	const REAL t = 1.0 ;
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
/*
template <typename PFP>
VEC3 HalfEdgeSelector_Radiance<PFP>::computeGradientLFerror(const Dart& v0, const Dart& v1) const
{
	MAP& m = this->m_map ;

	const VEC3& P0 = this->m_approx[m_approxindex_pos]->getAttr(m_attrindex_pos)[v0] ;
	const VEC3& P1 = this->m_approx[m_approxindex_pos]->getAttr(m_attrindex_pos)[v1] ;
	const VEC3& N = this->m_approx[m_approxindex_FN]->getApprox(v1,m_attrindex_FN) ;

	const VEC3& T1 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[v1] ;
	const VEC3& B1 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[v1] ;
	const VEC3& N1 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[v1] ;
	VEC3 coefs1[m_K] ;
	for (unsigned int k = 0 ; k < m_K ; ++k)
	{
		coefs1[k] = this->m_approx[m_approxindex_HF[k]]->getAttr(m_attrindex_HF[k])[v1] ;
	}

	Traversor2VF<MAP> tf(m,v0) ; // all faces around vertex v0

	VEC3 count ;
	REAL areaSum = 0 ;
	const VEC3 d = P1 - P0 ; // displacement vector

	for (Dart fi = tf.begin() ; fi != tf.end() ; fi = tf.next()) // foreach "blue" face
	{
		// get the data
		const Dart& vi = m.phi1(fi) ;
		const Dart& vj = m.phi_1(fi) ;
		const VEC3& Pi = this->m_position[vi] ;
		const VEC3& Pj = this->m_position[vj] ;

		const VEC3& Ti = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[vi] ;
		const VEC3& Bi = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[vi] ;
		const VEC3& Ni = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[vi] ;
		const VEC3& Tj = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[vj] ;
		const VEC3& Bj = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[vj] ;
		const VEC3& Nj = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[vj] ;
		VEC3 coefsi[m_K], coefsj[m_K] ;

		for (unsigned int k = 0 ; k < m_K ; ++k)
		{
			coefsi[k] = this->m_approx[m_approxindex_HF[k]]->getAttr(m_attrindex_HF[k])[vi] ;
			coefsj[k] = this->m_approx[m_approxindex_HF[k]]->getAttr(m_attrindex_HF[k])[vj] ;
		}

		// utils
		const VEC3 ei = P0 - Pj ;
		const VEC3 ej = Pi - P0 ;
		//const VEC3 e0 = Pj - Pi ;

		const REAL areaIJ0sq = (ei ^ ej).norm2() ;
		const REAL areaIJ0 = sqrt(areaIJ0sq)/2. ;
		areaSum += areaIJ0 ;

		const VEC3 displacementE = integrateDscalGrad(d, m_K, N, ei, ej,
													  coefs1, T1, B1, N1, m_avgColor[v1],
													  coefsi, Ti, Bi, Ni, m_avgColor[vi],
													  coefsj, Tj, Bj, Nj, m_avgColor[vj]) ;

		count += displacementE * areaIJ0 ;
	}

	const VEC3 lfDiff = computeSquaredLightfieldDifferenceNumerical(v0,v1,N) ;

	return lfDiff*areaSum + count ;
}

template <typename PFP>
VEC3 HalfEdgeSelector_Radiance<PFP>::computeDirDerivativeLFerror(const Dart& v0, const Dart& v1)
{
	MAP& m = this->m_map ;

	const VEC3& P0 = this->m_approx[m_approxindex_pos]->getAttr(m_attrindex_pos)[v0] ;
	const VEC3& P1 = this->m_approx[m_approxindex_pos]->getAttr(m_attrindex_pos)[v1] ;
	const VEC3& N = this->m_approx[m_approxindex_FN]->getApprox(v1,m_attrindex_FN) ;

	const VEC3& T1 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[v1] ;
	const VEC3& B1 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[v1] ;
	const VEC3& N1 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[v1] ;
	const VEC3& T0 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[v0] ;
	const VEC3& B0 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[v0] ;
	const VEC3& N0 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[v0] ;
	std::vector<VEC3*> coefs0, coefs1 ;
	coefs0.resize(m_K) ;
	coefs1.resize(m_K) ;
	for (unsigned int k = 0 ; k < m_K ; ++k)
	{
		coefs0[k] = &(this->m_approx[m_approxindex_HF[k]]->getAttr(m_attrindex_HF[k])[v0]) ;
		coefs1[k] = &(this->m_approx[m_approxindex_HF[k]]->getAttr(m_attrindex_HF[k])[v1]) ;
	}

	Traversor2VF<MAP> tf(m,v0) ; // all faces around vertex v0

	VEC3 error ;
	const VEC3 d = P1 - P0 ; // displacement vector

	std::vector<VEC3*> coefsi, coefsj ;
	coefsi.resize(m_K) ;
	coefsj.resize(m_K) ;

	for (Dart fi = tf.begin() ; fi != tf.end() ; fi = tf.next()) // foreach "blue" face
	{
		// get the data
		const Dart& vi = m.phi1(fi) ;
		const Dart& vj = m.phi_1(fi) ;
		const VEC3& Pi = this->m_position[vi] ;
		const VEC3& Pj = this->m_position[vj] ;

		const VEC3& Ti = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[vi] ;
		const VEC3& Bi = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[vi] ;
		const VEC3& Ni = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[vi] ;
		const VEC3& Tj = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[vj] ;
		const VEC3& Bj = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[vj] ;
		const VEC3& Nj = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[vj] ;

		//VEC3 coefsi[m_K], coefsj[m_K] ;
		for (unsigned int k = 0 ; k < m_K ; ++k)
		{
			coefsi[k] = &(this->m_approx[m_approxindex_HF[k]]->getAttr(m_attrindex_HF[k])[vi]) ;
			coefsj[k] = &(this->m_approx[m_approxindex_HF[k]]->getAttr(m_attrindex_HF[k])[vj]) ;
		}

		// utils
		const VEC3 ei = P1 - Pj ;
		const VEC3 ej = Pi - P1 ;

		// area x integration
		error += (ei ^ ej).norm()/2 * integrateDlf(d, m_K, N, ei, ej,
													  coefs0, T0, B0, N0, m_avgColor[v0],
													  coefs1, T1, B1, N1, m_avgColor[v1],
													  coefsi, Ti, Bi, Ni, m_avgColor[vi],
													  coefsj, Tj, Bj, Nj, m_avgColor[vj]) ;
	}

	return error ;
}

template <typename PFP>
VEC3 HalfEdgeSelector_Radiance<PFP>::computeSquaredLightfieldDifferenceNumerical(const Dart& v0, const Dart& v1, const VEC3& N) const
{
	// get two frames
	const VEC3& T1 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[v0] ;
	const VEC3& T2 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[v1] ;
	const VEC3& B1 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[v0] ;
	const VEC3& B2 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[v1] ;
	const VEC3& N1 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[v0] ;
	const VEC3& N2 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[v1] ;

	// get coefs of v1 and v2
	std::vector<VEC3> coefs1, coefs2 ;
	coefs1.resize(m_K) ; coefs2.resize(m_K) ;
	for (unsigned int i = 0 ; i < m_K ; ++i)
	{
		coefs1[i] = this->m_approx[m_approxindex_HF[i]]->getAttr(m_attrindex_HF[i])[v0] ;
		coefs2[i] = this->m_approx[m_approxindex_HF[i]]->getAttr(m_attrindex_HF[i])[v1] ;
	}

	// fill workspace
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[i] = double(N1[i]) ; // n
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[4+i] = double(T1[i]) ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[7+i] = double(B1[i]) ;
	m_workspace[10] = double(m_K) ;

	// fill workspace 2
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[11+m_K+i] = double(N2[i]) ; // n
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[15+m_K+i] = double(T2[i]) ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[18+m_K+i] = double(B2[i]) ;
	m_workspace[21+m_K] = double(m_K) ;

	// Get new frame
	// precondition : this->m_approx[m_approxindex_FN]->approximate should have been called !
	//const VEC3& N = this->m_approx[m_approxindex_FN]->getApprox(v1,m_attrindex_FN) ; // get new N
	//assert(N == N2 || !"HalfEdgeSelector_Radiance<PFP>::computeSquaredLightfieldDifferenceNumerical: only works with half-collapse") ;
	m_n[0] = N[0] ;
	m_n[1] = N[1] ;
	m_n[2] = N[2] ;

	// call integral function

	double integ, ar ;
	VEC3 integral ;
	VEC3 area ;
	for (unsigned int channel = 0 ; channel < 3 ; ++channel)
	{
		m_workspace[3] = m_avgColor[v0][channel];
		m_workspace[14+m_K] = m_avgColor[v1][channel];
		for (unsigned int i = 0 ; i < m_K ; ++i)
		{
			m_workspace[11+i] = coefs1[i][channel] ;
			m_workspace[22+m_K+i] = coefs2[i][channel] ;
		}
		m_integrator.Compute(&integ, &ar, &SquaredDifferenceOfCartesianFunctions, m_workspace, &isInDomain, m_n) ;
		integral[channel] = integ/(4*M_PI) ;
		area[channel] = ar ;
	}

	return integral ;
}

template <typename PFP>
VEC3 HalfEdgeSelector_Radiance<PFP>::computeSquaredLightfieldDifferenceAnalytical(const Dart& v0, const Dart& v1) const
{
	// get two frames
	const VEC3& T1 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[v0] ;
	const VEC3& T2 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[v1] ;
	const VEC3& B1 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[v0] ;
	const VEC3& B2 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[v1] ;
	const VEC3& N1 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[v0] ;
	const VEC3& N2 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[v1] ;

	// Get new frame
	// precondition : this->m_approx[m_approxindex_FN]->approximate should have been called !
	const VEC3& N = this->m_approx[m_approxindex_FN]->getApprox(v1,m_attrindex_FN) ; // get new N
	assert(N == N2 || !"HalfEdgeSelector_Radiance<PFP>::computeSquaredLightfieldDifferenceAnalytical: only works with half-collapse") ;

	// compute new frame
	VEC3 T ;
	if (N2 != N1)
		T = N2 ^ N1 ; // i is perpendicular to newNormal
	else
		T = N1 ^ VEC3(1,2,3) ; // second random vector
	T.normalize() ;

	// Compute D1' and D2'
	VEC3 B1prime = N1 ^ T ;
	B1prime.normalize() ;
	VEC3 B2prime = N2 ^ T ;
	B2prime.normalize() ;

	// Rotation dans sens trigo dans le plan tangent autour de N (T1 --> T)
	const REAL gamma1 = ((B1 * T) > 0 ? 1 : -1) * acos( std::max(std::min(REAL(1), T1 * T ), -REAL(1))) ; // angle positif ssi
	const REAL gamma2 = ((B2 * T) > 0 ? 1 : -1) * acos( std::max(std::min(REAL(1), T2 * T ), -REAL(1))) ; // -PI/2 < angle(i,j1) < PI/2  ssi i*j1 > 0
	// Rotation dans le sens trigo autour de l'axe T (N1 --> N)
	const REAL alpha1 = ((N * B1prime) > 0 ? -1 : 1) * acos( std::max(std::min(REAL(1), N * N1), -REAL(1)) ) ; // angle positif ssi
	const REAL alpha2 = ((N * B2prime) > 0 ? -1 : 1) * acos( std::max(std::min(REAL(1), N * N2), -REAL(1)) ) ; // PI/2 < angle(j1',n) < -PI/2 ssi j1'*n < 0

	double alpha = fabs(alpha1 + alpha2) ;

	// get coefs of v1 and v2
	std::vector<VEC3> coefs1, coefs2, coefs ;
	coefs1.resize(m_K) ; coefs2.resize(m_K) ;
	for (unsigned int i = 0 ; i < m_K ; ++i)
	{
		coefs1[i] = this->m_approx[m_approxindex_HF[i]]->getAttr(m_attrindex_HF[i])[v0] ;
		coefs2[i] = this->m_approx[m_approxindex_HF[i]]->getAttr(m_attrindex_HF[i])[v1] ;
	}

	Utils::QuadricHF<REAL> q(coefs2, gamma2, alpha2) ;
	bool opt = q.findOptimizedCoefs(coefs) ; // coefs of d1's lightfield rotated around new local axis
	q += Utils::QuadricHF<REAL>(coefs1, gamma1, alpha1) ;

	if (!opt)
	{
		std::cerr << "HalfEdgeSelector_Radiance<PFP>::Optimization failed (should never happen since no optim is done)" << std::endl ;
		std::cout << alpha1 << std::endl ;
	}

	VEC3 avgColDiff = m_avgColor[v0] - m_avgColor[v1] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		avgColDiff[i] = pow(avgColDiff[i],2) ;

	const VEC3 LFerr = q.evalR3(coefs) ;
	VEC3 err = (alpha / M_PI) * avgColDiff + LFerr ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		err[i] = std::max(REAL(0),err[i]) ;

	return err ;
}

template <typename PFP>
VEC3 HalfEdgeSelector_Radiance<PFP>::computeGradient(const VEC3& P0, const VEC3& Pi, const VEC3& Pj, const Dart& v0, const Dart& v1, const Dart& vi, const Dart& vj, unsigned int channel) const
{
	const VEC3 ei = P0 - Pj ;
	const VEC3 ej = Pi - P0 ;

	// domaine sur lequel intégrer : hémisphère défini par n
	const VEC3& ni = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[vi] ;
	const VEC3& nj = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[vj] ;
	const VEC3& n0 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[v0] ;
	VEC3 normal = (ni+nj+n0)/3. ;	normal.normalize() ;

	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_n[i] = normal[i] ;
	std::vector<Dart> vertices ;	vertices.resize(3) ;
	vertices[0] = vi ;
	vertices[1] = vj ;
	vertices[2] = v1 ;

	std::vector<double> coefs ;	coefs.resize(m_K) ;
	VEC3 res ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
	{
		const Dart& d = vertices[i] ;
		const VEC3& t = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[d] ;
		const VEC3& b = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[d] ;
		const VEC3& n_d = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[d] ;

		double avgCol = m_avgColor[d][channel] ;
		for (unsigned int k = 0 ; k < m_K ; ++k)
		{
			coefs[k] = this->m_approx[m_approxindex_HF[k]]->getAttr(m_attrindex_HF[k])[d][channel] ;
		}
		res[i] = computeIntegral(&avgCol, t, b, n_d, m_K, coefs) ;
	}

	const REAL& ci = res[0] ;
	const REAL& cj = res[1] ;
	const REAL& c1 = res[2] ;

	return (ei.norm2()*fabs(ci-c1) + (ei*ej)*fabs(cj-c1))*ej - (ej.norm2()*fabs(cj-c1) + (ei*ej)*fabs(ci-c1))*ei ;
}

template <typename PFP>
VEC3 HalfEdgeSelector_Radiance<PFP>::integrateDscalGrad(
	const VEC3& d, const unsigned int& K, const VEC3& N, const VEC3& ei, const VEC3& ej,
	const VEC3* coefs1, const VEC3& T1, const VEC3& B1, const VEC3& N1, const VEC3& avg1,
	const VEC3* coefsi, const VEC3& Ti, const VEC3& Bi, const VEC3& Ni, const VEC3& avgi,
	const VEC3* coefsj, const VEC3& Tj, const VEC3& Bj, const VEC3& Nj, const VEC3& avgj
) const
{
	// set domain of integration
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_n[i] = N[i] ;

	// prepare workspace
	for (unsigned int i = 0 ; i < 3 ; ++i)	// d
		m_workspace[i] = d[i] ;
	// nbCoefs
	m_workspace[3] = K ;
	for (unsigned int i = 0 ; i < 3 ; ++i)	// ei
		m_workspace[4+i] = ei[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)	// ej
		m_workspace[7+i] = ej[i] ;
	// avg1, avgi, avgj
	m_workspace[11] = avg1[0] ;
	m_workspace[12] = avgi[0] ;
	m_workspace[13] = avgj[0] ;
	m_workspace[14] = avg1[1] ;
	m_workspace[15] = avgi[1] ;
	m_workspace[16] = avgj[1] ;
	m_workspace[17] = avg1[2] ;
	m_workspace[18] = avgi[2] ;
	m_workspace[19] = avgj[2] ;

	// N1, Ni, Nj
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[20+i] = N1[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[23+i] = Ni[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[26+i] = Nj[i] ;
	// T1, Ti, Tj
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[29+i] = T1[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[32+i] = Ti[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[35+i] = Tj[i] ;
	// B1, Bi, Bj
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[38+i] = B1[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[41+i] = Bi[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[44+i] = Bj[i] ;

	// coefs1, coefsi, coefsj
	for (unsigned int c = 0 ; c < 3 ; ++c)
		for (unsigned int k = 0 ; k < K ; ++k)
		{
			m_workspace[47+c*K+k] = coefs1[k][c] ;
			m_workspace[47+(3+c)*K+k] = coefsi[k][c] ;
			m_workspace[47+(6+c)*K+k] = coefsj[k][c] ;
		}

	VEC3 res ;
	for (unsigned int c = 0 ; c < 3 ; ++c)
	{
		m_workspace[10] = c ;

		// exec integrator
		double integral, area ;
		m_integrator.Compute(&integral, &area, &dispScalGrad, m_workspace, &isInDomain, m_n) ;
		res[c] = integral/(4*M_PI) ;
	}

	return res ;
}

template <typename PFP>
VEC3 HalfEdgeSelector_Radiance<PFP>::integrateDlf(
	const VEC3& d, const unsigned int& K, const VEC3& N, const VEC3& ei, const VEC3& ej,
	const std::vector<VEC3*> coefs0, const VEC3& T0, const VEC3& B0, const VEC3& N0, const VEC3& avg0,
	const std::vector<VEC3*> coefs1, const VEC3& T1, const VEC3& B1, const VEC3& N1, const VEC3& avg1,
	const std::vector<VEC3*> coefsi, const VEC3& Ti, const VEC3& Bi, const VEC3& Ni, const VEC3& avgi,
	const std::vector<VEC3*> coefsj, const VEC3& Tj, const VEC3& Bj, const VEC3& Nj, const VEC3& avgj
) const
{
	// set domain of integration
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_n[i] = N[i] ;

	// prepare workspace
	for (unsigned int i = 0 ; i < 3 ; ++i)	// d
		m_workspace[i] = d[i] ;
	// nbCoefs
	m_workspace[3] = K ;
	for (unsigned int i = 0 ; i < 3 ; ++i)	// ei
		m_workspace[4+i] = ei[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)	// ej
		m_workspace[7+i] = ej[i] ;
	// avg1, avgi, avgj
	m_workspace[12] = avg0[0] ;
	m_workspace[13] = avg1[0] ;
	m_workspace[14] = avgi[0] ;
	m_workspace[15] = avgj[0] ;
	m_workspace[16] = avg0[1] ;
	m_workspace[17] = avg1[1] ;
	m_workspace[18] = avgi[1] ;
	m_workspace[19] = avgj[1] ;
	m_workspace[20] = avg0[2] ;
	m_workspace[21] = avg1[2] ;
	m_workspace[22] = avgi[2] ;
	m_workspace[23] = avgj[2] ;

	// N1, Ni, Nj
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[24+i] = N0[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[27+i] = N1[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[30+i] = Ni[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[33+i] = Nj[i] ;
	// T1, Ti, Tj
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[36+i] = T0[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[39+i] = T1[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[42+i] = Ti[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[45+i] = Tj[i] ;
	// B1, Bi, Bj
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[48+i] = B0[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[51+i] = B1[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[54+i] = Bi[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[57+i] = Bj[i] ;

	// coefs1, coefsi, coefsj
	for (unsigned int c = 0 ; c < 3 ; ++c)
		for (unsigned int k = 0 ; k < K ; ++k)
		{
			m_workspace[60+c*K+k] = coefs0[k]->operator[](c) ;
			m_workspace[60+(3+c)*K+k] = coefs1[k]->operator[](c) ;
			m_workspace[60+(6+c)*K+k] = coefsi[k]->operator[](c) ;
			m_workspace[60+(9+c)*K+k] = coefsj[k]->operator[](c) ;
		}

	m_workspace[11] = m_SH ;

	VEC3 res ;
	for (unsigned int c = 0 ; c < 3 ; ++c)
	{
		m_workspace[10] = c ;

		// exec integrator
		double integral, area ;
		m_integrator.Compute(&integral, &area, &dlf, m_workspace, &isInDomain, m_n) ;
		res[c] = integral/(4*M_PI) ;
	}

	return res ;
}

template <typename PFP>
REAL HalfEdgeSelector_Radiance<PFP>::computeIntegral(const double* avgi, const VEC3& ti, const VEC3& bi, const VEC3& ni, unsigned int nbCoefs, const std::vector<double>& coefs) const
{
	// n is filled

	// fill workspace
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[i] = double(ni[i]) ; // n
	m_workspace[3] = *avgi ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[4+i] = double(ti[i]) ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[7+i] = double(bi[i]) ;
	m_workspace[10] = double(nbCoefs) ;
	for (unsigned int i = 0 ; i < nbCoefs ; ++i)
		m_workspace[11+i] = coefs[i] ;

	// call integral function
	double integral ;
	double area ;
	m_integrator.Compute(&integral, &area, &CartesianFunction, m_workspace, &isInDomain, m_n) ;

	return integral*4*M_PI ;
}

// fonction qui retourne bool est_dans_domaine en fonction de f et omega
template <typename PFP>
bool HalfEdgeSelector_Radiance<PFP>::isInDomain(double x, double y, double z, void *data)
{
	// (x,y,z) vecteur de vue
	// n vecteur normal
	double* n = (double*)data ;

	// vrai ssi n*(x,y,z) > 0
	return n[0]*x+n[1]*y+n[2]*z >= 0 ;
}

// fonction qui retourne col en fonction de f et de omega
template <typename PFP>
double HalfEdgeSelector_Radiance<PFP>::CartesianFunction (double x, double y, double z, void* data)
{
	// (x,y,z) vecteur de vue
	// data = {n,avgCol,t,b,nbCoefs,coefs}
	double* myData = (double*)data ;

	// normale
	if (myData[0]*x + myData[1]*y + myData[2]*z < 0)
	{
		return myData[3] ;
	}

	// tangente
	double u = myData[4]*x + myData[5]*y + myData[6]*z ;

	// binormale
	double v = myData[7]*x + myData[8]*y + myData[9]*z ;

	// nbcoefs
	unsigned int nb = (unsigned int)(myData[10]) ;

	// eval en ((xyz)*t,(xyz)*b)
	double res = 0 ;
	if (nb > 0)
	{
		// coefs
		double* coefs = &(myData[11]) ;
		res = coefs[0] ;
		if (nb > 1)
		{
			res += coefs[1]*v ;
			res += coefs[2]*u ;
			if (nb > 3)
			{
				res += coefs[3]*u*v ;
				res += coefs[4]*v*v ;
				res += coefs[5]*u*u ;
				if (nb > 6)
				{
					res += coefs[6]*u*v*v ;
					res += coefs[7]*v*u*u ;
					res += coefs[8]*v*v*v ;
					res += coefs[9]*u*u*u ;
					if (nb > 10)
					{
						res += coefs[10]*v*v*u*u ;
						res += coefs[11]*v*v*v*u ;
						res += coefs[12]*v*u*u*u ;
						res += coefs[13]*v*v*v*v ;
						res += coefs[14]*u*u*u*u ;
					}
				}
			}
		}
	}

	return res ;
}

template <typename PFP>
double HalfEdgeSelector_Radiance<PFP>::dispScalGrad (double x, double y, double z, void* data)
{
	double* myData = (double*)data ;

	// decode workspace
	double* d = &(myData[0]) ;
	unsigned int K = myData[3] ;
	double* ei = &(myData[4]) ;
	double* ej = &(myData[7]) ;
	unsigned int channel = myData[10] ;
	bool isSH = myData[11] ;
	double* avg = &(myData[12+3*channel]) ;
	// (11=avg1_c0,avgI_c0,avgJ_c0,avg1_c1,avgI_c2,avgJ_c3,avg1_c1,avgI_c2,avgJ_c3)
	double* N1 = &(myData[21]) ;
	double* Ni = &(myData[24]) ;
	double* Nj = &(myData[27]) ;
	double* T1 = &(myData[30]) ;
	double* Ti = &(myData[33]) ;
	double* Tj = &(myData[36]) ;
	double* B1 = &(myData[39]) ;
	double* Bi = &(myData[42]) ;
	double* Bj = &(myData[45]) ;
	double* coefs1 = &(myData[48+K*channel]) ; // (coefs1_c0, ... coefsK_c0,coefs1_c1, ... coefsK_c1, coefs1_c2, coefsK_c2)
	double* coefsi = &(myData[48+3*K+K*channel]) ;
	double* coefsj = &(myData[48+6*K+K*channel]) ;

	double vectorProd[3] ;
	vectorProd[0] = ei[1]*ej[2] - ei[2]*ej[1] ;
	vectorProd[1] = ei[2]*ej[0] - ei[0]*ej[2] ;
	vectorProd[2] = ei[0]*ej[1] - ei[1]*ej[0] ;
	double areaSq = vectorProd[0]*vectorProd[0] + vectorProd[1]*vectorProd[1] + vectorProd[2]*vectorProd[2] ;

	if (areaSq < 1e-9)
		return 0 ;

	double einorm2 = ei[0]*ei[0] + ei[1]*ei[1] + ei[2]*ei[2] ;
	double ejnorm2 = ej[0]*ej[0] + ej[1]*ej[1] + ej[2]*ej[2] ;
	double eiej = ei[0]*ej[0] + ei[1]*ej[1] + ei[2] * ej[2] ;

	double f1 = evalF(N1,&(avg[0]),K,isSH,T1,B1,coefs1,x,y,z) ;
	double fi = evalF(Ni,&(avg[1]),K,isSH,Ti,Bi,coefsi,x,y,z) ;
	double fj = evalF(Nj,&(avg[2]),K,isSH,Tj,Bj,coefsj,x,y,z) ;

	double t1 = einorm2 * (fi-f1) + eiej*(fj-f1) ;
	double t2 = ejnorm2 * (fj-f1) + eiej*(fi-f1) ;

	double grad[3] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		grad[i] = (t1*ej[i] - t2*ei[i]) / areaSq ;

	return pow(d[0]*grad[0] + d[1]*grad[1] + d[2]*grad[2],2) ;
}

template <typename PFP>
double HalfEdgeSelector_Radiance<PFP>::dlf(double x, double y, double z, void* data)
{
	double* myData = (double*)data ;

	// decode workspace
	double* d = &(myData[0]) ;
	unsigned int K = myData[3] ;
	double* ei = &(myData[4]) ;
	double* ej = &(myData[7]) ;
	unsigned int channel = myData[10] ;
	bool isSH = myData[11] ;
	double* avg = &(myData[12+4*channel]) ;
	// (11=avg0_c0,avg1_c0,avgI_c0,avgJ_c0,avg0_c1,avg1_c1,avgI_c1,avgJ_c1,avg0_c2,avg1_c2,avgI_c2,avgJ_c2)
	double* N0 = &(myData[24]) ;
	double* N1 = &(myData[27]) ;
	double* Ni = &(myData[30]) ;
	double* Nj = &(myData[33]) ;
	double* T0 = &(myData[36]) ;
	double* T1 = &(myData[39]) ;
	double* Ti = &(myData[42]) ;
	double* Tj = &(myData[45]) ;
	double* B0 = &(myData[48]) ;
	double* B1 = &(myData[51]) ;
	double* Bi = &(myData[54]) ;
	double* Bj = &(myData[57]) ;
	double* coefs0 = &(myData[60+K*channel]) ; // (coefs1_c0, ... coefsK_c0,coefs1_c1, ... coefsK_c1, coefs1_c2, coefsK_c2)
	double* coefs1 = &(myData[60+3*K+K*channel]) ;
	double* coefsi = &(myData[60+6*K+K*channel]) ;
	double* coefsj = &(myData[60+9*K+K*channel]) ;

	double vectorProd[3] ;
	vectorProd[0] = ei[1]*ej[2] - ei[2]*ej[1] ;
	vectorProd[1] = ei[2]*ej[0] - ei[0]*ej[2] ;
	vectorProd[2] = ei[0]*ej[1] - ei[1]*ej[0] ;
	double areaSq = vectorProd[0]*vectorProd[0] + vectorProd[1]*vectorProd[1] + vectorProd[2]*vectorProd[2] ;

	if (areaSq < 1e-9)
		return 0 ;

	double einorm2 = ei[0]*ei[0] + ei[1]*ei[1] + ei[2]*ei[2] ;
	double ejnorm2 = ej[0]*ej[0] + ej[1]*ej[1] + ej[2]*ej[2] ;
	double eiej = ei[0]*ej[0] + ei[1]*ej[1] + ei[2] * ej[2] ;

//	std::cout << isSH << std::endl ;

	double f0 = evalF(N0, &(avg[0]),K,isSH,T0,B0,coefs0,x,y,z) ;
	double f1 = evalF(N1, &(avg[1]),K,isSH,T1,B1,coefs1,x,y,z) ;
	double fi = evalF(Ni, &(avg[2]),K,isSH,Ti,Bi,coefsi,x,y,z) ;
	double fj = evalF(Nj, &(avg[3]),K,isSH,Tj,Bj,coefsj,x,y,z) ;

	double t1 = einorm2 * (fi-f1) + eiej*(fj-f1) ;
	double t2 = ejnorm2 * (fj-f1) + eiej*(fi-f1) ;

	double grad[3] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		grad[i] = (t1*ej[i] - t2*ei[i]) / areaSq ;

	// for second metric
	return pow(f0 - f1 + d[0]*grad[0] + d[1]*grad[1] + d[2]*grad[2], 2) ;
}

template <typename PFP>
double HalfEdgeSelector_Radiance<PFP>::evalF(double* N, double* avg, unsigned int nb, bool isSH, double* T, double* B, double* coefs, double& x, double& y, double& z)
{
	if (N[0]*x+N[1]*y+N[2]*z < 0)
		return *avg ;

	// tangente
	double u = T[0]*x + T[1]*y + T[2]*z ;

	// binormale
	double v = B[0]*x + B[1]*y + B[2]*z ;

	// eval en ((xyz)*t,(xyz)*b)
	double res = 0 ;
	if (!isSH)
	{
		if (nb > 0)
		{
			// coefs
			res = coefs[0] ;
			if (nb > 1)
			{
				res += coefs[1]*v ;
				res += coefs[2]*u ;
				if (nb > 3)
				{
					res += coefs[3]*u*v ;
					res += coefs[4]*v*v ;
					res += coefs[5]*u*u ;
					if (nb > 6)
					{
						res += coefs[6]*u*v*v ;
						res += coefs[7]*v*u*u ;
						res += coefs[8]*v*v*v ;
						res += coefs[9]*u*u*u ;
						if (nb > 10)
						{
							res += coefs[10]*v*v*u*u ;
							res += coefs[11]*v*v*v*u ;
							res += coefs[12]*v*u*u*u ;
							res += coefs[13]*v*v*v*v ;
							res += coefs[14]*u*u*u*u ;
						}
					}
				}
			}
		}
	}
	else
	{
		double theta, phi ;
		cart2spherical(x, y, theta, phi) ;
		std::vector<double> basis = SphericalHarmonic<double>::evalBaseAt(theta, phi, nb) ;
		for (unsigned int i = 0 ; i < nb ; ++i)
		{
			res += coefs[i] * basis[i] ;
		}
	}

	return res ;
}

template <typename PFP>
void HalfEdgeSelector_Radiance<PFP>::cart2spherical(double u, double v, double& theta, double& phi)
{
	double z = 1.0 - (u*u + v*v) ;
	if (-1e-5 < z && z < 0)
		z = 0 ;
	z = sqrt(z) ;
	theta = M_PI + atan2(v, u);
	phi = acos(z) ;
}

// fonction qui retourne col en fonction de f et de omega
template <typename PFP>
double HalfEdgeSelector_Radiance<PFP>::SquaredDifferenceOfCartesianFunctions (double x, double y, double z, void* data)
{
	// (x,y,z) vecteur de vue
	// data = {n1,avgCol1,t1,b1,nbCoefs1,coefs1, n2,avgCol2,t2,b2,nbCoefs2,coefs2}
	double* myData1 = (double*)data ;
	unsigned int nb = (unsigned int)(myData1[10]) ;
	double* myData2 = &(myData1[11+nb]) ;

	return pow(CartesianFunction(x, y, z, myData1) - CartesianFunction(x, y, z, myData2), 2) ;
}
*/
} // namespace SCHNApps

} // namespace CGoGN
