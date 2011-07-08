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
	m_unif_colorAttenuationFactor = 0;


	// Initialize default display clipping variables
	m_clipPlanesDisplayColor = Geom::Vec3f (1.0, 0.6, 0.0);
	m_clipPlanesDisplayType = STRAIGHT_GRID;
	m_clipPlanesDisplayXRes = 2;
	m_clipPlanesDisplayYRes = 2;
	m_clipPlanesDisplaySize = 10.0;

	// Initialize default global clipping variables
	m_colorAttenuationFactor = 1.0;
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
	// Verify that the given clipping planes count is valid
	if (planesCount < 0)
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::setPlanesClipping"
		<< " - Given clipping planes count is not positive !"
		<< CGoGNendl;
		return;
	}

	// Shader name
	std::string shaderName = m_nameVS + "/" + m_nameFS + "/" + m_nameGS;

	// Verify that the shader has been well created
	if (!isCreated())
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::setPlaneClipping"
		<< " - Could not process shader "
		<< shaderName
		<< " source code : shader has not been created or has failed to compile"
		<< CGoGNendl;
		return;
	}
	
	// Verify that the shader does not use a geometry shader
	if (getGeometryShaderSrc() != NULL)
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::setPlaneClipping"
		<< " - Could not process shader "
		<< shaderName
		<< " source code : unable to add clipping to a shader which uses a geometry shader"
		<< CGoGNendl;
		return;
	}

	// String for clipping planes count
	std::string planesCountStr;
	std::stringstream ss;
	ss << planesCount;
	planesCountStr = ss.str();

	// Strings that will be inserted into the source code

	std::string VS_head_insertion =
	"\n"
	"VARYING_VERT vec3 clip_NonTransformedPos;\n"
	"\n";

	std::string VS_mainBegin_insertion =
	"\n"
	"	// Pass the non transformed vertex position to the fragment shader for clipping\n"
	"	clip_NonTransformedPos = VertexPosition;\n";

	std::string FS_head_insertion =
	"\n"
	"#define CLIP_PLANES_COUNT " + planesCountStr + "\n"
	"\n"
	"uniform vec4 clip_ClipPlanes[CLIP_PLANES_COUNT];\n"
	"uniform float clip_ColorAttenuationFactor;\n"
	"\n"
	"VARYING_FRAG vec3 clip_NonTransformedPos;\n"
	"\n";

	std::string FS_mainBegin_insertion =
	"\n"
	"	// Distance to the nearest plane, stored for color attenuation\n"
	"	float clip_MinDistanceToPlanes = -1.0;\n"
	"\n"
	"	// Do clipping for each plane\n"
	"	int i;\n"
	"	for (i = 0; i < CLIP_PLANES_COUNT; i++)\n"
	"	{\n"
	"		// Copy the plane to make it modifiable\n"
	"		vec4 clip_CurrClipPlane = clip_ClipPlanes[i];\n"
	"\n"
	"		// If the plane normal is zero, use a default normal vector (0.0, 0.0, 1.0)\n"
	"		float clip_NPlane = length(clip_CurrClipPlane.xyz);\n"
	"		if (clip_NPlane == 0.0)\n"
	"		{\n"
	"			clip_CurrClipPlane.z = 1.0;\n"
	"			clip_NPlane = 1.0;\n"
	"		}\n"
	"\n"
	"		// Signed distance between the point and the plane\n"
	"		float clip_DistanceToPlane = dot(clip_NonTransformedPos, clip_CurrClipPlane.xyz);\n"
	"		clip_DistanceToPlane += clip_CurrClipPlane.w;\n"
	"		clip_DistanceToPlane /= clip_NPlane;\n"
	"\n"
	"		// Keep the fragment only if it is 'above' the plane\n"
	"		if (clip_DistanceToPlane < 0.0)\n"
	"			discard;\n"
	"		else\n"
	"		{\n"
	"			// Keep the distance to the nearest plane\n"
	"			if (clip_MinDistanceToPlanes < 0.0)\n"
	"				clip_MinDistanceToPlanes = clip_DistanceToPlane;\n"
	"			else\n"
	"				clip_MinDistanceToPlanes = min(clip_MinDistanceToPlanes, clip_DistanceToPlane);\n"
	"		}\n"
	"	}\n";

	std::string FS_mainEnd_insertion =
	"\n"
	"	// Attenuate the final fragment color depending on its distance to the nearest plane\n"
	"	if (clip_MinDistanceToPlanes > 0.0)\n"
	"		gl_FragColor.rgb /= (1.0 + clip_MinDistanceToPlanes*clip_ColorAttenuationFactor);\n";

	// Previous planes count
	int previousPlanesCount = getClipPlanesCount();

	// If the previous planes count was zero, the previous shader source codes were the original ones. Store them
	// (the planes count is initially zero when the object is constructed)
	if (previousPlanesCount == 0)
	{
		originalVertShaderSrc = getVertexShaderSrc();
		originalFragShaderSrc = getFragmentShaderSrc();
	}

	// If the given plane count is > 0, modify the shader sources
	if (planesCount > 0)
	{

		// If the previous planes count wasn't zero, there is only one constant to change in the fragment shader
		if (previousPlanesCount != 0)
		{
			// Use a shader mutator
			ShaderMutator SM(shaderName, getVertexShaderSrc(), getFragmentShaderSrc());

			// Modify the constant in the fragment shader
			SM.changeIntConstantValue(ShaderMutator::FRAGMENT_SHADER, "CLIP_PLANES_COUNT", planesCount);

			// Reload fragment shader
			reloadFragmentShaderFromMemory(SM.getModifiedFragmentShaderSrc().c_str());
		}
		// Else the whole clipping code must be inserted in the original shader sources
		else
		{
			// Use a shader mutator
			ShaderMutator SM(shaderName, originalVertShaderSrc, originalFragShaderSrc);

			// First check if the vertex shader contains the VertexPosition attribute
			if (!SM.containsVariableDeclaration(ShaderMutator::VERTEX_SHADER, "VertexPosition"))
			{
				CGoGNerr
				<< "ERROR - "
				<< "ClippingShader::setPlaneClipping"
				<< " - Could not process shader "
				<< m_nameVS
				<< " source code : no VertexPosition attribute found"
				<< CGoGNendl;
				return;
			}

			// Modify vertex shader source code
			SM.insertCodeBeforeMainFunction(ShaderMutator::VERTEX_SHADER, VS_head_insertion);
			SM.insertCodeAtMainFunctionBeginning(ShaderMutator::VERTEX_SHADER, VS_mainBegin_insertion);


			// Modify fragment shader source code
			SM.setMinShadingLanguageVersion(ShaderMutator::FRAGMENT_SHADER, 120); // Following code insertions need at least shading language 120 (GLSL arrays)
			SM.insertCodeBeforeMainFunction(ShaderMutator::FRAGMENT_SHADER, FS_head_insertion);
			SM.insertCodeAtMainFunctionEnd(ShaderMutator::FRAGMENT_SHADER, FS_mainEnd_insertion);
			SM.insertCodeAtMainFunctionBeginning(ShaderMutator::FRAGMENT_SHADER, FS_mainBegin_insertion);

			// Reload both shaders
			reloadVertexShaderFromMemory(SM.getModifiedVertexShaderSrc().c_str());
			reloadFragmentShaderFromMemory(SM.getModifiedFragmentShaderSrc().c_str());
		}

	}
	// Else no clipping is wanted anymore, so get back the original shader sources
	else
	{
		// Reload both original shaders
		reloadVertexShaderFromMemory(originalVertShaderSrc.c_str());
		reloadFragmentShaderFromMemory(originalFragShaderSrc.c_str());
	}

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
	// Check if the given index is not out of range
	if ((planeIndex < 0) || (planeIndex > (getClipPlanesCount() - 1)))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::setClippingPlane"
		<< " - Given plane index is out of range"
		<< CGoGNendl;
		return;
	}

	// Copy the given clipping plane parameters
	m_clipPlanes[planeIndex].firstVec = vec1;
	m_clipPlanes[planeIndex].firstVec.normalize();
	m_clipPlanes[planeIndex].secondVec = vec2;
	m_clipPlanes[planeIndex].secondVec.normalize();
	m_clipPlanes[planeIndex].origin = origin;

	// Update the plane arrays
	updateClippingPlaneArray(planeIndex);

	// Send again the whole planes equations array to shader
	sendClippingPlanesUniform();

	// Update plane VBO
	updateClippingPlaneVBO(planeIndex);
}

