/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed  in the hope that it will be useful, but WITHOUT *
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

#include "clipping.h"
#include "Utils/vbo.h"
#include "Utils/static_assert.h"

/*******************************************************************************
 *														MISCELLANOUS
 *******************************************************************************/

void computeBasisFromVector(const Geom::Vec3f& inVec1, Geom::Vec3f& vec2, Geom::Vec3f& vec3)
{
	const float epsilon = 0.000001f;

	// Check if the given vector length is acceptable
	if (inVec1.norm() < epsilon)
		return;

	// First take a non colinear second vector to cross product with vec1
	// (by default (0.0, 0.0, 1.0)
	Geom::Vec3f tempVec (0.0, 0.0, 1.0);

	// Construct second vector, check other vectors non colinearity at the same time
	vec2 = inVec1 ^ tempVec;
	float sinAngle = vec2.norm() / (inVec1.norm()*tempVec.norm());
	if (sinAngle < epsilon) // f:x->sin(x) ~ f:x->x when x ~ 0
	{
		tempVec = Geom::Vec3f (1.0, 0.0, 0.0);
		vec2 = inVec1 ^ tempVec;
	}

	// Normalize second vector
	vec2.normalize();

	// Get third vector
	vec3 = inVec1 ^ vec2;

	// Normalize third vector
	vec3.normalize();

}

void Clipping::updatePickables()
{
	for (size_t i = 0; i < m_pickablePlanes.size(); i++)
	{
		unsigned int id = m_pickablePlanes[i]->id();
		Geom::Vec3f pos = m_shader->getClipPlaneParamsOrigin(id);
		Geom::Vec3f vec1, vec2, vec3;
		vec1 = m_shader->getClipPlaneParamsNormal(id);
		computeBasisFromVector(vec1, vec2, vec3);
		glm::mat4& transfoMat = m_pickablePlanes[i]->transfo();
		for (int j = 0; j < 3; j++)
		{
			transfoMat[0][j] = vec2[j];
			transfoMat[1][j] = vec3[j];
			transfoMat[2][j] = vec1[j];
			transfoMat[3][j] = pos[j];
		}
		transfoMat[0][3] = 0.0;
		transfoMat[1][3] = 0.0;
		transfoMat[2][3] = 0.0;
		transfoMat[3][3] = 1.0;
	}
	for (size_t i = 0; i < m_pickableSpheres.size(); i++)
	{
		unsigned int id = m_pickableSpheres[i]->id();
		Geom::Vec3f pos = m_shader->getClipSphereParamsCenter(id);
		float radius = m_shader->getClipSphereParamsRadius(id);
		Geom::Vec3f vec1 (radius, 0.0, 0.0);
		Geom::Vec3f vec2 (0.0, radius, 0.0);
		Geom::Vec3f vec3 (0.0, 0.0, radius);
		glm::mat4& transfoMat = m_pickableSpheres[i]->transfo();
		for (int j = 0; j < 3; j++)
		{
			transfoMat[0][j] = vec2[j];
			transfoMat[1][j] = vec3[j];
			transfoMat[2][j] = vec1[j];
			transfoMat[3][j] = pos[j];
		}
		transfoMat[0][3] = 0.0;
		transfoMat[1][3] = 0.0;
		transfoMat[2][3] = 0.0;
		transfoMat[3][3] = 1.0;
	}
}

/*******************************************************************************
 *														SLOTS
 *******************************************************************************/

void Clipping::slot_drawTopo(bool b)
{
	m_drawTopo = b;
	updateGL();
}

void Clipping::slot_drawVertices(bool b)
{
	m_drawVertices = b;
	updateGL();
}

void Clipping::slot_drawLines(bool b)
{
	m_drawLines = b;
	updateGL();
}

void Clipping::slot_drawFaces(bool b)
{
	m_drawFaces = b;
	updateGL();
}

void Clipping::slot_explodTopoPhi1(double c)
{
	m_coeffTopoExplod[0] = (float)c;
	m_render_topo->updateData(myMap, position, m_coeffTopoExplod[0], m_coeffTopoExplod[1], m_coeffTopoExplod[2]);
	updateGL();
}

void Clipping::slot_explodTopoPhi2(double c)
{
	m_coeffTopoExplod[1] = (float)c;
	m_render_topo->updateData(myMap, position, m_coeffTopoExplod[0], m_coeffTopoExplod[1], m_coeffTopoExplod[2]);
	updateGL();
}

void Clipping::slot_explodTopoPhi3(double c)
{
	m_coeffTopoExplod[2] = (float)c;
	m_render_topo->updateData(myMap, position, m_coeffTopoExplod[0], m_coeffTopoExplod[1], m_coeffTopoExplod[2]);
	updateGL();
}

void Clipping::slot_pushButton_addPlane()
{
	// Check if no animation is active
	if (m_lastAnimatedClippingPreset != NULL)
		return;

	// Create clipping and pickable objects
	unsigned int newPlaneId = m_shader->addClipPlane();
	Utils::Pickable* pickable = new Utils::Pickable(m_planeDrawable, newPlaneId);
	m_pickablePlanes.push_back(pickable);

	// Set params
	m_shader->setClipPlaneParamsOrigin(newPlaneId, m_bb.center());
	pickable->translate(m_bb.center());
	pickable->scale(Geom::Vec3f(m_bb.maxSize()*0.5));

	// Update shader sources edits
	dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
	dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

	updateGLMatrices();
}

