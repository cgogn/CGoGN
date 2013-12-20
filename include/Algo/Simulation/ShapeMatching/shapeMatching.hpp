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

namespace Simulation
{

namespace ShapeMatching
{


template <typename PFP>
ShapeMatching<PFP>::ShapeMatching(VertexAttribute<VEC3>& position, VertexAttribute<REAL>& mass):
    m_position(position),
    m_mass(mass)
{
    unsigned int nbE = m_position.nbElements();

    m_p.reserve(nbE);
    m_q.reserve(nbE);


    m_goal = this->m_map.template getAttribute<VEC3, VERTEX>("goal");

    if(!m_goal.isValid())
        m_goal = this->m_map.template addAttribute<VEC3, VERTEX>("goal");
}

template <typename PFP>
ShapeMatching<PFP>::~ShapeMatching()
{
    if(m_goal.isValid())
      m_map.template removeAttribute<VEC3, VERTEX>(m_goal);
}


VEC3 ShapeMatching<PFP>::massCenter()
{
    VEC3 xcm(0);
    REAL m = 0.0;

    for(unsigned int i = m_position.begin() ; i < m_position.end() ; i = m_position.next())
    {
        xcm += m_mass[i] * m_p[i];
        m += m_mass[i];
    }

    xcm /= m;

    return xcm;
}

/**
 * Initialize pre-computed ....
 * First, \f$ x^{0}_{cm} \f$
 * In a second step, \f$ q_{i} = x^{0}_{i} - x^{0}_{cm} \f$
 */
template <typename PFP>
void ShapeMatching<PFP>::initialize()
{
    VEC3 x0cm = massCenter();

    for(unsigned int i = m_position.begin() ; i < m_position.end() ; i = m_position.next())
    {
        m_q[i] = VEC3(m_position[i] - x0cm); //q_{i} = x^{0}_{i} - x^{0}_{cm}
    }
}

template <typename PFP>
void ShapeMatching<PFP>::shapeMatch()
{
    //1.
    VEC3 xcm = massCenter();

    for(unsigned int i = m_position.begin() ; i < m_position.end() ; i = m_position.next())
    {
        m_p[i] = VEC3(m_position[i] - xcm); //p_{i} = x_{i} - x_{cm}
    }

    //2.
    Eigen::Matrix3f apq;
    apq = Eigen::Matrix3f::Zero();

    for(unsigned int i=0 ; i < p.size() ; ++i)
    {
        apq(0,0) += m_p[i][0] * m_q[i][0];
        apq(0,1) += m_p[i][0] * m_q[i][1];
        apq(0,2) += m_p[i][0] * m_q[i][2];

        apq(1,0) += m_p[i][1] * m_q[i][0];
        apq(1,1) += m_p[i][1] * m_q[i][1];
        apq(1,2) += m_p[i][1] * m_q[i][2];

        apq(2,0) += m_p[i][2] * m_q[i][0];
        apq(2,1) += m_p[i][2] * m_q[i][1];
        apq(2,2) += m_p[i][2] * m_q[i][2];
    }

    Eigen::Matrix3f S = apq.transpose() * apq ; //symmetric matrix

    //3. Jacobi Diagonlisation
    Eigen::JacobiSVD<Eigen::Matrix3f> svd(S, ComputeFullU);
    Eigen::Matrix3f SR = svd.matrixU(); // diagonalization with Jacobi rotations

    S(0,0) = 1.0f/sqrt(S(0,0));
    S(1,1) = 1.0f/sqrt(S(1,1));
    S(2,2) = 1.0f/sqrt(S(2,2));

    S = SR * S * SR.transpose();

    Eigen::Matrix3f R = apq * S; //S^{-1}

    //4.
    for(unsigned int i = m_goal.begin() ; i < m_goal.end() ; i = m_goal.next())
    {
        m_goal[i] = R * m_q[i] + xcm; // g_{i} = R * q_i + x_{cm}
    }
}


} // namespace ShapeMatching

} // namespace Simulation

} // namespace Surface

} // namespace Algo

} // namespace CGoGN