void ClippingShader::setClipPlaneParamsFirstVec(Geom::Vec3f vec1, int planeIndex)
{
	// Check if the given index is not out of range
	if ((planeIndex < 0) || (planeIndex > (getClipPlanesCount() - 1)))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::setClippingPlane"
		<< " - Given plane index is out of range"
		<< CGoGNendl;
		return;
	}

	// Copy the given clipping plane parameter
	m_clipPlanes[planeIndex].firstVec = vec1;
	m_clipPlanes[planeIndex].firstVec.normalize();

	// Update the plane arrays
	updateClippingPlaneArray(planeIndex);

	// Send again the whole planes equations array to shader
	sendClippingPlanesUniform();

	// Update plane VBO
	updateClippingPlaneVBO(planeIndex);
}

void ClippingShader::setClipPlaneParamsSecondVec(Geom::Vec3f vec2, int planeIndex)
{
	// Check if the given index is not out of range
	if ((planeIndex < 0) || (planeIndex > (getClipPlanesCount() - 1)))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::setClippingPlane"
		<< " - Given plane index is out of range"
		<< CGoGNendl;
		return;
	}

	// Copy the given clipping plane parameter
	m_clipPlanes[planeIndex].secondVec = vec2;
	m_clipPlanes[planeIndex].secondVec.normalize();

	// Update the plane arrays
	updateClippingPlaneArray(planeIndex);

	// Send again the whole planes equations array to shader
	sendClippingPlanesUniform();

	// Update plane VBO
	updateClippingPlaneVBO(planeIndex);
}