void Clipping::slot_spinBox_GridResolution(int i)
{
	m_planeDrawable->updatePrecisionDrawing(i);
	updateGL();
}

void Clipping::slot_pushButton_changePlanesColor()
{
	QColor NewColor = QColorDialog::getColor();

	int r, g, b;
	NewColor.getRgb(&r, &g, &b);

	m_planeDrawable->setColor(Geom::Vec4f((float)r/255.0, (float)g/255.0, (float)b/255.0, 1.0));
}

void Clipping::slot_pushButton_addSphere()
{
	// Check if no animation is active
	if (m_lastAnimatedClippingPreset != NULL)
		return;

	// Create clipping and pickable objects
	unsigned int newSphereId = m_shader->addClipSphere();
	Utils::Pickable* pickable = new Utils::Pickable(m_sphereDrawable, newSphereId);
	m_pickableSpheres.push_back(pickable);

	// Set params
	m_shader->setClipSphereParamsCenter(newSphereId, m_bb.center());
	m_shader->setClipSphereParamsRadius(newSphereId, (m_bb.maxSize())*0.5);
	pickable->translate(m_bb.center());
	pickable->scale(Geom::Vec3f(m_bb.maxSize()*0.5));

	// Update shader sources edits
	dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
	dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

	updateGLMatrices();
}

void Clipping::slot_spinBox_SphereResolution(int i)
{
	m_sphereDrawable->updatePrecisionDrawing(i);
	updateGL();
}

void Clipping::slot_pushButton_changeSpheresColor()
{
	QColor NewColor = QColorDialog::getColor();


	int r, g, b;
	NewColor.getRgb(&r, &g, &b);

	m_sphereDrawable->setColor(Geom::Vec4f((float)r/255.0, (float)g/255.0, (float)b/255.0, 1.0));
}

void Clipping::slot_doubleSpinBox_ColorAttenuationFactor(double c)
{
	m_shader->setClipColorAttenuationFactorAbsolute((float)c);
	updateGL();
}

void Clipping::slot_radioButton_ClippingMode(bool b)
{
	if (b)
		m_shader->setClipMode(Utils::ClippingShader::CLIPPING_MODE_AND);
	else
		m_shader->setClipMode(Utils::ClippingShader::CLIPPING_MODE_OR);

	dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
	dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

	updateGLMatrices();
}

void Clipping::slot_radioButton_ColorAttenuationMode(bool b)
{
	if (b)
		m_shader->setClipColorAttenuationMode(Utils::ClippingShader::COLOR_ATTENUATION_MODE_LINEAR);
	else
		m_shader->setClipColorAttenuationMode(Utils::ClippingShader::COLOR_ATTENUATION_MODE_QUADRATIC);

	dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
	dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

	updateGLMatrices();
}

void Clipping::slot_pushButton_deleteSelectedObject()
{
	// Check if no animation is active
	if (m_lastAnimatedClippingPreset != NULL)
		return;

	if (m_lastPickedObject)
	{
		// Delete clipping object
		if (m_lastPickedObject->checkType<Utils::Grid>())
			m_shader->deleteClipPlane(m_lastPickedObject->id());
		else if (m_lastPickedObject->checkType<Utils::Sphere>())
			m_shader->deleteClipSphere(m_lastPickedObject->id());

		// Delete pickable
		if (m_lastPickedObject->checkType<Utils::Grid>())
		{
			for (size_t i = 0; i < m_pickablePlanes.size(); i++)
			{
				if (m_pickablePlanes[i] == m_lastPickedObject)
				{
					delete m_pickablePlanes[i];
					m_pickablePlanes.erase(m_pickablePlanes.begin() + i);
				}
			}
		}
		else if (m_lastPickedObject->checkType<Utils::Sphere>())
		{
			for (size_t i = 0; i < m_pickableSpheres.size(); i++)
			{
				if (m_pickableSpheres[i] == m_lastPickedObject)
				{
					delete m_pickableSpheres[i];
					m_pickableSpheres.erase(m_pickableSpheres.begin() + i);
				}
			}
		}
		m_lastPickedObject = NULL;
		m_frameManipulator->highlight(m_frameManipulatorPickedAxis);
		m_frameManipulatorPickedAxis = 0;

		// Update shader sources edits
		dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
		dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

		updateGLMatrices();

	}
}

