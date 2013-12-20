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

void ShapeMatching<PFP>::computeAqqMatrix()
{

    //A_{qq} = (Sum_i{ m_{i} q_{i} q_i^{T} })^{-1}

    m_aqq = Eigen::Matrix3f::Zero();

    for(unsigned int i = 0 ; i < m_q.size() ; ++i)
    {
        m_aqq(0,0) += m_q[i][0] * m_q[i][0];
        m_aqq(1,0) += m_q[i][0] * m_q[i][1];
        m_aqq(2,0) += m_q[i][0] * m_q[i][2];

        m_aqq(0,1) += m_q[i][1] * m_q[i][0];
        m_aqq(1,1) += m_q[i][1] * m_q[i][1];
        m_aqq(2,1) += m_q[i][1] * m_q[i][2];

        m_aqq(0,2) += m_q[i][2] * m_q[i][0];
        m_aqq(1,2) += m_q[i][2] * m_q[i][1];
        m_aqq(2,2) += m_q[i][2] * m_q[i][2];
    }

    m_aqq = m_aqq.inverse();
}


template <typename PFP>
void ShapeMatchingLinear<PFP>::initialize()
{
    this->initialize();

    computeAqqMatrix();
}

template <typename PFP>
void ShapeMatchingLinear<PFP>::shapeMatch()
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
    Eigen::Matrix3f A = apq * m_aqq; //

    REAL det = A.determinant();
    REAL detAInv = 1.0f/powf(det,1.0f/3.0f);

    if(std::isfinite(detAInv))
    {
        R(0,0) += (m_beta * A(0,0) * detAInv) + ((1.0f - m_beta) * R(0,0));
        R(0,1) += (m_beta * A(0,1) * detAInv) + ((1.0f - m_beta) * R(0,1));
        R(0,2) += (m_beta * A(0,2) * detAInv) + ((1.0f - m_beta) * R(0,2));

        R(1,0) += (m_beta * A(1,0) * detAInv) + ((1.0f - m_beta) * R(1,0));
        R(1,1) += (m_beta * A(1,1) * detAInv) + ((1.0f - m_beta) * R(1,1));
        R(1,2) += (m_beta * A(1,2) * detAInv) + ((1.0f - m_beta) * R(1,2));

        R(2,0) += (m_beta * A(2,0) * detAInv) + ((1.0f - m_beta) * R(2,0));
        R(2,1) += (m_beta * A(2,1) * detAInv) + ((1.0f - m_beta) * R(2,1));
        R(2,2) += (m_beta * A(2,2) * detAInv) + ((1.0f - m_beta) * R(2,2));
    }
    else
    {
        //no linear deformation (planar config ?), applying identity
        R.identity();
    }

    //5.
    for(unsigned int i = m_goal.begin() ; i < m_goal.end() ; i = m_goal.next())
    {
        m_goal[i] = R * m_q[i] + xcm; // g_{i} = R * q_i + x_{cm}
    }

}


} // namespace Simulation

} // namespace Surface

} // namespace Algo

} // namespace CGoGN
