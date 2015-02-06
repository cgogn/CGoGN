#ifndef _HALFEDGESELECTOR_RADIANCE_H_
#define _HALFEDGESELECTOR_RADIANCE_H_

#include "Algo/Decimation/selector.h"
#include "Algo/Decimation/approximator.h"
#include "Utils/qem.h"
#include "Utils/sphericalHarmonics.h"

#include "SphericalFunctionIntegratorCartesian.h"

namespace CGoGN
{

namespace SCHNApps
{

template <typename PFP>
class HalfEdgeSelector_Radiance : public Algo::Surface::Decimation::Selector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename Utils::SphericalHarmonics<REAL, VEC3> SH;

private:
	VertexAttribute<VEC3, MAP>& m_position;
	VertexAttribute<VEC3, MAP>& m_normal;
	VertexAttribute<SH, MAP>& m_radiance;
	Algo::Surface::Decimation::Approximator<PFP, VEC3, DART>& m_positionApproximator;
	Algo::Surface::Decimation::Approximator<PFP, VEC3, DART>& m_normalApproximator;
	Algo::Surface::Decimation::Approximator<PFP, SH, DART>& m_radianceApproximator;

	typedef	struct
	{
		typename std::multimap<float, Dart>::iterator it;
		bool valid;
		static std::string CGoGNnameOfType() { return "LightfieldGradHalfEdgeInfo"; }
	} LightfieldHalfEdgeInfo;
	typedef NoTypeNameAttribute<LightfieldHalfEdgeInfo> HalfEdgeInfo;

	DartAttribute<HalfEdgeInfo, PFP2::MAP> halfEdgeInfo;
	VertexAttribute<Utils::Quadric<PFP2::REAL>, PFP2::MAP> m_quadric;
	VertexAttribute<PFP2::VEC3, PFP2::MAP> m_avgColor;

	unsigned int m_nb_coefs;

	SphericalFunctionIntegratorCartesian m_integrator;
	double* m_n;
	double* m_workspace;

	std::multimap<float, Dart> halfEdges;
	typename std::multimap<float, Dart>::iterator cur;