void Clipping::slot_pushButton_applyStaticClippingPreset()
{
	// Check if no animation is active
	if (m_lastAnimatedClippingPreset != NULL)
		return;

	// Create and apply preset
	Utils::ClippingPreset *preset = NULL;
	switch (dock.comboBox_StaticClippingPresets->currentIndex())
	{
		case 0 : // Dual planes
	 	{
		 	using namespace CGoGN::Utils::QT;

		 	float centerX = m_bb.center()[0];
		 	float centerY = m_bb.center()[1];
		 	float centerZ = m_bb.center()[2];
		 	float size = m_bb.maxSize()*0.75f;
		 	int axis = 0;
		 	bool facing = false;
		 	if (inputValues(VarFloat(centerX - 100.0f, centerX + 100.0f, centerX, "Center X",
		 					VarFloat(centerY - 100.0f, centerY + 100.0f, centerY, "Center Y",
		 					VarFloat(centerZ - 100.0f, centerZ + 100.0f, centerZ, "Center Z",
		 					VarFloat(size - 100.0f, size + 100.0f, size, "Size",
		 					VarSlider(0, 2, axis, "Axis",
		 					VarBool(facing, "Facing"
		 				)))))), "Preset Setup"))
		 		preset = Utils::ClippingPreset::CreateDualPlanesPreset(Geom::Vec3f(centerX, centerY, centerZ), size, axis, facing);
	 	}
		break;

		case 1 : // Cube
	 	{
		 	using namespace CGoGN::Utils::QT;

		 	float centerX = m_bb.center()[0];
		 	float centerY = m_bb.center()[1];
		 	float centerZ = m_bb.center()[2];
		 	float size = m_bb.maxSize()*0.75f;
		 	bool facing = false;
		 	if (inputValues(VarFloat(centerX - 100.0f, centerX + 100.0f, centerX, "Center X",
		 					VarFloat(centerY - 100.0f, centerY + 100.0f, centerY, "Center Y",
		 					VarFloat(centerZ - 100.0f, centerZ + 100.0f, centerZ, "Center Z",
		 					VarFloat(size - 100.0f, size + 100.0f, size, "Size",
		 					VarBool(facing, "Facing"
		 				))))), "Preset Setup"))
		 		preset = Utils::ClippingPreset::CreateCubePreset(Geom::Vec3f(centerX, centerY, centerZ), size, facing);
	 	}
		break;

		case 2 : // Tube
	 	{
		 	using namespace CGoGN::Utils::QT;

		 	float centerX = m_bb.center()[0];
		 	float centerY = m_bb.center()[1];
		 	float centerZ = m_bb.center()[2];
		 	float size = m_bb.maxSize()*0.75f;
		 	int axis = 0;
		 	int precision = 1;
		 	bool facing = false;
		 	if (inputValues(VarFloat(centerX - 100.0f, centerX + 100.0f, centerX, "Center X",
		 					VarFloat(centerY - 100.0f, centerY + 100.0f, centerY, "Center Y",
		 					VarFloat(centerZ - 100.0f, centerZ + 100.0f, centerZ, "Center Z",
		 					VarFloat(size - 100.0f, size + 100.0f, size, "Size",
		 					VarSlider(0, 2, axis, "Axis",
		 					VarSlider(1, 30, precision, "Precision",
		 					VarBool(facing, "Facing"
		 				))))))), "Preset Setup"))
		 		preset = Utils::ClippingPreset::CreateTubePreset(Geom::Vec3f(centerX, centerY, centerZ), size, axis, precision, facing);
	 	}
		break;

		case 3 : // Molecule
	 	{
		 	using namespace CGoGN::Utils::QT;

		 	float centerX = m_bb.center()[0];
		 	float centerY = m_bb.center()[1];
		 	float centerZ = m_bb.center()[2];
		 	float size = m_bb.maxSize()*0.75f;
		 	float atomsRadiuses = m_bb.maxSize()*0.25f;
		 	bool orClipping = true;
		 	if (inputValues(VarFloat(centerX - 100.0f, centerX + 100.0f, centerX, "Center X",
		 					VarFloat(centerY - 100.0f, centerY + 100.0f, centerY, "Center Y",
		 					VarFloat(centerZ - 100.0f, centerZ + 100.0f, centerZ, "Center Z",
		 					VarFloat(size - 100.0f, size + 100.0f, size, "Size",
		 					VarFloat(atomsRadiuses - 100.0f, atomsRadiuses + 100.0f, atomsRadiuses, "Atoms radiuses",
		 					VarBool(orClipping, "OR clipping mode"
		 				)))))), "Preset Setup"))
		 		preset = Utils::ClippingPreset::CreateMoleculePreset(Geom::Vec3f(centerX, centerY, centerZ), size, atomsRadiuses, orClipping);
	 	}
		break;
	}

	if (preset == NULL)
		return;

	std::vector<unsigned int> planesIds;
	std::vector<unsigned int> spheresIds;
	preset->apply(m_shader, &planesIds, &spheresIds);
	delete preset;

	// Cleanup of pickables before adding new ones
	m_lastPickedObject = NULL;
	for (size_t i = 0; i < m_pickablePlanes.size(); i++)
		delete m_pickablePlanes[i];
	m_pickablePlanes.resize(0);
	for (size_t i = 0; i < m_pickableSpheres.size(); i++)
		delete m_pickableSpheres[i];
	m_pickableSpheres.resize(0);

	// Add new pickable objects
	for (size_t i = 0; i < planesIds.size(); i++)
	{
		Utils::Pickable* pickable = new Utils::Pickable(m_planeDrawable, planesIds[i]);
		m_pickablePlanes.push_back(pickable);
	}
	for (size_t i = 0; i < spheresIds.size(); i++)
	{
		Utils::Pickable* pickable = new Utils::Pickable(m_sphereDrawable, spheresIds[i]);
		m_pickableSpheres.push_back(pickable);
	}
	updatePickables();

	// Update shader sources edits
	dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
	dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

	// Update clipping parameters in interface
	Utils::ClippingShader::clippingMode clipMode = m_shader->getClipMode();
	if (clipMode == Utils::ClippingShader::CLIPPING_MODE_AND)
		dock.radioButton_ClippingModeAnd->setChecked(true);
	else if (clipMode == Utils::ClippingShader::CLIPPING_MODE_OR)
		dock.radioButton_ClippingModeOr->setChecked(true);


	updateGLMatrices();
}