void ClippingShader::setClipPlaneParamsOrigin(Geom::Vec3f origin, int planeIndex)
{
	// Check if the given index is not out of range
	if ((planeIndex < 0) || (planeIndex > (getClipPlanesCount() - 1)))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::setClippingPlane"
		<< " - Given plane index is out of range"
		<< CGoGNendl;
		return;
	}

	// Copy the given clipping plane parameter
	m_clipPlanes[planeIndex].origin = origin;

	// Update the plane arrays
	updateClippingPlaneArray(planeIndex);

	// Send again the whole planes equations array to shader
	sendClippingPlanesUniform();

	// Update plane VBO
	updateClippingPlaneVBO(planeIndex);
}

Geom::Vec3f ClippingShader::getClipPlaneParamsFirstVec(int planeIndex)
{
	// Check if the given index is not out of range
	if ((planeIndex < 0) || (planeIndex > (getClipPlanesCount() - 1)))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::setClippingPlane"
		<< " - Given plane index is out of range"
		<< CGoGNendl;
		return Geom::Vec3f(0.0, 0.0, 0.0);
	}

	// Return the parameter
	return m_clipPlanes[planeIndex].firstVec;
}

Geom::Vec3f ClippingShader::getClipPlaneParamsSecondVec(int planeIndex)
{
	// Check if the given index is not out of range
	if ((planeIndex < 0) || (planeIndex > (getClipPlanesCount() - 1)))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::setClippingPlane"
		<< " - Given plane index is out of range"
		<< CGoGNendl;
		return Geom::Vec3f(0.0, 0.0, 0.0);
	}

	// Return the parameter
	return m_clipPlanes[planeIndex].secondVec;
}

Geom::Vec3f ClippingShader::getClipPlaneParamsOrigin(int planeIndex)
{
	// Check if the given index is not out of range
	if ((planeIndex < 0) || (planeIndex > (getClipPlanesCount() - 1)))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::setClippingPlane"
		<< " - Given plane index is out of range"
		<< CGoGNendl;
		return Geom::Vec3f(0.0, 0.0, 0.0);
	}

	// Return the parameter
	return m_clipPlanes[planeIndex].origin;
}

void ClippingShader::displayClipPlanes()
{
	for (size_t i = 0; i < m_clipPlanesDrawers.size(); i++)
		m_clipPlanesDrawers[i]->callList();
}


/***********************************************
 *
 * 		Global Clipping Stuff
 *
 ***********************************************/


void ClippingShader::setClipColorAttenuationFactor(float colorAttenuationFactor)
{
	// Copy the given value
	m_colorAttenuationFactor = colorAttenuationFactor;

	// Send again the uniform to shader
	sendColorAttenuationFactorUniform();
}

float ClippingShader::getClipColorAttenuationFactor()
{
	return m_colorAttenuationFactor;
}


/***********************************************
 *
 * 		Clipping Uniforms Handling
 *
 ***********************************************/


void ClippingShader::updateClippingUniforms()
{
	// These uniforms only exist if the clipping planes count is > 0
	if (getClipPlanesCount() <= 0)
		return;

	// Shader name
	std::string shaderName = m_nameVS + "/" + m_nameFS + "/" + m_nameGS;

	// Get uniforms locations

	m_unif_clipPlanesEquations = glGetUniformLocation(program_handler(), "clip_ClipPlanes");
	if (m_unif_clipPlanesEquations == -1)
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::updateClippingUniforms"
		<< " - uniform 'clip_ClipPlane' not found in shader "
		<< shaderName
		<< CGoGNendl;
	}

	m_unif_colorAttenuationFactor = glGetUniformLocation(program_handler(), "clip_ColorAttenuationFactor");
	if (m_unif_colorAttenuationFactor == -1)
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::updateClippingUniforms"
		<< " - uniform 'clip_ColorAttenuationFactor' not found in shader "
		<< shaderName
		<< CGoGNendl;
	}

	// Send again uniforms values
	sendClippingPlanesUniform();
	sendColorAttenuationFactorUniform();
}


