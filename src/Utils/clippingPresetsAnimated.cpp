/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.u-strasbg.fr/                                         *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include "Utils/clippingPresetsAnimated.h"

namespace CGoGN
{

namespace Utils
{


/***********************************************
 *
 * 		Base Class
 *
 ***********************************************/


void ClippingPresetAnimated::apply(ClippingShader* clipShader, std::vector<unsigned int> *planesIds, std::vector<unsigned int> *spheresIds)
{
	if ( (clipShader == NULL) || (planesIds == NULL) || (spheresIds == NULL) )
		return;

	// Apply preset
	ClippingPreset::apply(clipShader, planesIds, spheresIds);

	// Store the clipping shader pointer
	m_attachedClippingShader = clipShader;

	// Store ids
	m_planesIds.resize(0);
	m_planesIds.insert(m_planesIds.begin(), planesIds->begin(), planesIds->end());
	m_spheresIds.resize(0);
	m_spheresIds.insert(m_spheresIds.begin(), spheresIds->begin(), spheresIds->end());
}


/***********************************************
 *
 * 		Derived Classes
 *
 ***********************************************/


ClippingPresetAnimatedDualPlanes::ClippingPresetAnimatedDualPlanes(
		Geom::Vec3f centerStart, Geom::Vec3f centerEnd, float size, int axis, bool facing, bool zigzag)
{
	// Store the animation settings
	m_dirVec = centerEnd - centerStart;
	m_dirVec.normalize();
	m_startPoint = centerStart;
	m_endPoint = centerEnd;
	m_zigzag = zigzag;

	// Correct axis if necessary
	if ((axis < 0) || (axis > 2))
		axis = 0;

	// Axis on which planes will be aligned
	Geom::Vec3f positDir (0.0f, 0.0f, 0.0f);
	positDir[axis] = 1.0f;
	Geom::Vec3f negDir (0.0f, 0.0f, 0.0f);
	negDir[axis] = -1.0f;

	// Facing of planes
	float side = 1.0f;
	if (facing)
		side = -1.0f;

	// Add planes to preset
	addClipPlane(positDir, centerStart + positDir*(size / 2.0f)*(side));
	addClipPlane(negDir, centerStart + negDir*(size / 2.0f)*(side));

	// Set clipping mode
	ClippingShader::clippingMode clipMode = ClippingShader::CLIPPING_MODE_AND;
	if (facing)
		clipMode = ClippingShader::CLIPPING_MODE_OR;
	setClippingMode(clipMode);
}

void ClippingPresetAnimatedDualPlanes::step(unsigned int amount)
{
	// Check if the animation has been stopped
	if (m_animationSpeedFactor == 0.0f)
		return;

	// Check the validity of planes or spheres ids
	if ( !m_attachedClippingShader->isClipPlaneIdValid(m_planesIds[0])
		|| !m_attachedClippingShader->isClipPlaneIdValid(m_planesIds[1]) )
	{
		CGoGNerr
		<< "ERROR -"
		<< "ClippingPresetAnimatedDualPlanes::step"
		<< " - Some planes or spheres ids are not valid anymore - Animation stopped"
		<< CGoGNendl;
		m_animationSpeedFactor = 0.0f;
		return;
	}

	// Store the old center
	Geom::Vec3f oldCenter = (1.0f - m_animParam)*m_startPoint + m_animParam*m_endPoint;

	// Update animation parameter value
	m_animParam += (float)amount * m_animationOneStepIncrement * m_animationSpeedFactor;
	if (!m_zigzag)
	{
		while (m_animParam < 0.0f)
			m_animParam += 1.0f;
		while (m_animParam > 1.0f)
			m_animParam -= 1.0f;
	}
	else
	{
		while ( (m_animParam < 0.0f) || (m_animParam > 1.0f) )
		{
			if (m_animParam < 0.0f)
			{
				m_animParam = -m_animParam;
				m_animationOneStepIncrement = -m_animationOneStepIncrement;
			}
			else if (m_animParam > 1.0f)
			{
				m_animParam = 1.0f - (m_animParam - 1.0f);
				m_animationOneStepIncrement = -m_animationOneStepIncrement;
			}
		}
	}

	// Calculate new center
	Geom::Vec3f newCenter = (1.0f - m_animParam)*m_startPoint + m_animParam*m_endPoint;

	// Update clipping planes
	Geom::Vec3f plane1CurrPos = m_attachedClippingShader->getClipPlaneParamsOrigin(m_planesIds[0]);
	Geom::Vec3f plane2CurrPos = m_attachedClippingShader->getClipPlaneParamsOrigin(m_planesIds[1]);
	m_attachedClippingShader->setClipPlaneParamsOrigin(m_planesIds[0], plane1CurrPos + newCenter - oldCenter);
	m_attachedClippingShader->setClipPlaneParamsOrigin(m_planesIds[1], plane2CurrPos + newCenter - oldCenter);
}


ClippingPresetAnimatedRotatingPlane::ClippingPresetAnimatedRotatingPlane(Geom::Vec3f center, int axis)
{
	// Correct axis if necessary and store it
	if ((axis < 0) || (axis > 2))
		axis = 0;
	m_axis = axis;

	// Axis on which planes will be aligned
	Geom::Vec3f normal (0.0f, 0.0f, 0.0f);
	normal[(axis + 1) % 3] = 1.0f;

	// Add plane to preset
	addClipPlane(normal, center);

	// Set clipping mode
	setClippingMode(ClippingShader::CLIPPING_MODE_AND);
}

void ClippingPresetAnimatedRotatingPlane::step(unsigned int amount)
{
	// Check if the animation has been stopped
	if (m_animationSpeedFactor == 0.0f)
		return;

	// Check the validity of planes or spheres ids
	if (!m_attachedClippingShader->isClipPlaneIdValid(m_planesIds[0]))
	{
		CGoGNerr
		<< "ERROR -"
		<< "ClippingPresetAnimatedDualPlanes::step"
		<< " - Some planes or spheres ids are not valid anymore - Animation stopped"
		<< CGoGNendl;
		m_animationSpeedFactor = 0.0f;
		return;
	}

	// Update animation parameter value
	m_animParam += (float)amount * m_animationOneStepIncrement * m_animationSpeedFactor;
	while (m_animParam < 0.0f)
		m_animParam += 1.0f;
	while (m_animParam > 1.0f)
		m_animParam -= 1.0f;

	// Calculate new normal
	Geom::Vec3f planeNormal = m_attachedClippingShader->getClipPlaneParamsNormal(m_planesIds[0]);
	float angle = m_animParam*2.0*M_PI;
	planeNormal[(m_axis + 1) % 3] = cos(angle);
	planeNormal[(m_axis + 2) % 3] = sin(angle);
	m_attachedClippingShader->setClipPlaneParamsNormal(m_planesIds[0], planeNormal);


}

} // namespace Utils

} // namespace CGoGN