void Clipping::slot_pushButton_applyAnimatedClippingPreset()
{
	// Check if no animation is active
	if (m_lastAnimatedClippingPreset != NULL)
		return;

	// Create and apply preset
	Utils::ClippingPresetAnimated *animatedPreset = NULL;
	switch (dock.comboBox_AnimatedClippingPresets->currentIndex())
	{
		case 0 : // Moving Dual planes
	 	{
		 	Utils::ClippingPresetAnimatedDualPlanes *preset = NULL;
		 	using namespace CGoGN::Utils::QT;

		 	float centerStartX = m_bb.center()[0] - m_bb.size(0)*0.75f;
		 	float centerStartY = m_bb.center()[1];
		 	float centerStartZ = m_bb.center()[2];
		 	float centerEndX = m_bb.center()[0] + m_bb.size(0)*0.75f;
		 	float centerEndY = m_bb.center()[1];
		 	float centerEndZ = m_bb.center()[2];
		 	float size = m_bb.maxSize()*0.1f;
		 	int axis = 0;
		 	bool facing = false;
		 	bool zigzag = false;
		 	if (inputValues(VarFloat(centerStartX - 100.0f, centerStartX + 100.0f, centerStartX, "Center Start X",
		 					VarFloat(centerStartY - 100.0f, centerStartY + 100.0f, centerStartY, "Center Start Y",
		 					VarFloat(centerStartZ - 100.0f, centerStartZ + 100.0f, centerStartZ, "Center Start Z",
		 					VarFloat(centerEndX - 100.0f, centerEndX + 100.0f, centerEndX, "Center End X",
		 					VarFloat(centerEndY - 100.0f, centerEndY + 100.0f, centerEndY, "Center End Y",
		 					VarFloat(centerEndZ - 100.0f, centerEndZ + 100.0f, centerEndZ, "Center End Z",
		 					VarFloat(size - 100.0f, size + 100.0f, size, "Size",
		 					VarSlider(0, 2, axis, "Axis",
		 					VarBool(facing, "Facing",
		 					VarBool(zigzag, "Zigzag"
		 				)))))))))), "Preset Setup"))
		 		preset = new Utils::ClippingPresetAnimatedDualPlanes(
		 				Geom::Vec3f(centerStartX, centerStartY, centerStartZ), Geom::Vec3f(centerEndX, centerEndY, centerEndZ),
		 				size, axis, facing, zigzag);

		 	animatedPreset = preset;
	 	}
		break;

		case 1 : // Rotating Plane
	 	{
		 	Utils::ClippingPresetAnimatedRotatingPlane *preset = NULL;
		 	using namespace CGoGN::Utils::QT;

		 	float centerX = m_bb.center()[0];
		 	float centerY = m_bb.center()[1];
		 	float centerZ = m_bb.center()[2];
		 	int axis = 0;
		 	if (inputValues(VarFloat(centerX - 100.0f, centerX + 100.0f, centerX, "Center X",
		 					VarFloat(centerY - 100.0f, centerY + 100.0f, centerY, "Center Y",
		 					VarFloat(centerZ - 100.0f, centerZ + 100.0f, centerZ, "Center Z",
		 					VarSlider(0, 2, axis, "Axis"
		 				)))), "Preset Setup"))
		 		preset = new Utils::ClippingPresetAnimatedRotatingPlane(
		 				Geom::Vec3f(centerX, centerY, centerZ), axis);

		 	animatedPreset = preset;
	 	}
		break;

		case 2 : // Scaled Sphere
	 	{
		 	Utils::ClippingPresetAnimatedScaledSphere *preset = NULL;
		 	using namespace CGoGN::Utils::QT;

		 	float centerX = m_bb.center()[0];
		 	float centerY = m_bb.center()[1];
		 	float centerZ = m_bb.center()[2];
		 	float radiusStart = m_bb.maxSize()*0.1f;
		 	float radiusEnd = m_bb.maxSize()*1.0f;
		 	bool zigzag = false;
		 	if (inputValues(VarFloat(centerX - 100.0f, centerX + 100.0f, centerX, "Center X",
		 					VarFloat(centerY - 100.0f, centerY + 100.0f, centerY, "Center Y",
		 					VarFloat(centerZ - 100.0f, centerZ + 100.0f, centerZ, "Center Z",
		 					VarFloat(-radiusEnd*4.0f, radiusEnd*4.0f, radiusStart, "Radius Start",
		 					VarFloat(-radiusEnd*4.0f, radiusEnd*4.0f, radiusEnd, "Radius End",
		 					VarBool(zigzag, "Zigzag"
		 				)))))), "Preset Setup"))
		 		preset = new Utils::ClippingPresetAnimatedScaledSphere(
		 				Geom::Vec3f(centerX, centerY, centerZ), radiusStart, radiusEnd, zigzag);

		 	animatedPreset = preset;
	 	}
		break;

		case 3 : // Spheres Cube Collision
	 	{
		 	Utils::ClippingPresetAnimatedSpheresCubeCollision *preset = NULL;
		 	using namespace CGoGN::Utils::QT;

		 	float centerX = m_bb.center()[0];
		 	float centerY = m_bb.center()[1];
		 	float centerZ = m_bb.center()[2];
		 	float size = m_bb.maxSize()*1.0f;
		 	int spheresCount = 5;
		 	float radius = m_bb.maxSize()*0.2f;
		 	if (inputValues(VarFloat(centerX - 100.0f, centerX + 100.0f, centerX, "Center X",
		 					VarFloat(centerY - 100.0f, centerY + 100.0f, centerY, "Center Y",
		 					VarFloat(centerZ - 100.0f, centerZ + 100.0f, centerZ, "Center Z",
		 					VarFloat(-size*4.0f, size*4.0f, size, "Cube Size",
		 					VarSlider(1, 20, spheresCount, "Spheres Count",
		 					VarFloat(-radius*4.0f, radius*4.0f, radius, "Spheres Radius"
		 				)))))), "Preset Setup"))
		 	{
		 		preset = new Utils::ClippingPresetAnimatedSpheresCubeCollision(
		 				Geom::Vec3f(centerX, centerY, centerZ), size, spheresCount, radius);
		 	}

		 	animatedPreset = preset;
	 	}
		break;
	}

	if (animatedPreset == NULL)
		return;

	std::vector<unsigned int> planesIds;
	std::vector<unsigned int> spheresIds;
	animatedPreset->apply(m_shader, &planesIds, &spheresIds);

	// Cleanup of pickables before adding new ones
	m_lastPickedObject = NULL;
	for (size_t i = 0; i < m_pickablePlanes.size(); i++)
		delete m_pickablePlanes[i];
	m_pickablePlanes.resize(0);
	for (size_t i = 0; i < m_pickableSpheres.size(); i++)
		delete m_pickableSpheres[i];
	m_pickableSpheres.resize(0);

	// Add new pickable objects
	for (size_t i = 0; i < planesIds.size(); i++)
	{
		Utils::Pickable* pickable = new Utils::Pickable(m_planeDrawable, planesIds[i]);
		m_pickablePlanes.push_back(pickable);
	}
	for (size_t i = 0; i < spheresIds.size(); i++)
	{
		Utils::Pickable* pickable = new Utils::Pickable(m_sphereDrawable, spheresIds[i]);
		m_pickableSpheres.push_back(pickable);
	}
	updatePickables();

	// Update shader sources edits
	dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
	dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

	// Update clipping parameters in interface
	Utils::ClippingShader::clippingMode clipMode = m_shader->getClipMode();
	if (clipMode == Utils::ClippingShader::CLIPPING_MODE_AND)
		dock.radioButton_ClippingModeAnd->setChecked(true);
	else if (clipMode == Utils::ClippingShader::CLIPPING_MODE_OR)
		dock.radioButton_ClippingModeOr->setChecked(true);

	// Set on animated mode
	m_lastAnimatedClippingPreset = animatedPreset;
	m_timer->start(1000.0f/60.0f);
	m_lastAnimatedClippingPreset->setAnimationSpeedFactor((float)dock.doubleSpinBox_AnimatedClippingPresetSpeed->value());
	slot_setAnimationState(true);


	updateGLMatrices();
}

