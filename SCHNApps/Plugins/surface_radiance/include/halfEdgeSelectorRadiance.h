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
//	VertexAttribute<PFP2::VEC3, PFP2::MAP> m_avgColor;

	unsigned int m_nb_coefs;

	SphericalFunctionIntegratorCartesian m_integrator;

	std::multimap<float, Dart> halfEdges;
	typename std::multimap<float, Dart>::iterator cur;

	void initHalfEdgeInfo(Dart d);
	void updateHalfEdgeInfo(Dart d, bool recompute);
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo);
	void recomputeQuadric(const Dart d);

	typename PFP::REAL computeRadianceError(Dart d);

	static bool isInHemisphere(double x, double y, double z, void* u)
	{ // true iff [x,y,z] and u have the same direction
		REAL* n = (REAL*)(u);
		return x*n[0] + y*n[1] + z*n[2] >= 0.0;
	}

	static double SHEvalCartesian_Error(double x, double y, double z, void* u)
	{
		SH& e = *(SH*)(u);
		VEC3 c = e.evaluate_at(x, y, z, 0);
		return c.norm2();
	}

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
		m_nb_coefs(0)
	{
		halfEdgeInfo = m.template checkAttribute<HalfEdgeInfo, DART, PFP2::MAP>("halfEdgeInfo");
		m_quadric = m.template checkAttribute<Utils::Quadric<PFP2::REAL>, VERTEX, PFP2::MAP>("QEMquadric");
//		m_avgColor = m.template checkAttribute<PFP2::VEC3, VERTEX, PFP2::MAP>("avgColor");
	}

	~HalfEdgeSelector_Radiance()
	{
		this->m_map.removeAttribute(halfEdgeInfo);
		this->m_map.removeAttribute(m_quadric);
//		this->m_map.removeAttribute(m_avgColor);
		m_integrator.Release();
	}

	Algo::Surface::Decimation::SelectorType getType() { return Algo::Surface::Decimation::S_OTHER; }
	bool init();
	bool nextEdge(Dart& d) const;
	void updateBeforeCollapse(Dart d);
	void updateAfterCollapse(Dart d2, Dart dd2);

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
