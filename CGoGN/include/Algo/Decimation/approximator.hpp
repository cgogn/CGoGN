/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2013, IGG Team, ICube, University of Strasbourg           *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

namespace CGoGN
{

namespace Algo
{

namespace Surface
{

namespace Decimation
{

template <typename PFP, typename T, unsigned int ORBIT>
Approximator<PFP,T,ORBIT>::Approximator(MAP& m, VertexAttribute<T, MAP>& attr, Predictor<PFP, T>* predictor) :
	ApproximatorGen<PFP>(m),
	m_attr(attr),
	m_predictor(predictor)
{
	if (!m_attr.isValid())
		std::cerr << "Approximator Warning: given attribute is not valid" << std::endl ;

	std::stringstream aname ;
	aname << "approx_" << m_attr.name() ;
	m_approx = this->m_map.template addAttribute<T, ORBIT, MAP>(aname.str()) ;

	if (m_predictor)	// if a predictor is associated to the approximator
	{					// create attribute to store the details needed for reconstruction
		std::stringstream dname ;
		dname << "detail_" << m_attr.name() ;
		m_detail = this->m_map.template addAttribute<T, ORBIT, MAP>(dname.str()) ;
	}
}

template <typename PFP, typename T, unsigned int ORBIT>
Approximator<PFP,T,ORBIT>::~Approximator()
{
	this->m_map.removeAttribute(m_approx) ;
	if (m_predictor)
		this->m_map.removeAttribute(m_detail) ;
}

template <typename PFP, typename T, unsigned int ORBIT>
const VertexAttribute<T, typename PFP::MAP>& Approximator<PFP,T,ORBIT>::getApproximatedAttribute() const
{
	return m_attr ;
}

template <typename PFP, typename T, unsigned int ORBIT>
VertexAttribute<T, typename PFP::MAP>& Approximator<PFP,T,ORBIT>::getApproximatedAttribute()
{
	return m_attr ;
}

template <typename PFP, typename T, unsigned int ORBIT>
const std::string& Approximator<PFP,T,ORBIT>::getApproximatedAttributeName() const
{
	return m_attr.name() ;
}

template <typename PFP, typename T, unsigned int ORBIT>
const AttributeHandler<T, ORBIT, typename PFP::MAP>& Approximator<PFP,T,ORBIT>::getApproximationResultAttribute() const
{
	return m_approx ;
}

template <typename PFP, typename T, unsigned int ORBIT>
void Approximator<PFP,T,ORBIT>::saveApprox(Dart d)
{
	m_app = m_approx[d] ;
}

template <typename PFP, typename T, unsigned int ORBIT>
void Approximator<PFP,T,ORBIT>::affectApprox(Dart d)
{
	m_attr[d] = m_app ;
}

template <typename PFP, typename T, unsigned int ORBIT>
const T& Approximator<PFP,T,ORBIT>::getApprox(Dart d) const
{
	return m_approx[d] ;
}

template <typename PFP, typename T, unsigned int ORBIT>
const Predictor<PFP, T>* Approximator<PFP,T,ORBIT>::getPredictor() const
{
	return m_predictor ;
}

template <typename PFP, typename T, unsigned int ORBIT>
const T& Approximator<PFP,T,ORBIT>::getDetail(Dart d) const
{
	assert(m_predictor || !"Trying to get detail on a non-predictive scheme") ;
	return m_detail[d] ;
}

template <typename PFP, typename T, unsigned int ORBIT>
void Approximator<PFP,T,ORBIT>::setDetail(Dart d, const T& val)
{
	assert(m_predictor || !"Trying to set detail on a non-predictive scheme") ;
	m_detail[d] = val ;
}

// TODO works only for vector types !!
//REAL detailMagnitude(Dart d)
//{
//	assert(m_predictor || !"Trying to get detail magnitude on a non-predictive scheme") ;
//	return m_detail[d].norm2() ;
//}

// TODO works only for vector types !!
//template <typename PFP, typename T, unsigned int ORBIT>
//void Approximator<PFP,T,ORBIT>::addDetail(Dart d, double amount, bool sign, typename PFP::MATRIX33* detailTransform)
//{
//	assert(m_predictor || !"Trying to add detail on a non-predictive scheme") ;

//	for (unsigned int i = 0 ; i < m_attrV.size() ; ++i)
//	{
//		T det = m_detail[i][d] ;
//		if(detailTransform)
//			det = (*detailTransform) * det ;
//		det *= amount ;
//		if(!sign)
//			det *= REAL(-1) ;
//		m_attrV[i].operator[](d) += det ;
//	}
//}

} // namespace Decimation

} // namespace Surface

} // namespace Algo

} // namespace CGoGN