void Clipping::slot_pushButton_StopAnimation()
{
	if (m_lastAnimatedClippingPreset != NULL)
	{
		delete m_lastAnimatedClippingPreset;
		m_lastAnimatedClippingPreset = NULL;
	}
	slot_setAnimationState(false);
}

void Clipping::slot_doubleSpinBox_AnimatedClippingPresetSpeed(double c)
{
	if (m_lastAnimatedClippingPreset != NULL)
		m_lastAnimatedClippingPreset->setAnimationSpeedFactor((float)c);
}

void Clipping::slot_setAnimationState(bool state)
{
	dock.pushButton_addPlane->setEnabled(!state);
	dock.pushButton_addSphere->setEnabled(!state);
	dock.PushButton_ApplyStaticClippingPreset->setEnabled(!state);
	dock.PushButton_ApplyAnimatedClippingPreset->setEnabled(!state);
}

void Clipping::slot_animationTimer()
{
	if (m_lastAnimatedClippingPreset == NULL)
	{
		m_timer->stop();
		return;
	}

	m_lastAnimatedClippingPreset->step(1);
	updatePickables();

	updateGL();
}

void Clipping::button_compile()
{
	QString st1 = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->vertexEdit->toPlainText();
	QString st2 = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->fragmentEdit->toPlainText();

	m_shader->reloadVertexShaderFromMemory(st1.toStdString().c_str());
	m_shader->reloadFragmentShaderFromMemory(st2.toStdString().c_str());

	m_shader->recompile();
	updateGLMatrices();
}

/*******************************************************************************
 *														Init Functions
 *******************************************************************************/

Clipping::Clipping():
	m_drawVertices(false),
	m_drawLines(true),
	m_drawFaces(true),
	m_drawTopo(false),
	m_render(NULL),
	m_render_topo(NULL),
	m_positionVBO(NULL),
	m_shader(NULL),
	m_timer(NULL),
	m_lastAnimatedClippingPreset(NULL)
{
	m_coeffTopoExplod = Geom::Vec3f(0.9f,0.9f,0.9f);
}