	void initHalfEdgeInfo(Dart d);
	void updateHalfEdgeInfo(Dart d);
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo);
	void recomputeQuadric(const Dart d);
/*
	PFP2::VEC3 computeGradientLFerror(const Dart& v0, const Dart& v1) const;
	PFP2::VEC3 computeDirDerivativeLFerror(const Dart& v0, const Dart& v1);
	PFP2::VEC3 computeSquaredLightfieldDifferenceNumerical(const Dart& d1, const Dart& d2, const PFP2::VEC3& N) const;
	PFP2::VEC3 computeSquaredLightfieldDifferenceAnalytical(const Dart& d1, const Dart& d2) const;
	PFP2::VEC3 computeGradient(const PFP2::VEC3& P0, const PFP2::VEC3& Pi, const PFP2::VEC3& Pj, const Dart& v0, const Dart& v1, const Dart& vi, const Dart& vj, unsigned int channel) const;
	PFP2::VEC3 integrateDscalGrad(const PFP2::VEC3& d, const unsigned int& K, const PFP2::VEC3& N, const PFP2::VEC3& ei, const PFP2::VEC3& ej,
		const PFP2::VEC3* coefs1, const PFP2::VEC3& T1, const PFP2::VEC3& B1, const PFP2::VEC3& N1, const PFP2::VEC3& avg1,
		const PFP2::VEC3* coefsi, const PFP2::VEC3& Ti, const PFP2::VEC3& Bi, const PFP2::VEC3& Ni, const PFP2::VEC3& avgi,
		const PFP2::VEC3* coefsj, const PFP2::VEC3& Tj, const PFP2::VEC3& Bj, const PFP2::VEC3& Nj, const PFP2::VEC3& avgj) const;
	PFP2::VEC3 integrateDlf(const PFP2::VEC3& d, const unsigned int& K, const PFP2::VEC3& N, const PFP2::VEC3& ei, const PFP2::VEC3& ej,
		const std::vector<PFP2::VEC3*> coefs0, const PFP2::VEC3& T0, const PFP2::VEC3& B0, const PFP2::VEC3& N0, const PFP2::VEC3& avg0,
		const std::vector<PFP2::VEC3*> coefs1, const PFP2::VEC3& T1, const PFP2::VEC3& B1, const PFP2::VEC3& N1, const PFP2::VEC3& avg1,
		const std::vector<PFP2::VEC3*> coefsi, const PFP2::VEC3& Ti, const PFP2::VEC3& Bi, const PFP2::VEC3& Ni, const PFP2::VEC3& avgi,
		const std::vector<PFP2::VEC3*> coefsj, const PFP2::VEC3& Tj, const PFP2::VEC3& Bj, const PFP2::VEC3& Nj, const PFP2::VEC3& avgj) const;
	PFP2::REAL computeIntegral(const double *avgi, const PFP2::VEC3& ti, const PFP2::VEC3& bi, const PFP2::VEC3& ni, unsigned int nbCoefs, const std::vector<double>& coefs) const;

	static double dispScalGrad (double x, double y, double z, void* data);
	static double dlf (double x, double y, double z, void* data);
	static double evalF(double* N, double* avg, unsigned int nb, bool isSH, double* T, double* B, double* coefs, double& x, double& y, double& z);
	static void cart2spherical(double u, double v, double& theta, double& phi);
	static double SquaredDifferenceOfCartesianFunctions (double x, double y, double z, void* data);
	static bool isInDomain(double x, double y, double z, void *data);
	static double CartesianFunction (double x, double y, double z, void* data);
*/
public:
	HalfEdgeSelector_Radiance(
		MAP& m,
		VertexAttribute<VEC3, MAP>& pos,
		VertexAttribute<VEC3, MAP>& norm,
		VertexAttribute<SH, MAP>& rad,
		Algo::Surface::Decimation::Approximator<PFP, VEC3, DART>& posApprox,
		Algo::Surface::Decimation::Approximator<PFP, VEC3, DART>& normApprox,
		Algo::Surface::Decimation::Approximator<PFP, SH, DART>& radApprox
	) :
		Algo::Surface::Decimation::Selector<PFP2>(m),
		m_position(pos),
		m_normal(norm),
		m_radiance(rad),
		m_positionApproximator(posApprox),
		m_normalApproximator(normApprox),
		m_radianceApproximator(radApprox),
		m_nb_coefs(0),
		m_n(NULL),
		m_workspace(NULL)
	{
		halfEdgeInfo = m.template checkAttribute<HalfEdgeInfo, DART, PFP2::MAP>("halfEdgeInfo");
		m_quadric = m.template checkAttribute<Utils::Quadric<PFP2::REAL>, VERTEX, PFP2::MAP>("QEMquadric");
		m_avgColor = m.template checkAttribute<PFP2::VEC3, VERTEX, PFP2::MAP>("avgColor");
		m_n = new double[3];
	}

	~HalfEdgeSelector_Radiance()
	{
		this->m_map.removeAttribute(halfEdgeInfo);
		this->m_map.removeAttribute(m_quadric);
		this->m_map.removeAttribute(m_avgColor);
		m_integrator.Release();
		delete[] m_n;
		delete[] m_workspace;
	}

	Algo::Surface::Decimation::SelectorType getType() { return Algo::Surface::Decimation::S_OTHER; }
	bool init();
	bool nextEdge(Dart& d) const;
	void updateBeforeCollapse(Dart d);
	void updateAfterCollapse(Dart d2, Dart);

	void updateWithoutCollapse() { }

	void getEdgeErrors(EdgeAttribute<PFP2::REAL, PFP2::MAP> *errors) const
	{
		assert(errors != NULL || !"HalfEdgeSelector_Radiance::getEdgeErrors requires non null vertexattribute argument") ;
		if (!errors->isValid())
			std::cerr << "HalfEdgeSelector_Radiance::getEdgeErrors requires valid edgeattribute argument" << std::endl ;
		assert(halfEdgeInfo.isValid());

		TraversorE<PFP2::MAP> travE(this->m_map);
		for(Dart d = travE.begin() ; d != travE.end() ; d = travE.next())
		{
			Dart dd = this->m_map.phi2(d);
			if (halfEdgeInfo[d].valid)
			{
				(*errors)[d] = halfEdgeInfo[d].it->first;
			}
			if (halfEdgeInfo[dd].valid && halfEdgeInfo[dd].it->first < (*errors)[d])
			{
				(*errors)[d] = halfEdgeInfo[dd].it->first;
			}
			if (!(halfEdgeInfo[d].valid || halfEdgeInfo[dd].valid))
				(*errors)[d] = -1;
		}
	}
};

} // namespace SCHNApps

} // namespace CGoGN

#include "halfEdgeSelectorRadiance.hpp"

#endif // _HALFEDGESELECTOR_RADIANCE_H_
