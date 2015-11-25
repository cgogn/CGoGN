#ifndef _EDGESELECTOR_RADIANCE_H_
#define _EDGESELECTOR_RADIANCE_H_

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
class EdgeSelector_Radiance : public Algo::Surface::Decimation::Selector<PFP>
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
	Algo::Surface::Decimation::Approximator<PFP, VEC3, EDGE>& m_positionApproximator;
	Algo::Surface::Decimation::Approximator<PFP, VEC3, EDGE>& m_normalApproximator;
	Algo::Surface::Decimation::Approximator<PFP, SH, EDGE>& m_radianceApproximator;

	typedef	struct
	{
		typename std::multimap<float, Dart>::iterator it;
		bool valid;
		static std::string CGoGNnameOfType() { return "LightfieldGradEdgeInfo"; }
	} LightfieldEdgeInfo;
	typedef NoTypeNameAttribute<LightfieldEdgeInfo> EdgeInfo;

	EdgeAttribute<EdgeInfo, PFP2::MAP> edgeInfo;
	VertexAttribute<Utils::Quadric<PFP2::REAL>, PFP2::MAP> m_quadric;
//	VertexAttribute<PFP2::VEC3, PFP2::MAP> m_avgColor;

	unsigned int m_nb_coefs;

	SphericalFunctionIntegratorCartesian m_integrator;

	std::multimap<float, Dart> edges;
	typename std::multimap<float, Dart>::iterator cur;

	void initEdgeInfo(Dart d);
	void updateEdgeInfo(Dart d);
	void computeEdgeInfo(Dart d, EdgeInfo& einfo);
	void recomputeQuadric(const Dart d);

	typename PFP::REAL computeRadianceError(Dart d, const VEC3& p, const VEC3& n, const SH& r);

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
	EdgeSelector_Radiance(
		MAP& m,
		VertexAttribute<VEC3, MAP>& pos,
		VertexAttribute<VEC3, MAP>& norm,
		VertexAttribute<SH, MAP>& rad,
		Algo::Surface::Decimation::Approximator<PFP, VEC3, EDGE>& posApprox,
		Algo::Surface::Decimation::Approximator<PFP, VEC3, EDGE>& normApprox,
		Algo::Surface::Decimation::Approximator<PFP, SH, EDGE>& radApprox
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
		edgeInfo = m.template checkAttribute<EdgeInfo, EDGE, PFP2::MAP>("EdgeInfo");
		m_quadric = m.template checkAttribute<Utils::Quadric<PFP2::REAL>, VERTEX, PFP2::MAP>(pos.name() + "_QEM");
//		m_avgColor = m.template checkAttribute<PFP2::VEC3, VERTEX, PFP2::MAP>("avgColor");
	}

	~EdgeSelector_Radiance()
	{
		this->m_map.removeAttribute(edgeInfo);
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
		assert(errors != NULL || !"EdgeSelector_Radiance::getEdgeErrors requires non null vertexattribute argument") ;
		if (!errors->isValid())
			std::cerr << "EdgeSelector_Radiance::getEdgeErrors requires valid edgeattribute argument" << std::endl ;
		assert(edgeInfo.isValid());

		TraversorE<PFP2::MAP> travE(this->m_map);
		for(Dart d = travE.begin() ; d != travE.end() ; d = travE.next())
		{
			Dart dd = this->m_map.phi2(d);
			if (edgeInfo[d].valid)
			{
				(*errors)[d] = edgeInfo[d].it->first;
			}
			if (edgeInfo[dd].valid && edgeInfo[dd].it->first < (*errors)[d])
			{
				(*errors)[d] = edgeInfo[dd].it->first;
			}
			if (!(edgeInfo[d].valid || edgeInfo[dd].valid))
				(*errors)[d] = -1;
		}
	}
};

} // namespace SCHNApps

} // namespace CGoGN

#include "edgeSelectorRadiance.hpp"

#endif // _HALFEDGESELECTOR_RADIANCE_H_