void Clipping::initGUI()
{
	CGoGNStream::allToConsole(this) ;
	CGoGNStream::allToStd(true) ;

	setDock(&dock);

	dock.setWindowFlags(Qt::WindowMinimizeButtonHint);

	setCallBack(dock.checkBox_vertices, SIGNAL(toggled(bool)), SLOT(slot_drawVertices(bool)));
	setCallBack(dock.checkBox_lines, SIGNAL(toggled(bool)), SLOT(slot_drawLines(bool)));
	setCallBack(dock.checkBox_faces, SIGNAL(toggled(bool)), SLOT(slot_drawFaces(bool)));
	setCallBack(dock.groupBox_topo, SIGNAL(toggled(bool)), SLOT(slot_drawTopo(bool)));

	setCallBack(dock.explod_phi1, SIGNAL(valueChanged(double)), SLOT(slot_explodTopoPhi1(double)));
	setCallBack(dock.explod_phi2, SIGNAL(valueChanged(double)), SLOT(slot_explodTopoPhi2(double)));
	setCallBack(dock.explod_phi3, SIGNAL(valueChanged(double)), SLOT(slot_explodTopoPhi3(double)));

	setCallBack(dock.pushButton_addPlane, SIGNAL(clicked()), SLOT(slot_pushButton_addPlane()));

	setCallBack(dock.spinBox_GridResolution, SIGNAL(valueChanged(int)), SLOT(slot_spinBox_GridResolution(int)));
	setCallBack(dock.pushButton_changePlanesColor, SIGNAL(clicked()), SLOT(slot_pushButton_changePlanesColor()));

	setCallBack(dock.compileButton, SIGNAL(clicked()), SLOT(button_compile()));

	dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
	dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

	unsigned int planesPrecision1, planesPrecision2;
	m_planeDrawable->getPrecisionDrawing(planesPrecision1, planesPrecision2);
	dock.spinBox_GridResolution->setValue(planesPrecision1);

	setCallBack(dock.pushButton_addSphere, SIGNAL(clicked()), SLOT(slot_pushButton_addSphere()));

	setCallBack(dock.spinBox_SphereResolution, SIGNAL(valueChanged(int)), SLOT(slot_spinBox_SphereResolution(int)));
	setCallBack(dock.pushButton_changeSpheresColor, SIGNAL(clicked()), SLOT(slot_pushButton_changeSpheresColor()));

	unsigned int spheresPrecision1, spheresPrecision2;
	m_sphereDrawable->getPrecisionDrawing(spheresPrecision1, spheresPrecision2);
	dock.spinBox_SphereResolution->setValue(spheresPrecision1);

	setCallBack(dock.doubleSpinBox_ColorAttenuationFactor, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_ColorAttenuationFactor(double)));
	setCallBack(dock.radioButton_ClippingModeAnd, SIGNAL(toggled(bool)), SLOT(slot_radioButton_ClippingMode(bool)));
	setCallBack(dock.radioButton_ColorAttenuationModeLinear, SIGNAL(toggled(bool)), SLOT(slot_radioButton_ColorAttenuationMode(bool)));

	dock.doubleSpinBox_ColorAttenuationFactor->setValue(m_shader->getClipColorAttenuationFactor());
	Utils::ClippingShader::clippingMode clipMode = m_shader->getClipMode();
	if (clipMode == Utils::ClippingShader::CLIPPING_MODE_AND)
		dock.radioButton_ClippingModeAnd->setChecked(true);
	else if (clipMode == Utils::ClippingShader::CLIPPING_MODE_OR)
		dock.radioButton_ClippingModeOr->setChecked(true);
	Utils::ClippingShader::colorAttenuationMode colorAttMode = m_shader->getClipColorAttenuationMode();
	if (colorAttMode == Utils::ClippingShader::COLOR_ATTENUATION_MODE_LINEAR)
		dock.radioButton_ColorAttenuationModeLinear->setChecked(true);
	else if (colorAttMode == Utils::ClippingShader::COLOR_ATTENUATION_MODE_QUADRATIC)
		dock.radioButton_ColorAttenuationModeQuadratic->setChecked(true);

	setCallBack(dock.PushButton_ApplyStaticClippingPreset, SIGNAL(clicked()), SLOT(slot_pushButton_applyStaticClippingPreset()));

	dock.comboBox_StaticClippingPresets->addItem("Dual Planes");
	dock.comboBox_StaticClippingPresets->addItem("Cube");
	dock.comboBox_StaticClippingPresets->addItem("Tube");
	dock.comboBox_StaticClippingPresets->addItem("Molecule");

	setCallBack(dock.PushButton_ApplyAnimatedClippingPreset, SIGNAL(clicked()), SLOT(slot_pushButton_applyAnimatedClippingPreset()));

	dock.comboBox_AnimatedClippingPresets->addItem("Moving Dual Planes");
	dock.comboBox_AnimatedClippingPresets->addItem("Rotating Plane");
	dock.comboBox_AnimatedClippingPresets->addItem("Scaled Sphere");
	dock.comboBox_AnimatedClippingPresets->addItem("Spheres Cube Collision");

	setCallBack(dock.pushButton_StopAnimation, SIGNAL(clicked()), SLOT(slot_pushButton_StopAnimation()));
	setCallBack(dock.doubleSpinBox_AnimatedClippingPresetSpeed, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_AnimatedClippingPresetSpeed(double)));

	// timer used for animation
	m_timer = new QTimer( this );
	setCallBack( m_timer, SIGNAL(timeout()), SLOT(slot_animationTimer()) );
}

void Clipping::cb_Open()
{
	std::string filters("all (*.*)") ;
	std::string filename = selectFile("Open Mesh", "", filters) ;

	importMesh(filename) ;
	updateGL() ;
}

