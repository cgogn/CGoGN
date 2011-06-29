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

ClippingShader::ClippingShader()
{
	// Initialize clipping planes variables
	m_unif_clipPlanes = 0;
	/*m_clipPlaneQuaternion[0] = 1.0;
	m_clipPlaneQuaternion[1] = 0.0;
	m_clipPlaneQuaternion[2] = 0.0;
	m_clipPlaneQuaternion[3] = 0.0;*/
	
	// Initialize color attenuation variables
	m_colorAttenuationFactor = 0.0;
	m_unif_colorAttenuationFactor = 0;

	// Plane Drawer
	float planeSize = 100.0;
	m_planeDrawer = new Drawer();
	m_planeDrawer->newList(GL_COMPILE);
	m_planeDrawer->begin(GL_QUADS);
	m_planeDrawer->color3f(0.7, 0.7, 0.2);
	m_planeDrawer->vertex3f(-planeSize/2.0, -planeSize/2.0, 0.0);
	m_planeDrawer->vertex3f(-planeSize/2.0, planeSize/2.0, 0.0);
	m_planeDrawer->vertex3f(planeSize/2.0, planeSize/2.0, 0.0);
	m_planeDrawer->vertex3f(planeSize/2.0, -planeSize/2.0, 0.0);
	m_planeDrawer->end();
	m_planeDrawer->endList();
}

ClippingShader::~ClippingShader()
{
	delete m_planeDrawer;
}

void ClippingShader::setClippingPlaneEquation(Geom::Vec4f clipPlane, int planeIndex)
{
	// Check if the given index is not out of range
	if ((planeIndex < 0) || ((4*planeIndex) > ((int)m_clipPlanesEquations.size() - 1)))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::setClippingPlaneEquation"
		<< " - Given plane index is out of range"
		<< CGoGNendl;
		return;
	}

	// Copy the given clipPlane
	int i;
	for (i = 0; i < 4; i++)
		m_clipPlanesEquations[4*planeIndex + i] = clipPlane[i];

	// Recalculate quaternion rotation
	/*float m[4][4];
	build_rotmatrix(m, m_clipPlaneQuaternion);
	Geom::Matrix44f rotMat;
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			rotMat(i, j) = m[i][j];
	Geom::Vec4f rotatedVec = rotMat * m_clipPlaneEquation;*/

	// Send again the whole planes equations array to shader
	sendClippingPlanesUniform();
}

Geom::Vec4f ClippingShader::getClippingPlaneEquation(int planeIndex)
{
	// Check if the given index is not out of range
	if ((planeIndex < 0) || (4*planeIndex > ((int)m_clipPlanesEquations.size() - 1)))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::getClippingPlaneEquation"
		<< " - Given plane index is out of range"
		<< CGoGNendl;
		return Geom::Vec4f(0.0, 0.0, 0.0, 0.0);
	}
	else
	{
		return Geom::Vec4f(
				m_clipPlanesEquations[4*planeIndex + 0],
				m_clipPlanesEquations[4*planeIndex + 1],
				m_clipPlanesEquations[4*planeIndex + 2],
				m_clipPlanesEquations[4*planeIndex + 3]);
	}
}

/*void ClippingShader::setClippingPlaneQuaternion(float quat[4])
{
	m_clipPlaneQuaternion[0] = quat[0];
	m_clipPlaneQuaternion[1] = quat[1];
	m_clipPlaneQuaternion[2] = quat[2];
	m_clipPlaneQuaternion[3] = quat[3];

	// Recalculate and send again the clipping plane equation
	setClippingPlaneEquation(m_clipPlaneEquation);
}

Geom::Vec4f ClippingShader::getClippingPlaneQuaternion()
{
	return Geom::Vec4f (m_clipPlaneQuaternion[0], m_clipPlaneQuaternion[1], m_clipPlaneQuaternion[2], m_clipPlaneQuaternion[3]);
}*/

void ClippingShader::setClippingColorAttenuationFactor(float colorAttenuationFactor)
{
	// Copy the given value
	m_colorAttenuationFactor = colorAttenuationFactor;

	// Send again the uniform to shader
	sendColorAttenuationFactorUniform();
}

float ClippingShader::getClippingColorAttenuationFactor()
{
	return m_colorAttenuationFactor;
}

void ClippingShader::setPlaneClipping(int planesCount)
{
	// Verify that the given clipping planes count is valid
	if (planesCount < 0)
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::setPlanesClipping"
		<< " - Given clipping planes count given is not positive !"
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
	"		// Else keep the positive distance to the nearest plane\n"
	"		else\n"
	"		{\n"
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
	int previousPlanesCount = getClippingPlanesCount();

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

	// Resize the planes equations uniform to the right size
	m_clipPlanesEquations.resize(4*(size_t)planesCount, 0.0);

	// Recompile shaders (automatically calls updateClippingUniforms)
	recompile();
}

void ClippingShader::updateClippingUniforms()
{
	// These uniforms only exist if the clipping planes count is > 0
	if (getClippingPlanesCount() <= 0)
		return;

	// Shader name
	std::string shaderName = m_nameVS + "/" + m_nameFS + "/" + m_nameGS;

	// Get uniforms locations
	m_unif_clipPlanes = glGetUniformLocation(program_handler(), "clip_ClipPlanes");
	if (m_unif_clipPlanes == -1)
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

void ClippingShader::sendClippingPlanesUniform()
{
	bind();
	glUniform4fv(m_unif_clipPlanes, m_clipPlanesEquations.size()/4, &m_clipPlanesEquations.front());
}

void ClippingShader::sendColorAttenuationFactorUniform()
{
	bind();
	glUniform1f(m_unif_colorAttenuationFactor, m_colorAttenuationFactor);
}

void ClippingShader::displayClippingPlane()
{
	m_planeDrawer->callList();
}

} // namespace Utils

} // namespace CGoGN

