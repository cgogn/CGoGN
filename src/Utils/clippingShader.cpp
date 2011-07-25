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


ClippingShader::ClippingShader():
		// Initialize clipping shapes variables
		m_unif_clipPlanesEquations (0),
		m_unif_clipSpheresCentersAndRadiuses (0),

		// Initialize default global clipping variables
		m_hasClippingCodeBeenInserted (false),
		m_clipColorAttenuationFactor (1.0),
		m_unif_clipColorAttenuationFactor (0),
		m_clipMode (CLIPPING_MODE_AND)
{

}


/***********************************************
 *
 * 		Plane Clipping
 *
 ***********************************************/


void ClippingShader::setClipPlanesCount(int planesCount)
{
	if (planesCount != getClipPlanesCount())
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
		updateClipPlaneUniformsArray(planeIndex);

		// Send again the whole planes equations array to shader
		sendClipPlanesEquationsUniform();
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
		updateClipPlaneUniformsArray(planeIndex);

		// Send again the whole planes equations array to shader
		sendClipPlanesEquationsUniform();
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
		updateClipPlaneUniformsArray(planeIndex);

		// Send again the whole planes equations array to shader
		sendClipPlanesEquationsUniform();
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
		updateClipPlaneUniformsArray(planeIndex);

		// Send again the whole planes equations array to shader
		sendClipPlanesEquationsUniform();
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

void ClippingShader::updateClipPlaneUniformsArray(int planeIndex)
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
 * 		Sphere Clipping
 *
 ***********************************************/


void ClippingShader::setClipSpheresCount(int spheresCount)
{
	if (spheresCount != getClipSpheresCount())
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
		updateClipSphereUniformsArray(sphereIndex);

		// Send again the whole spheres centers and radiuses array to shader
		sendClipSpheresCentersAndRadiusesUniform();
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
		updateClipSphereUniformsArray(sphereIndex);

		// Send again the whole spheres centers and radiuses array to shader
		sendClipSpheresCentersAndRadiusesUniform();
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
		updateClipSphereUniformsArray(sphereIndex);

		// Send again the whole spheres centers and radiuses array to shader
		sendClipSpheresCentersAndRadiusesUniform();
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

void ClippingShader::updateClipSphereUniformsArray(int sphereIndex)
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
	"// In following clipping modes, pixels may be deleted :\n"
	"//  - OR : only after being matched with every object\n"
	"//  - AND : on the fly as soon as one object does not match\n"
	"#define CLIPPING_MODE_AND 0\n"
	"#define CLIPPING_MODE_OR 1\n"
	"#define CLIPPING_MODE 0\n"
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
	"		// OR clipping mode needs a boolean to know if the pixel must be clipped or not\n"
	"		// By default set to true : one clipping object matched sets it to false\n"
	"		#if (CLIPPING_MODE == CLIPPING_MODE_OR)\n"
	"			bool discardPixel = true;\n"
	"		#endif\n"
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
	"				if (distanceToPlane > 0.0)\n"
	"				{\n"
	"					// AND clipping mode discards at first unmatched clipping object\n"
	"					#if (CLIPPING_MODE == CLIPPING_MODE_AND)\n"
	"						discard;\n"
	"					#endif\n"
	"				}\n"
	"				else\n"
	"				{\n"
	"					// In OR clipping mode, one match = no pixel clipping\n"
	"					#if (CLIPPING_MODE == CLIPPING_MODE_OR)\n"
	"						discardPixel = false;\n"
	"					#endif\n"
	"\n"
	"					// Keep the distance to the nearest plane\n"
	"					if (minDistanceToClipping < 0.0)\n"
	"						minDistanceToClipping = abs(distanceToPlane);\n"
	"					else\n"
	"						minDistanceToClipping = min(minDistanceToClipping, abs(distanceToPlane));\n"
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
	"				// Signed distance between the point and the sphere\n"
	"				float distanceToSphere = length(clip_nonTransformedPos - currClipSphereCenter);\n"
	"				distanceToSphere -= abs(currClipSphereRadius);\n"
	"\n"
	"				// If the sphere radius is negative, this inverses the clipping effect\n"
	"				distanceToSphere *= sign(currClipSphereRadius);\n"
	"\n"
	"				// Keep the fragment only if it is inside the sphere\n"
	"				if (distanceToSphere > 0.0)\n"
	"				{\n"
	"					// AND clipping mode discards at first unmatched clipping object\n"
	"					#if (CLIPPING_MODE == CLIPPING_MODE_AND)\n"
	"						discard;\n"
	"					#endif\n"
	"				}\n"
	"				else\n"
	"				{\n"
	"					// In OR clipping mode, one match = no pixel clipping\n"
	"					#if (CLIPPING_MODE == CLIPPING_MODE_OR)\n"
	"						discardPixel = false;\n"
	"					#endif\n"
	"\n"
	"					// Keep the distance to the nearest sphere\n"
	"					if (minDistanceToClipping < 0.0)\n"
	"						minDistanceToClipping = abs(distanceToSphere);\n"
	"					else\n"
	"						minDistanceToClipping = min(minDistanceToClipping, abs(distanceToSphere));\n"
	"				}\n"
	"			}\n"
	"\n"
	"		#endif\n"
	"\n"
	"		// In OR clipping mode, the pixel may be deleted only after being matched with every object\n"
	"		#if (CLIPPING_MODE == CLIPPING_MODE_OR)\n"
	"			if (discardPixel)\n"
	"				discard;\n"
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
	if (colorAttenuationFactor != m_clipColorAttenuationFactor)
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

void ClippingShader::setClipMode(clippingMode clipMode)
{
	if (clipMode != m_clipMode)
	{
		// Check if the clipping code has been inserted into shader
		if (errorRaiseClippingCodeNotInserted(!m_hasClippingCodeBeenInserted, "ClippingShader::setClipPlanesCount"))
			return;

		// Shader name string
		std::string shaderName = m_nameVS + "/" + m_nameFS + "/" + m_nameGS;

		// Copy the given value
		m_clipMode = clipMode;

		// Use a shader mutator
		ShaderMutator SM(shaderName, getVertexShaderSrc(), getFragmentShaderSrc());

		// Change clipping mode constant
		int newConstantValue;
		switch (clipMode)
		{
			case CLIPPING_MODE_AND :
				newConstantValue = 0;
				break;

			case CLIPPING_MODE_OR :
				newConstantValue = 1;
				break;

			default :
				newConstantValue = 0;
				break;
		}
		SM.changeIntConstantValue(ShaderMutator::FRAGMENT_SHADER, "CLIPPING_MODE", newConstantValue);

		// Reload modified shader
		reloadFragmentShaderFromMemory(SM.getModifiedFragmentShaderSrc().c_str());

		// Recompile shaders (automatically calls updateClippingUniforms)
		recompile();
	}
}

ClippingShader::clippingMode ClippingShader::getClipMode()
{
	return m_clipMode;
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