void Clipping::importMesh(std::string& filename)
{
	std::vector<std::string> attrNames ;

	size_t pos = filename.rfind(".");    // position of "." in filename
	std::string extension = filename.substr(pos);

    if(extension == std::string(".map"))
    {
        if(!myMap.loadMapBin(filename))
        {
            CGoGNerr << "could not import " << filename << CGoGNendl ;
            return;
        }
        else
			position = myMap.getAttribute<VEC3, VERTEX ,MAP>("position") ;
    }
    else
    {
        if(!Algo::Volume::Import::importMesh<PFP>(myMap, filename, attrNames))
        {
            std::cerr << "could not import " << filename << std::endl ;
            return ;
        }
        else
			position = myMap.getAttribute<VEC3, VERTEX, MAP>(attrNames[0]) ;
    }

	updateVBOprimitives(Algo::Render::GL2::TRIANGLES | Algo::Render::GL2::LINES | Algo::Render::GL2::POINTS) ;

	m_render_topo->updateData(myMap, position, m_coeffTopoExplod[0], m_coeffTopoExplod[1], m_coeffTopoExplod[2]);

	m_bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
	gPosObj = m_bb.center() ;
	float tailleX = m_bb.size(0) ;
	float tailleY = m_bb.size(1) ;
	float tailleZ = m_bb.size(2) ;
	gWidthObj = std::max<float>(std::max<float>(tailleX, tailleY), tailleZ) ;

	m_positionVBO->updateData(position) ;

	setParamObject(gWidthObj, gPosObj.data()) ;

	updateGLMatrices();

	m_shader->setClipColorAttenuationFactorRelative(m_bb.maxSize(), 5.0);
}

/*******************************************************************************
 *														GL FUNCTIONS
 *******************************************************************************/

void Clipping::cb_initGL()
{
	// create the render
	m_render = new Algo::Render::GL2::MapRender();
	m_render_topo = new Algo::Render::GL2::Topo3RenderMap<PFP>();

	// create VBO for position
	m_positionVBO = new Utils::VBO();

	// using simple shader with color
	m_shader = new Utils::ShaderSimpleColor();
	m_shader->setAttributePosition(m_positionVBO);
	m_shader->setColor(Geom::Vec4f(0.,1.,0.,0.));

	registerShader(m_shader);

	// setup clipping
	m_shader->insertClippingCode();

	// setup clipping shapes
	m_planeDrawable = new Utils::Grid;
	m_planeDrawable->setColor(Geom::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
	m_planeDrawable->updatePrecisionDrawing(5);
	m_sphereDrawable = new Utils::IcoSphere;
	m_sphereDrawable->setColor(Geom::Vec4f(0.0f, 0.4f, 1.0f, 1.0f));
	m_sphereDrawable->updatePrecisionDrawing(1);

	// setup clipping picking frame
	m_frameManipulator = new Utils::FrameManipulator();
	m_lastPickedObject = NULL;
}

void Clipping::updateVBOprimitives(int upType)
{
	if(upType & Algo::Render::GL2::TRIANGLES)
		m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::TRIANGLES) ;

	if(upType & Algo::Render::GL2::LINES)
		m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::LINES,false) ;

	if(upType & Algo::Render::GL2::POINTS)
		m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::POINTS) ;
}

void Clipping::cb_redraw()
{
	if(m_drawVertices)
	{
		m_shader->setColor(Geom::Vec4f(0.,0.,1.,0.));
		m_render->draw(m_shader, Algo::Render::GL2::POINTS);
	}

	if(m_drawLines)
	{
		glLineWidth(2.0f);
		m_shader->setColor(Geom::Vec4f(1.,1.,0.,0.));
		m_render->draw(m_shader, Algo::Render::GL2::LINES);
	}

	if(m_drawFaces)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_LIGHTING);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);
		m_shader->setColor(Geom::Vec4f(0.,1.,0.,0.));
		m_render->draw(m_shader, Algo::Render::GL2::TRIANGLES);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	if (m_drawTopo)
		m_render_topo->drawTopo();

	// Display clipping shapes
	for (size_t i = 0; i < m_pickablePlanes.size(); i++)
	{
		if (m_pickablePlanes[i] == m_lastPickedObject)
			glLineWidth(5.0);
		else
			glLineWidth(2.0);
		m_pickablePlanes[i]->draw();
	}
	for (size_t i = 0; i < m_pickableSpheres.size(); i++)
	{
		if (m_pickableSpheres[i] == m_lastPickedObject)
			glLineWidth(5.0);
		else
			glLineWidth(2.0);
		m_pickableSpheres[i]->draw();
	}

	// Display picking frame
	if (m_lastPickedObject)
		m_frameManipulator->draw();

}

void Clipping::cb_keyPress(int code)
{
	switch (code)
	{
		case 16777223 : // Suppr
			slot_pushButton_deleteSelectedObject();
			break;

		case '+' :
			dock.doubleSpinBox_AnimatedClippingPresetSpeed->setValue(
					dock.doubleSpinBox_AnimatedClippingPresetSpeed->value() + 0.1);
			break;

		case '-' :
			dock.doubleSpinBox_AnimatedClippingPresetSpeed->setValue(
					dock.doubleSpinBox_AnimatedClippingPresetSpeed->value() - 0.1);
			break;
	}
}

