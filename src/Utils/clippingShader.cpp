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

#include "Utils/clippingShader.h"

namespace CGoGN
{

namespace Utils
{


/***********************************************
 *
 * 		Constructor / Destructor
 *
 ***********************************************/


ClippingShader::ClippingShader()
{
	// Initialize uniforms ids
	m_unif_clipPlanesEquations = 0;
	m_unif_clipSpheresCentersAndRadiuses = 0;
	m_unif_clipColorAttenuationFactor = 0;

	// Initialize default display variables
	m_clipPlanesDisplayColor = Geom::Vec3f (1.0, 0.6, 0.0);
	m_clipPlanesDisplayXRes = 2;
	m_clipPlanesDisplayYRes = 2;
	m_clipPlanesDisplaySize = 10.0;
	m_clipSpheresDisplayColor = Geom::Vec3f (0.0, 0.4, 1.0);
	m_clipSpheresDisplayXRes = 15;
	m_clipSpheresDisplayYRes = 10;

	// Initialize default global clipping variables
	m_hasClippingCodeBeenInserted = false;
	m_clipColorAttenuationFactor = 1.0;
}

ClippingShader::~ClippingShader()
{
	// Destroy remaining clip planes drawers
	size_t i;
	for (i = 0; i < m_clipPlanesDrawers.size(); i++)
		delete m_clipPlanesDrawers[i];
}


/***********************************************
 *
 * 		Plane Clipping
 *
 ***********************************************/


void ClippingShader::setClipPlanesCount(int planesCount)
{
	// Check if the clipping code has been inserted into shader
	if (errorRaiseClippingCodeNotInserted(!m_hasClippingCodeBeenInserted, "ClippingShader::setClipPlanesCount"))
		return;

	// Check if the given clipping planes count is valid
	if (errorRaiseParameterIsNotPositive((planesCount < 0), "ClippingShader::setClipPlanesCount", "planesCount"))
		return;

	// Shader name string
	std::string shaderName = m_nameVS + "/" + m_nameFS + "/" + m_nameGS;

	// Use a shader mutator
	ShaderMutator SM(shaderName, getVertexShaderSrc(), getFragmentShaderSrc());

	// Modify the clip planes count constant in both shader
	SM.changeIntConstantValue(ShaderMutator::VERTEX_SHADER, "CLIP_PLANES_COUNT", planesCount);
	SM.changeIntConstantValue(ShaderMutator::FRAGMENT_SHADER, "CLIP_PLANES_COUNT", planesCount);

	// Reload both shaders
	reloadVertexShaderFromMemory(SM.getModifiedVertexShaderSrc().c_str());
	reloadFragmentShaderFromMemory(SM.getModifiedFragmentShaderSrc().c_str());


	// Previous planes count
	int previousPlanesCount = getClipPlanesCount();

	// Resize the planes arrays to the right size
	m_clipPlanes.resize((size_t)planesCount);
	m_clipPlanesEquations.resize(4*(size_t)planesCount, 0.0);

	// Resize the planes drawers array to the right size, and create/destroy objects
	if (planesCount > previousPlanesCount)
	{
		m_clipPlanesDrawers.resize((size_t)planesCount, NULL);
		for (int i = previousPlanesCount; i < planesCount; i++)
			m_clipPlanesDrawers[i] = new Drawer;
	}
	else
	{
		//for (int i = planesCount; i < previousPlanesCount; i++)
			//delete m_clipPlanesDrawers[i]; // TODO : Bug dans drawer, crash le prochain affichage
		m_clipPlanesDrawers.resize((size_t)planesCount);
	}

	// Set default parameters values for new planes
	if (planesCount > previousPlanesCount)
	{
		Geom::Vec3f defaultFirstVec (1.0, 0.0, 0.0);
		Geom::Vec3f defaultSecondVec (0.0, 1.0, 0.0);
		Geom::Vec3f defaultOrigin (0.0, 0.0, 0.0);

		for (int i = previousPlanesCount; i < planesCount; i++)
			setClipPlaneParamsAll(defaultFirstVec, defaultSecondVec, defaultOrigin, i);
	}

	// Recompile shaders (automatically calls updateClippingUniforms)
	recompile();
}

int ClippingShader::getClipPlanesCount()
{
	return (int)m_clipPlanes.size();
}

void ClippingShader::setClipPlaneParamsAll(Geom::Vec3f vec1, Geom::Vec3f vec2, Geom::Vec3f origin, int planeIndex)
{
	// Check if the given index is out of range
	if (errorRaiseParameterIsOutOfRange(((planeIndex < 0) || (planeIndex > (getClipPlanesCount() - 1))), "ClippingShader::setClipPlaneParamsAll", "planeIndex"))
			return;

	// Normalize
	Geom::Vec3f vec1Normalized = vec1;
	vec1Normalized.normalize();
	Geom::Vec3f vec2Normalized = vec2;
	vec2Normalized.normalize();

	if ((vec1Normalized != m_clipPlanes[planeIndex].firstVec)
			|| (vec2Normalized != m_clipPlanes[planeIndex].secondVec)
			|| (origin != m_clipPlanes[planeIndex].origin))
	{
		// Copy the given clipping plane parameters
		m_clipPlanes[planeIndex].firstVec = vec1Normalized;
		m_clipPlanes[planeIndex].secondVec = vec2Normalized;
		m_clipPlanes[planeIndex].origin = origin;

		// Update the plane arrays
		updateClipPlaneArray(planeIndex);

		// Send again the whole planes equations array to shader
		sendClipPlanesEquationsUniform();

		// Update plane VBO
		updateClipPlaneVBO(planeIndex);
	}
}

void ClippingShader::setClipPlaneParamsFirstVec(Geom::Vec3f vec1, int planeIndex)
{
	// Check if the given index is out of range
	if (errorRaiseParameterIsOutOfRange(((planeIndex < 0) || (planeIndex > (getClipPlanesCount() - 1))), "ClippingShader::setClipPlaneParamsFirstVec", "planeIndex"))
			return;

	// Normalize
	Geom::Vec3f vec1Normalized = vec1;
	vec1Normalized.normalize();

	if (vec1Normalized != m_clipPlanes[planeIndex].firstVec)
	{
		// Copy the given clipping plane parameter
		m_clipPlanes[planeIndex].firstVec = vec1Normalized;

		// Update the plane arrays
		updateClipPlaneArray(planeIndex);

		// Send again the whole planes equations array to shader
		sendClipPlanesEquationsUniform();

		// Update plane VBO
		updateClipPlaneVBO(planeIndex);
	}
}

void ClippingShader::setClipPlaneParamsSecondVec(Geom::Vec3f vec2, int planeIndex)
{
	// Check if the given index is out of range
	if (errorRaiseParameterIsOutOfRange(((planeIndex < 0) || (planeIndex > (getClipPlanesCount() - 1))), "ClippingShader::setClipPlaneParamsSecondVec", "planeIndex"))
			return;

	// Normalize
	Geom::Vec3f vec2Normalized = vec2;
	vec2Normalized.normalize();

	if (vec2Normalized != m_clipPlanes[planeIndex].secondVec)
	{
		// Copy the given clipping plane parameter
		m_clipPlanes[planeIndex].secondVec = vec2Normalized;

		// Update the plane arrays
		updateClipPlaneArray(planeIndex);

		// Send again the whole planes equations array to shader
		sendClipPlanesEquationsUniform();

		// Update plane VBO
		updateClipPlaneVBO(planeIndex);
	}
}

void ClippingShader::setClipPlaneParamsOrigin(Geom::Vec3f origin, int planeIndex)
{
	// Check if the given index is out of range
	if (errorRaiseParameterIsOutOfRange(((planeIndex < 0) || (planeIndex > (getClipPlanesCount() - 1))), "ClippingShader::setClipPlaneParamsOrigin", "planeIndex"))
			return;

	if (origin != m_clipPlanes[planeIndex].origin)
	{
		// Copy the given clipping plane parameter
		m_clipPlanes[planeIndex].origin = origin;

		// Update the plane arrays
		updateClipPlaneArray(planeIndex);

		// Send again the whole planes equations array to shader
		sendClipPlanesEquationsUniform();

		// Update plane VBO
		updateClipPlaneVBO(planeIndex);
	}
}

Geom::Vec3f ClippingShader::getClipPlaneParamsFirstVec(int planeIndex)
{
	// Check if the given index is out of range
	if (errorRaiseParameterIsOutOfRange(((planeIndex < 0) || (planeIndex > (getClipPlanesCount() - 1))), "ClippingShader::getClipPlaneParamsFirstVec", "planeIndex"))
			return Geom::Vec3f(0.0, 0.0, 0.0);

	// Return the parameter
	return m_clipPlanes[planeIndex].firstVec;
}

Geom::Vec3f ClippingShader::getClipPlaneParamsSecondVec(int planeIndex)
{
	// Check if the given index is out of range
	if (errorRaiseParameterIsOutOfRange(((planeIndex < 0) || (planeIndex > (getClipPlanesCount() - 1))), "ClippingShader::getClipPlaneParamsSecondVec", "planeIndex"))
			return Geom::Vec3f(0.0, 0.0, 0.0);

	// Return the parameter
	return m_clipPlanes[planeIndex].secondVec;
}

Geom::Vec3f ClippingShader::getClipPlaneParamsOrigin(int planeIndex)
{
	// Check if the given index is out of range
	if (errorRaiseParameterIsOutOfRange(((planeIndex < 0) || (planeIndex > (getClipPlanesCount() - 1))), "ClippingShader::getClipPlaneParamsOrigin", "planeIndex"))
			return Geom::Vec3f(0.0, 0.0, 0.0);

	// Return the parameter
	return m_clipPlanes[planeIndex].origin;
}

void ClippingShader::updateClipPlaneArray(int planeIndex)
{
	// Check if the given index is out of range
	if (errorRaiseParameterIsOutOfRange(((planeIndex < 0) || (planeIndex > (getClipPlanesCount() - 1))), "ClippingShader::updateClipPlaneArray", "planeIndex"))
			return;

	// Update the planes equations array
	Geom::Vec3f planeNormal = m_clipPlanes[planeIndex].firstVec ^ m_clipPlanes[planeIndex].secondVec;
	float d = -(planeNormal * m_clipPlanes[planeIndex].origin);
	m_clipPlanesEquations[4*planeIndex + 0] = planeNormal[0];
	m_clipPlanesEquations[4*planeIndex + 1] = planeNormal[1];
	m_clipPlanesEquations[4*planeIndex + 2] = planeNormal[2];
	m_clipPlanesEquations[4*planeIndex + 3] = d;

}


/***********************************************
 *
 * 		Plane Clipping Display
 *
 ***********************************************/


void ClippingShader::displayClipPlanes()
{
	for (size_t i = 0; i < m_clipPlanesDrawers.size(); i++)
		m_clipPlanesDrawers[i]->callList();
}

void ClippingShader::setClipPlanesDisplayColor(Geom::Vec3f color)
{
	if (color != m_clipPlanesDisplayColor)
	{
		m_clipPlanesDisplayColor = color;
		updateClipPlanesVBOs();
	}
}

Geom::Vec3f ClippingShader::getClipPlanesDisplayColor()
{
	return m_clipPlanesDisplayColor;
}

void ClippingShader::setClipPlanesDisplayXRes(size_t res)
{
	if (res != m_clipPlanesDisplayXRes)
	{
		m_clipPlanesDisplayXRes = res;
		updateClipPlanesVBOs();
	}
}

size_t ClippingShader::getClipPlanesDisplayXRes()
{
	return m_clipPlanesDisplayXRes;
}

void ClippingShader::setClipPlanesDisplayYRes(size_t res)
{
	if (res != m_clipPlanesDisplayYRes)
	{
		m_clipPlanesDisplayYRes = res;
		updateClipPlanesVBOs();
	}
}

size_t ClippingShader::getClipPlanesDisplayYRes()
{
	return m_clipPlanesDisplayYRes;
}

void ClippingShader::setClipPlanesDisplaySize(float size)
{
	if (size != m_clipPlanesDisplaySize)
	{
		m_clipPlanesDisplaySize = size;
		updateClipPlanesVBOs();
	}
}

float ClippingShader::getClipPlanesDisplaySize()
{
	return m_clipPlanesDisplaySize;
}

void ClippingShader::updateClipPlaneVBO(int planeIndex)
{
	// Check if the given index is out of range
	if (errorRaiseParameterIsOutOfRange(((planeIndex < 0) || (planeIndex > (getClipPlanesCount() - 1))), "ClippingShader::updateClipPlaneVBO", "planeIndex"))
			return;

	// Compute four point of the plane at equal distance from plane origin
	Geom::Vec3f p1 = m_clipPlanes[planeIndex].origin
			+ (0.5f * m_clipPlanesDisplaySize) * m_clipPlanes[planeIndex].firstVec
			+ (0.5f * m_clipPlanesDisplaySize) * m_clipPlanes[planeIndex].secondVec;
	Geom::Vec3f p2 = m_clipPlanes[planeIndex].origin
			+ (0.5f * m_clipPlanesDisplaySize) * m_clipPlanes[planeIndex].firstVec
			- (0.5f * m_clipPlanesDisplaySize) * m_clipPlanes[planeIndex].secondVec;
	Geom::Vec3f p3 = m_clipPlanes[planeIndex].origin
			- (0.5f * m_clipPlanesDisplaySize) * m_clipPlanes[planeIndex].firstVec
			- (0.5f * m_clipPlanesDisplaySize) * m_clipPlanes[planeIndex].secondVec;
	Geom::Vec3f p4 = m_clipPlanes[planeIndex].origin
			- (0.5f * m_clipPlanesDisplaySize) * m_clipPlanes[planeIndex].firstVec
			+ (0.5f * m_clipPlanesDisplaySize) * m_clipPlanes[planeIndex].secondVec;


	// Build the VBO with the new points

	m_clipPlanesDrawers[planeIndex]->newList(GL_COMPILE);

	// Only display the grid if both x and y resolutions are not zero
	if ( (m_clipPlanesDisplayXRes != 0) && (m_clipPlanesDisplayYRes != 0) )
	{
		float t;
		Geom::Vec3f p1p2Interp;
		Geom::Vec3f p4p3Interp;
		Geom::Vec3f p2p3Interp;
		Geom::Vec3f p1p4Interp;

		// X lines
		for (size_t i = 0; i <= m_clipPlanesDisplayXRes; i++)
		{
			// Compute the linear interpolation parameter from the current value of 'i'
			t = (float)i / (float)m_clipPlanesDisplayXRes;

			// Grid construction

			// Compute linear interpolations between points
			p1p2Interp = p1*t + p2*(1.0 - t);
			p4p3Interp = p4*t + p3*(1.0 - t);

			// Draw lines between the resulting points
			m_clipPlanesDrawers[planeIndex]->begin(GL_LINES);
			m_clipPlanesDrawers[planeIndex]->color3f(
					m_clipPlanesDisplayColor[0],
					m_clipPlanesDisplayColor[1],
					m_clipPlanesDisplayColor[2]);
			m_clipPlanesDrawers[planeIndex]->vertex3f(
					p1p2Interp[0],
					p1p2Interp[1],
					p1p2Interp[2]);
			m_clipPlanesDrawers[planeIndex]->vertex3f(
					p4p3Interp[0],
					p4p3Interp[1],
					p4p3Interp[2]);
			m_clipPlanesDrawers[planeIndex]->end();
		}

		// Y lines
		for (size_t i = 0; i <= m_clipPlanesDisplayYRes; i++)
		{
			// Compute the linear interpolation parameter from the current value of 'i'
			t = (float)i / (float)m_clipPlanesDisplayYRes;

			// Grid construction

			// Compute linear interpolations between points
			p2p3Interp = p2*t + p3*(1.0 - t);
			p1p4Interp = p1*t + p4*(1.0 - t);

			// Draw lines between the resulting points
			m_clipPlanesDrawers[planeIndex]->begin(GL_LINES);
			m_clipPlanesDrawers[planeIndex]->color3f(
					m_clipPlanesDisplayColor[0],
					m_clipPlanesDisplayColor[1],
					m_clipPlanesDisplayColor[2]);
			m_clipPlanesDrawers[planeIndex]->vertex3f(
					p2p3Interp[0],
					p2p3Interp[1],
					p2p3Interp[2]);
			m_clipPlanesDrawers[planeIndex]->vertex3f(
					p1p4Interp[0],
					p1p4Interp[1],
					p1p4Interp[2]);
			m_clipPlanesDrawers[planeIndex]->end();
		}

	}

	m_clipPlanesDrawers[planeIndex]->endList();
}

void ClippingShader::updateClipPlanesVBOs()
{
	for (int i = 0; i < getClipPlanesCount(); i++)
		updateClipPlaneVBO(i);
}


/***********************************************
 *
 * 		Sphere Clipping
 *
 ***********************************************/


void ClippingShader::setClipSpheresCount(int spheresCount)
{
	// Check if the clipping code has been inserted into shader
	if (errorRaiseClippingCodeNotInserted(!m_hasClippingCodeBeenInserted, "ClippingShader::setClipSpheresCount"))
		return;

	// Check if the given clipping spheres count is valid
	if (errorRaiseParameterIsNotPositive((spheresCount < 0), "ClippingShader::setClipSpheresCount", "spheresCount"))
		return;

	// Shader name string
	std::string shaderName = m_nameVS + "/" + m_nameFS + "/" + m_nameGS;

	// Use a shader mutator
	ShaderMutator SM(shaderName, getVertexShaderSrc(), getFragmentShaderSrc());

	// Modify the clip spheres count constant in both shader
	SM.changeIntConstantValue(ShaderMutator::VERTEX_SHADER, "CLIP_SPHERES_COUNT", spheresCount);
	SM.changeIntConstantValue(ShaderMutator::FRAGMENT_SHADER, "CLIP_SPHERES_COUNT", spheresCount);

	// Reload both shaders
	reloadVertexShaderFromMemory(SM.getModifiedVertexShaderSrc().c_str());
	reloadFragmentShaderFromMemory(SM.getModifiedFragmentShaderSrc().c_str());


	// Previous spheres count
	int previousSpheresCount = getClipSpheresCount();

	// Resize the spheres arrays to the right size
	m_clipSpheres.resize((size_t)spheresCount);
	m_clipSpheresCentersAndRadiuses.resize(4*(size_t)spheresCount, 0.0);

	// Resize the spheres drawers array to the right size, and create/destroy objects
	if (spheresCount > previousSpheresCount)
	{
		m_clipSpheresDrawers.resize((size_t)spheresCount, NULL);
		for (int i = previousSpheresCount; i < spheresCount; i++)
			m_clipSpheresDrawers[i] = new Drawer;
	}
	else
	{
		//for (int i = spheresCount; i < previousSpheresCount; i++)
			//delete m_clipSpheresDrawers[i]; // TODO : Bug dans drawer, crash le prochain affichage
		m_clipSpheresDrawers.resize((size_t)spheresCount);
	}

	// Set default parameters values for new spheres
	if (spheresCount > previousSpheresCount)
	{
		Geom::Vec3f defaultCenter (0.0, 0.0, 0.0);
		float defaultRadius = 10.0;

		for (int i = previousSpheresCount; i < spheresCount; i++)
			setClipSphereParamsAll(defaultCenter, defaultRadius, i);
	}

	// Recompile shaders (automatically calls updateClippingUniforms)
	recompile();
}

int ClippingShader::getClipSpheresCount()
{
	return (int)m_clipSpheres.size();
}

void ClippingShader::setClipSphereParamsAll(Geom::Vec3f center, float radius, int sphereIndex)
{
	// Check if the given index is out of range
	if (errorRaiseParameterIsOutOfRange(((sphereIndex < 0) || (sphereIndex > (getClipSpheresCount() - 1))), "ClippingShader::setClipSphereParamsAll", "sphereIndex"))
			return;

	if ((center != m_clipSpheres[sphereIndex].center)
			|| (radius != m_clipSpheres[sphereIndex].radius))
	{
		// Copy the given clipping sphere parameters
		m_clipSpheres[sphereIndex].center = center;
		m_clipSpheres[sphereIndex].radius = radius;

		// Update the sphere array
		updateClipSphereArray(sphereIndex);

		// Send again the whole spheres centers and radiuses array to shader
		sendClipSpheresCentersAndRadiusesUniform();

		// Update sphere VBO
		updateClipSphereVBO(sphereIndex);
	}
}

void ClippingShader::setClipSphereParamsCenter(Geom::Vec3f center, int sphereIndex)
{
	// Check if the given index is out of range
	if (errorRaiseParameterIsOutOfRange(((sphereIndex < 0) || (sphereIndex > (getClipSpheresCount() - 1))), "ClippingShader::setClipSphereParamsCenter", "sphereIndex"))
			return;

	if (center != m_clipSpheres[sphereIndex].center)
	{
		// Copy the given clipping sphere parameter
		m_clipSpheres[sphereIndex].center = center;

		// Update the sphere array
		updateClipSphereArray(sphereIndex);

		// Send again the whole spheres centers and radiuses array to shader
		sendClipSpheresCentersAndRadiusesUniform();

		// Update sphere VBO
		updateClipSphereVBO(sphereIndex);
	}
}

void ClippingShader::setClipSphereParamsRadius(float radius, int sphereIndex)
{
	// Check if the given index is out of range
	if (errorRaiseParameterIsOutOfRange(((sphereIndex < 0) || (sphereIndex > (getClipSpheresCount() - 1))), "ClippingShader::setClipSphereParamsRadius", "sphereIndex"))
			return;

	if (radius != m_clipSpheres[sphereIndex].radius)
	{
		// Copy the given clipping sphere parameter
		m_clipSpheres[sphereIndex].radius = radius;

		// Update the sphere array
		updateClipSphereArray(sphereIndex);

		// Send again the whole spheres centers and radiuses array to shader
		sendClipSpheresCentersAndRadiusesUniform();

		// Update sphere VBO
		updateClipSphereVBO(sphereIndex);
	}
}

Geom::Vec3f ClippingShader::getClipSphereParamsCenter(int sphereIndex)
{
	// Check if the given index is out of range
	if (errorRaiseParameterIsOutOfRange(((sphereIndex < 0) || (sphereIndex > (getClipSpheresCount() - 1))), "ClippingShader::getClipSphereParamsCenter", "sphereIndex"))
			return Geom::Vec3f(0.0, 0.0, 0.0);

	// Return the parameter
	return m_clipSpheres[sphereIndex].center;
}

float ClippingShader::getClipSphereParamsRadius(int sphereIndex)
{
	// Check if the given index is out of range
	if (errorRaiseParameterIsOutOfRange(((sphereIndex < 0) || (sphereIndex > (getClipSpheresCount() - 1))), "ClippingShader::getClipSphereParamsRadius", "sphereIndex"))
			return 0.0;

	// Return the parameter
	return m_clipSpheres[sphereIndex].radius;
}

void ClippingShader::updateClipSphereArray(int sphereIndex)
{
	// Check if the given index is out of range
	if (errorRaiseParameterIsOutOfRange(((sphereIndex < 0) || (sphereIndex > (getClipSpheresCount() - 1))), "ClippingShader::updateClipSphereArray", "sphereIndex"))
			return;

	// Update the spheres centers and radiuses array
	m_clipSpheresCentersAndRadiuses[4*sphereIndex + 0] = m_clipSpheres[sphereIndex].center[0];
	m_clipSpheresCentersAndRadiuses[4*sphereIndex + 1] = m_clipSpheres[sphereIndex].center[1];
	m_clipSpheresCentersAndRadiuses[4*sphereIndex + 2] = m_clipSpheres[sphereIndex].center[2];
	m_clipSpheresCentersAndRadiuses[4*sphereIndex + 3] = m_clipSpheres[sphereIndex].radius;

}


/***********************************************
 *
 * 		Sphere Clipping Display
 *
 ***********************************************/


void ClippingShader::displayClipSpheres()
{
	for (size_t i = 0; i < m_clipSpheresDrawers.size(); i++)
		m_clipSpheresDrawers[i]->callList();
}

void ClippingShader::setClipSpheresDisplayColor(Geom::Vec3f color)
{
	if (color != m_clipSpheresDisplayColor)
	{
		m_clipSpheresDisplayColor = color;
		updateClipSpheresVBOs();
	}
}

Geom::Vec3f ClippingShader::getClipSpheresDisplayColor()
{
	return m_clipSpheresDisplayColor;
}

void ClippingShader::setClipSpheresDisplayXRes(size_t res)
{
	if (res != m_clipSpheresDisplayXRes)
	{
		m_clipSpheresDisplayXRes = res;
		updateClipSpheresVBOs();
	}
}

size_t ClippingShader::getClipSpheresDisplayXRes()
{
	return m_clipSpheresDisplayXRes;
}

void ClippingShader::setClipSpheresDisplayYRes(size_t res)
{
	if (res != m_clipSpheresDisplayYRes)
	{
		m_clipSpheresDisplayYRes = res;
		updateClipSpheresVBOs();
	}
}

size_t ClippingShader::getClipSpheresDisplayYRes()
{
	return m_clipSpheresDisplayYRes;
}

void ClippingShader::updateClipSphereVBO(int sphereIndex)
{
	// Check if the given index is out of range
	if (errorRaiseParameterIsOutOfRange(((sphereIndex < 0) || (sphereIndex > (getClipSpheresCount() - 1))), "ClippingShader::updateClipSphereVBO", "sphereIndex"))
			return;

	// Build the VBO
	m_clipSpheresDrawers[sphereIndex]->newList(GL_COMPILE);

	m_clipSpheresDrawers[sphereIndex]->color3f(
			m_clipSpheresDisplayColor[0],
			m_clipSpheresDisplayColor[1],
			m_clipSpheresDisplayColor[2]);

	m_clipSpheresDrawers[sphereIndex]->begin(GL_LINES);

	// Compute angle displacement steps
	float dTheta = 0.0;
	if (m_clipSpheresDisplayXRes != 0)
		dTheta = 2 * M_PI / (float)m_clipSpheresDisplayXRes;
	float dPhi = 0.0;
	if (m_clipSpheresDisplayYRes != 0)
		dPhi = M_PI / (float)m_clipSpheresDisplayYRes;

	// Draw the sphere in wireframe
	for (size_t i = 0; i < m_clipSpheresDisplayXRes; i++)
	{
		for (size_t j = 0; j < m_clipSpheresDisplayYRes; j++)
		{
			// Compute 3 points according to the parametric equation of the sphere
			Geom::Vec3f p1 (
					cos(i*dTheta) * sin(j*dPhi),
					sin(i*dTheta) * sin(j*dPhi),
					cos(j*dPhi));
			Geom::Vec3f p2 (
					cos((i+1)*dTheta) * sin(j*dPhi),
					sin((i+1)*dTheta) * sin(j*dPhi),
					cos(j*dPhi));
			Geom::Vec3f p3 (
					cos(i*dTheta) * sin((j+1)*dPhi),
					sin(i*dTheta) * sin((j+1)*dPhi),
					cos((j+1)*dPhi));

			// Scale with the radius
			p1 *= m_clipSpheres[sphereIndex].radius;
			p2 *= m_clipSpheres[sphereIndex].radius;
			p3 *= m_clipSpheres[sphereIndex].radius;

			// Translate to the center
			p1 += m_clipSpheres[sphereIndex].center;
			p2 += m_clipSpheres[sphereIndex].center;
			p3 += m_clipSpheres[sphereIndex].center;

			// Draw two lines with the 3 points
			m_clipSpheresDrawers[sphereIndex]->vertex(p1);
			m_clipSpheresDrawers[sphereIndex]->vertex(p2);
			m_clipSpheresDrawers[sphereIndex]->vertex(p1);
			m_clipSpheresDrawers[sphereIndex]->vertex(p3);
		}
	}

	m_clipSpheresDrawers[sphereIndex]->end();

	m_clipSpheresDrawers[sphereIndex]->endList();
}

void ClippingShader::updateClipSpheresVBOs()
{
	for (int i = 0; i < getClipSpheresCount(); i++)
		updateClipSphereVBO(i);
}


/***********************************************
 *
 * 		Global Clipping Stuff
 *
 ***********************************************/


bool ClippingShader::insertClippingCode()
{
	// Check if the code has not been already inserted
	if (errorRaiseClippingCodeAlreadyInserted(m_hasClippingCodeBeenInserted, "ClippingShader::insertClippingCode"))
		return false;

	// Check if the vertex and fragment sources are not empty
	if (errorRaiseShaderSourceIsEmpty((getVertexShaderSrc() == NULL), "ClippingShader::insertClippingCode", ShaderMutator::VERTEX_SHADER))
		return false;
	if (errorRaiseShaderSourceIsEmpty((getFragmentShaderSrc() == NULL), "ClippingShader::insertClippingCode", ShaderMutator::FRAGMENT_SHADER))
			return false;

	// Check if the shader does not use a geometry shader
	if (errorRaiseShaderUsesGeometryShader((getGeometryShaderSrc() != NULL), "ClippingShader::insertClippingCode"))
		return false;


	// Strings to insert in shader sources

	std::string VS_headInsertion =
	"\n"
	"#define CLIP_PLANES_COUNT 0\n"
	"#define CLIP_SPHERES_COUNT 0\n"
	"\n"
	"#define PLANE_CLIPPING_ENABLED (CLIP_PLANES_COUNT > 0)\n"
	"#define SPHERE_CLIPPING_ENABLED (CLIP_SPHERES_COUNT > 0)\n"
	"\n"
	"#define CLIPPING_ENABLED (PLANE_CLIPPING_ENABLED || SPHERE_CLIPPING_ENABLED)\n"
	"\n"
	"#if CLIPPING_ENABLED\n"
	"	VARYING_VERT vec3 clip_nonTransformedPos;\n"
	"#endif\n"
	"\n";

	std::string VS_mainEndInsertion =
	"\n"
	"	#if CLIPPING_ENABLED\n"
	"		// Pass the non transformed vertex position to the fragment shader for clipping\n"
	"		clip_nonTransformedPos = VertexPosition;\n"
	"	#endif\n";

	std::string FS_headInsertion =
	"\n"
	"#define CLIP_PLANES_COUNT 0\n"
	"#define CLIP_SPHERES_COUNT 0\n"
	"\n"
	"#define PLANE_CLIPPING_ENABLED (CLIP_PLANES_COUNT > 0)\n"
	"#define SPHERE_CLIPPING_ENABLED (CLIP_SPHERES_COUNT > 0)\n"
	"\n"
	"#define CLIPPING_ENABLED (PLANE_CLIPPING_ENABLED || SPHERE_CLIPPING_ENABLED)\n"
	"\n"
	"#if CLIPPING_ENABLED\n"
	"\n"
	"	#if PLANE_CLIPPING_ENABLED\n"
	"		uniform vec4 clip_clipPlanesEquations[CLIP_PLANES_COUNT];\n"
	"	#endif\n"
	"\n"
	"	#if SPHERE_CLIPPING_ENABLED\n"
	"		uniform vec4 clip_clipSpheresCentersAndRadiuses[CLIP_SPHERES_COUNT];\n"
	"	#endif\n"
	"\n"
	"	uniform float clip_clipColorAttenuationFactor;\n"
	"\n"
	"	VARYING_FRAG vec3 clip_nonTransformedPos;\n"
	"\n"
	"#endif\n"
	"\n"
	"#if CLIPPING_ENABLED\n"

	"	float clip_doClippingAndGetClippingDistance()\n"
	"	{\n"
	"		// Distance to the nearest clipping object\n"
	"		float minDistanceToClipping = -1.0;\n"
	"\n"
	"		#if PLANE_CLIPPING_ENABLED\n"
	"\n"
	"			// Do clipping for each plane\n"
	"			for (int i = 0; i < CLIP_PLANES_COUNT; i++)\n"
	"			{\n"
	"				// Get the current plane equation\n"
	"				vec4 currClipPlane = clip_clipPlanesEquations[i];\n"
	"\n"
	"				// If the plane normal is zero, use a default normal vector (0.0, 0.0, 1.0)\n"
	"				float clipPlaneNormalLength = length(currClipPlane.xyz);\n"
	"				if (clipPlaneNormalLength == 0.0)\n"
	"				{\n"
	"					currClipPlane.z = 1.0;\n"
	"					clipPlaneNormalLength = 1.0;\n"
	"				}\n"
	"\n"
	"				// Signed distance between the point and the plane\n"
	"				float distanceToPlane = dot(clip_nonTransformedPos, currClipPlane.xyz);\n"
	"				distanceToPlane += currClipPlane.w;\n"
	"				distanceToPlane /= clipPlaneNormalLength;\n"
	"\n"
	"				// Keep the fragment only if it is 'above' the plane\n"
	"				if (distanceToPlane < 0.0)\n"
	"					discard;\n"
	"				else\n"
	"				{\n"
	"					// Keep the distance to the nearest plane\n"
	"					if (minDistanceToClipping < 0.0)\n"
	"						minDistanceToClipping = distanceToPlane;\n"
	"					else\n"
	"						minDistanceToClipping = min(minDistanceToClipping, distanceToPlane);\n"
	"				}\n"
	"			}\n"
	"\n"
	"		#endif\n"
	"\n"
	"		#if SPHERE_CLIPPING_ENABLED\n"

	"			// Do clipping for each sphere\n"
	"			for (int i = 0; i < CLIP_SPHERES_COUNT; i++)\n"
	"			{\n"
	"				// Get the current sphere center and radius\n"
	"				vec3 currClipSphereCenter = clip_clipSpheresCentersAndRadiuses[i].xyz;\n"
	"				float currClipSphereRadius = clip_clipSpheresCentersAndRadiuses[i].w;\n"
	"\n"
	"				// If the sphere radius is negative, bring it back to zero\n"
	"				if (currClipSphereRadius < 0.0)\n"
	"					currClipSphereRadius = 0.0;\n"
	"\n"
	"				// Signed distance between the point and the sphere\n"
	"				float distanceToSphere = length(clip_nonTransformedPos - currClipSphereCenter);\n"
	"				distanceToSphere -= currClipSphereRadius;\n"
	"\n"
	"				// Keep the fragment only if it is inside the sphere\n"
	"				if (distanceToSphere > 0.0)\n"
	"					discard;\n"
	"				else\n"
	"				{\n"
	"					// Keep the distance to the nearest sphere\n"
	"					if (minDistanceToClipping < 0.0)\n"
	"						minDistanceToClipping = -distanceToSphere;\n"
	"					else\n"
	"						minDistanceToClipping = min(minDistanceToClipping, -distanceToSphere);\n"
	"				}\n"
	"			}\n"
	"\n"
	"		#endif\n"
	"\n"
	"		return minDistanceToClipping;\n"
	"	}\n"
	"\n"
	"#endif\n"
	"\n";

	std::string FS_mainBeginInsertion =
	"\n"
	"	#if CLIPPING_ENABLED\n"
	"		// Apply clipping and get the clipping distance\n"
	"		float clip_minDistanceToClipping = clip_doClippingAndGetClippingDistance();\n"
	"	#endif\n";

	std::string FS_mainEndInsertion =
	"\n"
	"	#if CLIPPING_ENABLED\n"
	"		// Attenuate the final fragment color depending on its distance to the clipping\n"
	"		gl_FragColor.rgb /= (1.0 + clip_minDistanceToClipping * clip_clipColorAttenuationFactor);\n"
	"	#endif\n";

	// Shader name string
	std::string shaderName = m_nameVS + "/" + m_nameFS + "/" + m_nameGS;

	// Use a shader mutator
	ShaderMutator SM(shaderName, getVertexShaderSrc(), getFragmentShaderSrc());

	// First check if the vertex shader contains the VertexPosition attribute
	if (errorRaiseVariableNotFoundInShader(!SM.containsVariableDeclaration(ShaderMutator::VERTEX_SHADER, "VertexPosition"), "ClippingShader::insertClippingCode", ShaderMutator::VERTEX_SHADER, "VertexPosition"))
		return false;

	// Modify vertex shader source code
	SM.insertCodeBeforeMainFunction(ShaderMutator::VERTEX_SHADER, VS_headInsertion);
	SM.insertCodeAtMainFunctionBeginning(ShaderMutator::VERTEX_SHADER, VS_mainEndInsertion);

	// Modify fragment shader source code
	SM.setMinShadingLanguageVersion(ShaderMutator::FRAGMENT_SHADER, 120); // Following code insertions need at least shading language 120 (GLSL arrays)
	SM.insertCodeBeforeMainFunction(ShaderMutator::FRAGMENT_SHADER, FS_headInsertion);
	SM.insertCodeAtMainFunctionBeginning(ShaderMutator::FRAGMENT_SHADER, FS_mainBeginInsertion);
	SM.insertCodeAtMainFunctionEnd(ShaderMutator::FRAGMENT_SHADER, FS_mainEndInsertion);

	// Reload both shaders
	reloadVertexShaderFromMemory(SM.getModifiedVertexShaderSrc().c_str());
	reloadFragmentShaderFromMemory(SM.getModifiedFragmentShaderSrc().c_str());

	// Recompile shaders (automatically calls updateClippingUniforms)
	recompile();

	m_hasClippingCodeBeenInserted = true;

	return true;
}

void ClippingShader::setClipColorAttenuationFactor(float colorAttenuationFactor)
{
	if (m_clipColorAttenuationFactor != colorAttenuationFactor)
	{
		// Copy the given value
		m_clipColorAttenuationFactor = colorAttenuationFactor;

		// Send again the uniform to shader
		sendClipColorAttenuationFactorUniform();
	}
}

float ClippingShader::getClipColorAttenuationFactor()
{
	return m_clipColorAttenuationFactor;
}


/***********************************************
 *
 * 		Clipping Uniforms Handling
 *
 ***********************************************/


void ClippingShader::updateClippingUniforms()
{
	// Plane clipping uniforms
	if (getClipPlanesCount() > 0)
	{
		// Get uniform location
		m_unif_clipPlanesEquations = glGetUniformLocation(program_handler(), "clip_clipPlanesEquations");
		errorRaiseUniformNotFoundInShader((m_unif_clipPlanesEquations == -1), "ClippingShader::updateClippingUniforms", "clip_clipPlanesEquations");

		// Send again uniform value
		sendClipPlanesEquationsUniform();
	}

	// Sphere clipping uniforms
	if (getClipSpheresCount() > 0)
	{
		// Get uniform location
		m_unif_clipSpheresCentersAndRadiuses = glGetUniformLocation(program_handler(), "clip_clipSpheresCentersAndRadiuses");
		errorRaiseUniformNotFoundInShader((m_unif_clipSpheresCentersAndRadiuses == -1), "ClippingShader::updateClippingUniforms", "clip_clipSpheresCentersAndRadiuses");

		// Send again uniform value
		sendClipSpheresCentersAndRadiusesUniform();
	}

	// Global clipping uniforms
	if ((getClipPlanesCount() > 0) || (getClipSpheresCount() > 0))
	{
		// Get uniform location
		m_unif_clipColorAttenuationFactor = glGetUniformLocation(program_handler(), "clip_clipColorAttenuationFactor");
		errorRaiseUniformNotFoundInShader((m_unif_clipColorAttenuationFactor == -1), "ClippingShader::updateClippingUniforms", "clip_clipColorAttenuationFactor");

		// Send again uniform value
		sendClipColorAttenuationFactorUniform();
	}
}

void ClippingShader::sendClipPlanesEquationsUniform()
{
	bind();
	glUniform4fv(m_unif_clipPlanesEquations, getClipPlanesCount(), &m_clipPlanesEquations.front());
}

void ClippingShader::sendClipSpheresCentersAndRadiusesUniform()
{
	bind();
	glUniform4fv(m_unif_clipSpheresCentersAndRadiuses, getClipSpheresCount(), &m_clipSpheresCentersAndRadiuses.front());
}

void ClippingShader::sendClipColorAttenuationFactorUniform()
{
	bind();
	glUniform1f(m_unif_clipColorAttenuationFactor, m_clipColorAttenuationFactor);
}


/***********************************************
 *
 * 		Error Raising
 *
 ***********************************************/


bool ClippingShader::errorRaiseParameterIsNotPositive(bool condition, const std::string& location, const std::string& paramName)
{
	if (condition)
	{
		CGoGNerr
		<< "ERROR - "
		<< location
		<< " - Given value for "
		<< paramName
		<< " is not positive"
		<< CGoGNendl;
	}

	return condition;
}

bool ClippingShader::errorRaiseShaderSourceIsEmpty(bool condition, const std::string& location, ShaderMutator::shaderSrcType shaderType)
{
	if (condition)
	{
		std::string shaderName;
		switch (shaderType)
		{
			case ShaderMutator::VERTEX_SHADER :
				shaderName = m_nameVS;
				break;

			case ShaderMutator::FRAGMENT_SHADER :
				shaderName = m_nameFS;
				break;

			case ShaderMutator::GEOMETRY_SHADER :
				shaderName = m_nameGS;
				break;

			default :
				shaderName = m_nameVS;
				break;
		}

		CGoGNerr
		<< "ERROR - "
		<< location
		<< " - Could not process shader "
		<< shaderName
		<< " source code : shader source is empty"
		<< CGoGNendl;
	}

	return condition;
}

bool ClippingShader::errorRaiseShaderUsesGeometryShader(bool condition, const std::string& location)
{
	if (condition)
	{
		std::string shaderName = m_nameVS + "/" + m_nameFS + "/" + m_nameGS;

		CGoGNerr
		<< "ERROR - "
		<< location
		<< " - Could not process shader "
		<< shaderName
		<< " source code : unable to add clipping to a shader which uses a geometry shader"
		<< CGoGNendl;
	}

	return condition;
}

bool ClippingShader::errorRaiseVariableNotFoundInShader(bool condition, const std::string& location, ShaderMutator::shaderSrcType shaderType, const std::string& varName)
{
	if (condition)
	{
		std::string shaderName;
		switch (shaderType)
		{
			case ShaderMutator::VERTEX_SHADER :
				shaderName = m_nameVS;
				break;

			case ShaderMutator::FRAGMENT_SHADER :
				shaderName = m_nameFS;
				break;

			case ShaderMutator::GEOMETRY_SHADER :
				shaderName = m_nameGS;
				break;

			default :
				shaderName = m_nameVS;
				break;
		}

		CGoGNerr
		<< "ERROR - "
		<< location
		<< " - Could not process shader "
		<< shaderName
		<< " source code : "
		<< varName
		<< " not found"
		<< CGoGNendl;
	}

	return condition;
}
bool ClippingShader::errorRaiseParameterIsOutOfRange(bool condition, const std::string& location, const std::string& paramName)
{
	if (condition)
	{
		CGoGNerr
		<< "ERROR - "
		<< location
		<< " - Given value for "
		<< paramName
		<< " is out of range"
		<< CGoGNendl;
	}

	return condition;
}

bool ClippingShader::errorRaiseUniformNotFoundInShader(bool condition, const std::string& location, const std::string& uniformName)
{
	if (condition)
	{
		std::string shaderName = m_nameVS + "/" + m_nameFS + "/" + m_nameGS;

		CGoGNerr
		<< "ERROR - "
		<< location
		<< " - Uniform "
		<< uniformName
		<< " not found in shader "
		<< shaderName
		<< CGoGNendl;
	}

	return condition;
}

bool ClippingShader::errorRaiseClippingCodeAlreadyInserted(bool condition, const std::string& location)
{
	if (condition)
	{
		std::string shaderName = m_nameVS + "/" + m_nameFS + "/" + m_nameGS;

		CGoGNerr
		<< "ERROR - "
		<< location
		<< " - Clipping code has already been inserted into shader "
		<< shaderName
		<< CGoGNendl;
	}

	return condition;
}

bool ClippingShader::errorRaiseClippingCodeNotInserted(bool condition, const std::string& location)
{
	if (condition)
	{
		std::string shaderName = m_nameVS + "/" + m_nameFS + "/" + m_nameGS;

		CGoGNerr
		<< "ERROR - "
		<< location
		<< " - Clipping code must be inserted into shader "
		<< shaderName
		<< " before adding clipping objects"
		<< CGoGNendl;
	}

	return condition;
}

} // namespace Utils

} // namespace CGoGN
