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
	// Default values for plane clipping
	m_clipPlaneEquation = Geom::Vec4f(0.0, 0.0, 1.0, 0.0);
	m_unif_clipPlane = 0;
	m_clipPlaneQuaternion[0] = 1.0;
	m_clipPlaneQuaternion[1] = 0.0;
	m_clipPlaneQuaternion[2] = 0.0;
	m_clipPlaneQuaternion[3] = 0.0;
	
	// Default values for color attenuation
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

void ClippingShader::setClippingPlaneEquation(Geom::Vec4f clipPlane)
{
	m_clipPlaneEquation = clipPlane;

	// Recalculate quaternion rotation
	float m[4][4];
	build_rotmatrix(m, m_clipPlaneQuaternion);
	Geom::Matrix44f rotMat;
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			rotMat(i, j) = m[i][j];
	Geom::Vec4f rotatedVec = rotMat * m_clipPlaneEquation;

	// Send the resulting plane equation to shader
	bind();
	glUniform4fv(m_unif_clipPlane, 1, rotatedVec.data());
}

Geom::Vec4f ClippingShader::getClippingPlaneEquation()
{
	return m_clipPlaneEquation;
}

void ClippingShader::setClippingPlaneQuaternion(float quat[4])
{
	m_clipPlaneQuaternion[0] = quat[0];
	m_clipPlaneQuaternion[1] = quat[1];
	m_clipPlaneQuaternion[2] = quat[2];
	m_clipPlaneQuaternion[3] = quat[3];

	// Recalculate and resend the clipping plane equation
	setClippingPlaneEquation(m_clipPlaneEquation);
}

Geom::Vec4f ClippingShader::getClippingPlaneQuaternion()
{
	return Geom::Vec4f (m_clipPlaneQuaternion[0], m_clipPlaneQuaternion[1], m_clipPlaneQuaternion[2], m_clipPlaneQuaternion[3]);
}

void ClippingShader::setClippingColorAttenuationFactor(float colorAttenuationFactor)
{
	m_colorAttenuationFactor = colorAttenuationFactor;
	bind();
	glUniform1f(m_unif_colorAttenuationFactor, m_colorAttenuationFactor);
}

float ClippingShader::getClippingColorAttenuationFactor()
{
	return m_colorAttenuationFactor;
}

void ClippingShader::addPlaneClippingToShaderSource()
{
	// Shader name
	std::string shaderName = m_nameVS + "/" + m_nameFS + "/" + m_nameGS;

	// Verify that the shader has been well created
	if (!isCreated())
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::addPlaneClippingToShaderSource"
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
		<< "ClippingShader::addPlaneClippingToShaderSource"
		<< " - Could not process shader "
		<< shaderName
		<< " source code : unable to add clipping to a shader which uses a geometry shader"
		<< CGoGNendl;
		return;
	}

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
	"uniform vec4 clip_ClipPlane;\n"
	"\n"
	"uniform float clip_ColorAttenuationFactor;\n"
	"\n"
	"VARYING_FRAG vec3 clip_NonTransformedPos;\n"
	"\n";

	std::string FS_mainBegin_insertion =
	"\n"
	"	// Signed distance between the point and the plane\n"
	"	float clip_DistanceToPlane = dot(clip_NonTransformedPos, clip_ClipPlane.xyz) + clip_ClipPlane.w;\n"
	"	float clip_NPlane = length(clip_ClipPlane.xyz);\n"
	"	if (clip_NPlane != 0.0)\n"
	"		clip_DistanceToPlane /= clip_NPlane;\n"
	"\n"
	"	// Keep the fragment only if it is 'above' the plane\n"
	"	if (clip_DistanceToPlane < 0.0)\n"
	"		discard;\n"
	"	else\n"
	"	{\n";

	std::string FS_mainEnd_insertion =
	"\n"
	"	}\n"
	"\n"
	"	// Attenuate the final fragment color depending on its distance to the plane\n"
	"	if (clip_DistanceToPlane > 0.0)\n"
	"	gl_FragColor.rgb /= (1.0 + clip_DistanceToPlane*clip_ColorAttenuationFactor);\n";

	
	// Use a shader mutator
	ShaderMutator SM(shaderName, getVertexShaderSrc(), getFragmentShaderSrc(), "");
	
	// First check if the vertex shader contains the VertexPosition attribute
	if (!SM.VS_containsVariableDeclaration("VertexPosition"))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::addPlaneClippingToShaderSource"
		<< " - Could not process shader "
		<< m_nameVS
		<< " source code : no VertexPosition attribute found"
		<< CGoGNendl;
		return;
	}
	
	// Modify vertex shader source code
	SM.VS_insertCodeBeforeMainFunction(VS_head_insertion);
	SM.VS_insertCodeAtMainFunctionBeginning(VS_mainBegin_insertion);
	
	// Modify fragment shader source code
	SM.FS_insertCodeBeforeMainFunction(FS_head_insertion);
	SM.FS_insertCodeAtMainFunctionEnd(FS_mainEnd_insertion);
	SM.FS_insertCodeAtMainFunctionBeginning(FS_mainBegin_insertion);

	// Reload both shaders
	reloadVertexShaderFromMemory(SM.getModifiedVertexShaderSrc().c_str());
	reloadFragmentShaderFromMemory(SM.getModifiedFragmentShaderSrc().c_str());

	// Recompile shaders (automatically calls updateClippingUniforms
	recompile();
}

void ClippingShader::updateClippingUniforms()
{
	// Shader name
	std::string shaderName = m_nameVS + "/" + m_nameFS + "/" + m_nameGS;

	// Get uniforms locations
	m_unif_clipPlane = glGetUniformLocation(program_handler(), "clip_ClipPlane");
	if (m_unif_clipPlane == -1)
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::addPlaneClippingToShaderSource"
		<< " - uniform 'clip_ClipPlane' not found in shader "
		<< shaderName
		<< CGoGNendl;
	}
	m_unif_colorAttenuationFactor = glGetUniformLocation(program_handler(), "clip_ColorAttenuationFactor");
	if (m_unif_colorAttenuationFactor == -1)
	{
		CGoGNerr
		<< "ERROR - "
		<< "ClippingShader::addPlaneClippingToShaderSource"
		<< " - uniform 'clip_ColorAttenuationFactor' not found in shader "
		<< shaderName
		<< CGoGNendl;
	}
	
	// Set uniforms values
	setClippingPlaneEquation(m_clipPlaneEquation);
	setClippingPlaneQuaternion(m_clipPlaneQuaternion);
	setClippingColorAttenuationFactor(m_colorAttenuationFactor);
}

void ClippingShader::displayClippingPlane()
{
	m_planeDrawer->callList();
}

} // namespace Utils

} // namespace CGoGN