void Clipping::cb_mousePress(int /*button*/, int x, int y)
{
	// Check if no animation is active
	if (m_lastAnimatedClippingPreset != NULL)
		return;

	if (!Shift())
		return;

	m_lastClickedX = x;
	m_lastClickedY = y;

	// get ray of selection
	Geom::Vec3f rayA, rayB;
	float dist = getOrthoScreenRay(x, y, rayA, rayB);
	Geom::Vec3f AB = rayB - rayA;


	unsigned int newPickedFrameAxis = 0;
	unsigned int lastPickedFrameAxis = m_frameManipulatorPickedAxis;
	if (m_lastPickedObject)	// an object is already picked ? => frame is drawn
	{
		// picking the frame -> axis
		newPickedFrameAxis = m_frameManipulator->pick(rayA, AB, dist);
		m_frameManipulatorPickedAxis = newPickedFrameAxis;

		// highlight new axis
		if (lastPickedFrameAxis != newPickedFrameAxis)
			m_frameManipulator->highlight(m_frameManipulatorPickedAxis);
	}

	if (newPickedFrameAxis == 0)	// frame not picked -> pick the pickable objects
	{
		std::vector<Utils::Pickable*> tempo = m_pickablePlanes;
		tempo.insert(tempo.end(), m_pickableSpheres.begin(), m_pickableSpheres.end());
		m_lastPickedObject = Utils::Pickable::pick(tempo, rayA, AB);

		// set FrameManipulator on picked object
		if (m_lastPickedObject)
		{
			m_frameManipulator->setTransformation(m_lastPickedObject->transfo());

			// Lock individual scale axes for the sphere
			if (m_lastPickedObject->checkType<Utils::Grid>())
				m_frameManipulator->unlock(Utils::FrameManipulator::Scales);
			else if (m_lastPickedObject->checkType<Utils::Sphere>())
				m_frameManipulator->lock(Utils::FrameManipulator::Scales);
		}
	}

	// store origin & selected axis on screen projection for easy manipulation.
	m_frameManipulator->storeProjection(m_frameManipulatorPickedAxis);

	updateGL();
}

void Clipping::cb_mouseMove(int buttons, int x, int y)
{
	// Check if no animation is active
	if (m_lastAnimatedClippingPreset != NULL)
		return;

	if (!Shift())
		return;

	bool clippingUpdateNeeded = false;

	// rotation selected ?
	if (Utils::FrameManipulator::rotationAxis(m_frameManipulatorPickedAxis))
	{
		if (buttons & 1)
		{
			float angle = m_frameManipulator->angleFromMouse(x, y, x - m_lastClickedX, y - m_lastClickedY);
			m_frameManipulator->rotate(m_frameManipulatorPickedAxis, angle);
		}
		else if (buttons & 2)
			m_frameManipulator->rotateInScreen(x - m_lastClickedX, y - m_lastClickedY);

		m_lastPickedObject->transfo() = m_frameManipulator->transfo();

		clippingUpdateNeeded = true;
	}
	// translation selected
	else if (Utils::FrameManipulator::translationAxis(m_frameManipulatorPickedAxis))
	{
		if (buttons & 1)
		{
			float dist =  m_frameManipulator->distanceFromMouse(x - m_lastClickedX, y - m_lastClickedY);
			m_frameManipulator->translate(m_frameManipulatorPickedAxis, dist);
		}
		else if (buttons & 2)
			m_frameManipulator->translateInScreen(x - m_lastClickedX, y - m_lastClickedY);

		m_lastPickedObject->transfo() = m_frameManipulator->transfo();

		clippingUpdateNeeded = true;
	}
	// scale selected
	else if (Utils::FrameManipulator::scaleAxis(m_frameManipulatorPickedAxis) )
	{
		float scale = m_frameManipulator->scaleFromMouse(x - m_lastClickedX, y - m_lastClickedY);
		m_frameManipulator->scale(m_frameManipulatorPickedAxis, scale );
		m_lastPickedObject->transfo() = m_frameManipulator->transfo();

		clippingUpdateNeeded = true;
	}

	// Update clipping shape if needed
	if (clippingUpdateNeeded && m_lastPickedObject)
	{
		if (m_lastPickedObject->checkType<Utils::Grid>())
		{
			// Get plane position and normal
			Geom::Vec3f planeNewPos = m_lastPickedObject->getPosition();
			float planeNewAxisScale; // Not used
			Geom::Vec3f planeNewNormal = m_lastPickedObject->getAxisScale(2, planeNewAxisScale); // 2 = Z axis = plane normal

			// Update clipping shape
			m_shader->setClipPlaneParamsAll(m_lastPickedObject->id(), planeNewNormal, planeNewPos);
		}
		else if (m_lastPickedObject->checkType<Utils::Sphere>())
		{
			// Get sphere position and size
			Geom::Vec3f sphereNewPos = m_lastPickedObject->getPosition();
			float sphereNewSize;
			m_lastPickedObject->getAxisScale(0, sphereNewSize); // On the sphere, every axis has the same size, so take 0 as default

			// Update clipping shape
			m_shader->setClipSphereParamsAll(m_lastPickedObject->id(), sphereNewPos, sphereNewSize);
		}
	}

	m_lastClickedX = x;
	m_lastClickedY = y;

	updateGL();

}

/**********************************************************************************************
 *                  MAIN FUNCTION                         								      *
 **********************************************************************************************/

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    Clipping sqt;
    sqt.setGeometry(0, 0, 1000, 800);
    sqt.show();

	if(argc == 2)
    {
		std::string filename(argv[1]);
		sqt.importMesh(filename);
    }
	else
	{
		sqt.position = sqt.myMap.addAttribute<VEC3, VERTEX, MAP>("position");

        Algo::Volume::Tilings::Cubic::Grid<PFP> cubic(sqt.myMap, 10,10,10);
        cubic.embedIntoGrid(sqt.position, 1.0f, 1.0f, 1.0f);
	}

    sqt.initGUI();

    return app.exec();
}
