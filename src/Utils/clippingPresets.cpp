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

#include "Utils/clippingPresets.h"

namespace CGoGN
{

namespace Utils
{


/***********************************************
 *
 * 		Constructors
 *
 ***********************************************/


ClippingPreset* ClippingPreset::CreateEmptyPreset()
{
	ClippingPreset *preset = new ClippingPreset;

	return preset;
}

ClippingPreset* ClippingPreset::CreateDualPlanesPreset(Geom::Vec3f center, float distance, int axis, bool facing)
{
	ClippingPreset *preset = new ClippingPreset;

	// Axis on which planes will be aligned
	if ((axis < 0) || (axis > 2))
		axis = 0;
	Geom::Vec3f positDir (0.0, 0.0, 0.0);
	positDir[axis] = 1.0;
	Geom::Vec3f negDir (0.0, 0.0, 0.0);
	negDir[axis] = -1.0;

	// Add planes to preset
	preset->addClipPlane(positDir, center + positDir*(distance / 2.0));
	preset->addClipPlane(negDir, center + negDir*(distance / 2.0));

	// Set clipping mode
	preset->setClippingMode(ClippingShader::CLIPPING_MODE_AND);

	return preset;
}

ClippingPreset* ClippingPreset::CreateCubePreset(Geom::Vec3f center, float distance, bool facing)
{
	ClippingPreset *preset = new ClippingPreset;

	// Directions
	Geom::Vec3f xAxisPos = Geom::Vec3f (1.0, 0.0, 0.0);
	Geom::Vec3f xAxisNeg = Geom::Vec3f (-1.0, 0.0, 0.0);
	Geom::Vec3f yAxisPos = Geom::Vec3f (0.0, 1.0, 0.0);
	Geom::Vec3f yAxisNeg = Geom::Vec3f (0.0, -1.0, 0.0);
	Geom::Vec3f zAxisPos = Geom::Vec3f (0.0, 0.0, 1.0);
	Geom::Vec3f zAxisNeg = Geom::Vec3f (0.0, 0.0, -1.0);

	// Add planes to preset
	preset->addClipPlane(xAxisPos, center + xAxisPos*(distance / 2.0));
	preset->addClipPlane(xAxisNeg, center + xAxisNeg*(distance / 2.0));
	preset->addClipPlane(yAxisPos, center + yAxisPos*(distance / 2.0));
	preset->addClipPlane(yAxisNeg, center + yAxisNeg*(distance / 2.0));
	preset->addClipPlane(zAxisPos, center + zAxisPos*(distance / 2.0));
	preset->addClipPlane(zAxisNeg, center + zAxisNeg*(distance / 2.0));

	// Set clipping mode
	preset->setClippingMode(ClippingShader::CLIPPING_MODE_AND);

	return preset;
}

ClippingPreset::ClippingPreset():
		m_clipMode (ClippingShader::CLIPPING_MODE_AND)
{

}


/***********************************************
 *
 * 		Preset settings
 *
 ***********************************************/


void ClippingPreset::addClipPlane(Geom::Vec3f normal, Geom::Vec3f origin)
{
	clipPlane newPlane;
	newPlane.normal = normal;
	newPlane.origin = origin;

	m_clipPlanes.push_back(newPlane);
}

void ClippingPreset::addClipSphere(Geom::Vec3f center, float radius)
{
	clipSphere newSphere;
	newSphere.center = center;
	newSphere.radius = radius;

	m_clipSpheres.push_back(newSphere);
}

void ClippingPreset::setClippingMode(ClippingShader::clippingMode clipMode)
{
	m_clipMode = clipMode;
}


/***********************************************
 *
 * 		Preset application
 *
 ***********************************************/


void ClippingPreset::apply(ClippingShader* clipShader, std::vector<unsigned int> *planesIds, std::vector<unsigned int> *spheresIds)
{
	if ( (clipShader == NULL) || (planesIds == NULL) || (spheresIds == NULL) )
		return;

	// Delete any remaining clip planes or spheres
	clipShader->deleteAllClipPlanes();
	clipShader->deleteAllClipSpheres();

	// Initialize ids arrays (they should already be of zero size)
	planesIds->resize(0);
	spheresIds->resize(0);

	// Add planes and spheres and get their ids
	for (size_t i = 0; i < m_clipPlanes.size(); i++)
	{
		unsigned int id = clipShader->addClipPlane();
		planesIds->push_back(id);
		clipShader->setClipPlaneParamsAll(id, m_clipPlanes[i].normal, m_clipPlanes[i].origin);
	}
	for (size_t i = 0; i < m_clipSpheres.size(); i++)
	{
		unsigned int id = clipShader->addClipSphere();
		spheresIds->push_back(id);
		clipShader->setClipSphereParamsAll(id, m_clipSpheres[i].center, m_clipSpheres[i].radius);
	}

	// Set new clipping mode
	clipShader->setClipMode(m_clipMode);

}

} // namespace Utils

} // namespace CGoGN