/***********************************************
 *
 * 		Private Section
 *
 ***********************************************/


void ClippingShader::sendClippingPlanesUniform()
{
	bind();
	glUniform4fv(m_unif_clipPlanesEquations, getClipPlanesCount(), &m_clipPlanesEquations.front());
}

void ClippingShader::sendColorAttenuationFactorUniform()
{
	bind();
	glUniform1f(m_unif_colorAttenuationFactor, m_colorAttenuationFactor);
}

void ClippingShader::updateClippingPlaneArray(int planeIndex)
{
	// Check if the given index is not out of range
	if ((planeIndex < 0) || (planeIndex > (getClipPlanesCount() - 1)))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::updateClippingPlaneVBO"
		<< " - Given plane index is out of range"
		<< CGoGNendl;
		return;
	}

	// Update the planes equations array
	Geom::Vec3f planeNormal = m_clipPlanes[planeIndex].firstVec ^ m_clipPlanes[planeIndex].secondVec;
	float d = -(planeNormal * m_clipPlanes[planeIndex].origin);
	m_clipPlanesEquations[4*planeIndex + 0] = planeNormal[0];
	m_clipPlanesEquations[4*planeIndex + 1] = planeNormal[1];
	m_clipPlanesEquations[4*planeIndex + 2] = planeNormal[2];
	m_clipPlanesEquations[4*planeIndex + 3] = d;

}

void ClippingShader::updateClippingPlaneVBO(int planeIndex)
{
	// Check if the given index is not out of range
	if ((planeIndex < 0) || (planeIndex > (getClipPlanesCount() - 1)))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::updateClippingPlaneVBO"
		<< " - Given plane index is out of range"
		<< CGoGNendl;
		return;
	}

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


	// Build again the VBO with the new points

	m_clipPlanesDrawers[planeIndex]->newList(GL_COMPILE);

	// Only display the grid if both x and y resolutions are not zero
	if ( (m_clipPlanesDisplayXRes != 0) && (m_clipPlanesDisplayYRes != 0) )
	{
		float t;
		Geom::Vec3f p1p2Interp;
		Geom::Vec3f p4p3Interp;
		Geom::Vec3f p3p4Interp; // Used for radial grid construction
		Geom::Vec3f p2p3Interp;
		Geom::Vec3f p1p4Interp;
		Geom::Vec3f p4p1Interp; // Used for radial grid construction

		// X lines
		for (size_t i = 0; i <= m_clipPlanesDisplayXRes; i++)
		{
			// Compute the linear interpolation parameter from the current value of 'i'
			t = (float)i / (float)m_clipPlanesDisplayXRes;

			// Straight grid construction
			if (m_clipPlanesDisplayType == STRAIGHT_GRID)
			{
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
			// Radial grid construction
			else if (m_clipPlanesDisplayType == RADIAL_GRID)
			{
				// Compute linear interpolations between points
				p1p2Interp = p1*t + p2*(1.0 - t);
				p3p4Interp = p3*t + p4*(1.0 - t);

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
						p3p4Interp[0],
						p3p4Interp[1],
						p3p4Interp[2]);
				m_clipPlanesDrawers[planeIndex]->end();
			}
		}

		// Y lines
		for (size_t i = 0; i <= m_clipPlanesDisplayYRes; i++)
		{
			// Compute the linear interpolation parameter from the current value of 'i'
			t = (float)i / (float)m_clipPlanesDisplayYRes;

			// Straight grid construction
			if (m_clipPlanesDisplayType == STRAIGHT_GRID)
			{
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
			// Radial grid construction
			else if (m_clipPlanesDisplayType == RADIAL_GRID)
			{
				// Compute linear interpolations between points
				p2p3Interp = p2*t + p3*(1.0 - t);
				p4p1Interp = p4*t + p1*(1.0 - t);

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
						p4p1Interp[0],
						p4p1Interp[1],
						p4p1Interp[2]);
				m_clipPlanesDrawers[planeIndex]->end();
			}
		}

	}

	m_clipPlanesDrawers[planeIndex]->endList();
}

void ClippingShader::updateAllClippingPlanesVBOs()
{
	for (int i = 0; i < getClipPlanesCount(); i++)
		updateClippingPlaneVBO(i);
}

} // namespace Utils

} // namespace CGoGN
