/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
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
* Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include "pmView.h"

/**********************************************************************************************
 *                                 AntTweakBar CALLBACKS                                      *
 **********************************************************************************************/

void TW_CALL setRenderStyleCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->renderStyle = *(GLenum*)value ;
	if(mgw->renderStyle == MyGlutWin::FLAT)
		TwDefine("Viewer/FaceShrinkage visible=true") ;
	else
		TwDefine("Viewer/FaceShrinkage visible=false") ;
	glutPostRedisplay() ;
}
void TW_CALL getRenderStyleCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLenum*)value = mgw->renderStyle ;
}

void TW_CALL setShininessCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->shininess = *(float*)value ;
	glutPostRedisplay() ;
}
void TW_CALL getShininessCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(float*)value = mgw->shininess ;
}

void TW_CALL setFocalCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->setFocal(*(float*)value) ;
	mgw->reshape(-1, -1) ;
	glutPostRedisplay() ;
}
void TW_CALL getFocalCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(float*)value = mgw->getFocal() ;
}

void TW_CALL setRenderObjectCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->renderObject = *(GLboolean*)value ;
	glutPostRedisplay() ;
}
void TW_CALL getRenderObjectCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLboolean*)value = mgw->renderObject ;
}

void TW_CALL setRenderLinesCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->renderLines = *(GLboolean*)value ;
	glutPostRedisplay() ;
}
void TW_CALL getRenderLinesCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLboolean*)value = mgw->renderLines ;
}

void TW_CALL setRenderNormalsCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->renderNormals = *(GLboolean*)value ;
	if(mgw->renderNormals)
	{
		mgw->initDLNormals() ;
		TwDefine("Viewer/NormalScaleFactor visible=true") ;
	}
	else
		TwDefine("Viewer/NormalScaleFactor visible=false") ;
	glutPostRedisplay() ;
}
void TW_CALL getRenderNormalsCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLboolean*)value = mgw->renderNormals ;
}

void TW_CALL setNormalScaleFactorCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->normalScaleFactor = *(float*)value ;
	if(mgw->renderNormals) mgw->initDLNormals() ;
	glutPostRedisplay() ;
}
void TW_CALL getNormalScaleFactorCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(float*)value = mgw->normalScaleFactor ;
}

void TW_CALL setFaceShrinkageCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->faceShrinkage = *(float*)value ;
	glutPostRedisplay() ;
}
void TW_CALL getFaceShrinkageCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(float*)value = mgw->faceShrinkage ;
}

void TW_CALL startPM_CB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->start_PM() ;
	if(mgw->m_pmesh)
	{
		if(mgw->renderNormals) mgw->initDLNormals() ;
		glutPostRedisplay() ;
		TwDefine("Viewer/Start_Progressive_Mesh visible=false") ;
		TwDefine("Viewer/Stop_Progressive_Mesh visible=true") ;
		TwDefine("Viewer/FastViz visible=true") ;
		TwDefine("Viewer/Resolution_Level visible=true") ;
		if(!mgw->m_pmesh->predictors().empty())
		{
			TwDefine("Viewer/Detail_% visible=true") ;
			TwDefine("Viewer/RecomputeApproxAndDetails visible=true") ;
			TwDefine("Viewer/NbQuantizationClasses visible=true") ;
			TwDefine("Viewer/QuantizeDetails visible=true") ;
			TwDefine("Viewer/LocalizeDetails visible=true") ;
			TwDefine("Viewer/DistanceColored visible=true") ;
		}
	}
}

void TW_CALL stopPM_CB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->stop_PM() ;
	if(mgw->renderNormals) mgw->initDLNormals() ;
	glutPostRedisplay() ;
	TwDefine("Viewer/Start_Progressive_Mesh visible=true") ;
	TwDefine("Viewer/Stop_Progressive_Mesh visible=false") ;
	TwDefine("Viewer/FastViz visible=false") ;
	TwDefine("Viewer/Resolution_Level visible=false") ;
	TwDefine("Viewer/Detail_% visible=false") ;
	TwDefine("Viewer/RecomputeApproxAndDetails visible=false") ;
	TwDefine("Viewer/NbQuantizationClasses visible=false") ;
	TwDefine("Viewer/QuantizeDetails visible=false") ;
	TwDefine("Viewer/ResetDetails visible=false") ;
	TwDefine("Viewer/LocalizeDetails visible=false") ;
	TwDefine("Viewer/GlobalizeDetails visible=false") ;
	TwDefine("Viewer/DistanceColored visible=false") ;
	TwDefine("Viewer/Distance visible=false") ;
}

void TW_CALL setApproximatorCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->approximator = *(Algo::Decimation::ApproximatorType*)value ;
}
void TW_CALL getApproximatorCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(Algo::Decimation::ApproximatorType*)value = mgw->approximator ;
}

void TW_CALL setFastVizCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->usePrecomputedTableIndices = *(GLboolean*)value ;
	if(mgw->usePrecomputedTableIndices)
	{
		mgw->computeVBOprimitives() ;
	}
	else
	{
		mgw->resetVBOprimitives() ;
		unsigned int l = (float(mgw->currentLevel)/100.0f)*(mgw->m_pmesh->nbSplits()) ;
		mgw->m_pmesh->gotoLevel(l) ;
	}
}
void TW_CALL getFastVizCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLboolean*)value = mgw->usePrecomputedTableIndices ;
}

void TW_CALL setResolutionLevelCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->currentLevel = *(unsigned int*)value ;
	if(mgw->usePrecomputedTableIndices)
		mgw->updateVBOprimitives(Algo::Render::VBO::TRIANGLES | Algo::Render::VBO::LINES) ;
	else
	{
		mgw->m_pmesh->gotoLevel((float(mgw->currentLevel)/100.0f)*(mgw->m_pmesh->nbSplits())) ;
		mgw->updateVBOprimitives(Algo::Render::VBO::TRIANGLES | Algo::Render::VBO::LINES) ;
	}
	if(mgw->renderNormals) mgw->initDLNormals() ;
	glutPostRedisplay() ;
}
void TW_CALL getResolutionLevelCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(unsigned int*)value = mgw->currentLevel ;
}

void TW_CALL setDetailAmountCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	unsigned int l = *(unsigned int*)value ;
	mgw->m_pmesh->setDetailAmount(double(l)/100.0) ;
	mgw->updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
	if(mgw->renderNormals) mgw->initDLNormals() ;
	glutPostRedisplay() ;
}
void TW_CALL getDetailAmountCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(unsigned int*)value = mgw->m_pmesh->detailAmount() * 100 ;
}

void TW_CALL recomputeApproxAndDetailsCB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->m_pmesh->recomputeApproxAndDetails() ;
	mgw->updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
	if(mgw->renderNormals) mgw->initDLNormals() ;
}

void TW_CALL quantizeDetailsCB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->m_pmesh->quantizeDetailVectors(mgw->nbQuantizationClasses) ;
	mgw->updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
	if(mgw->distanceColoredMesh)
		mgw->updateVBOdata(Algo::Render::VBO::COLORS) ;
	if(mgw->renderNormals) mgw->initDLNormals() ;
	TwDefine("Viewer/QuantizeDetails visible=false") ;
	TwDefine("Viewer/ResetDetails visible=true") ;
}

void TW_CALL resetDetailsCB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->m_pmesh->resetDetailVectors() ;
	mgw->updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
	if(mgw->distanceColoredMesh)
		mgw->updateVBOdata(Algo::Render::VBO::COLORS) ;
	if(mgw->renderNormals) mgw->initDLNormals() ;
	TwDefine("Viewer/ResetDetails visible=false") ;
	TwDefine("Viewer/QuantizeDetails visible=true") ;
}

void TW_CALL localizeDetailsCB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->m_pmesh->localizeDetailVectors() ;
	mgw->updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
	if(mgw->distanceColoredMesh)
		mgw->updateVBOdata(Algo::Render::VBO::COLORS) ;
	if(mgw->renderNormals) mgw->initDLNormals() ;
	TwDefine("Viewer/LocalizeDetails visible=false") ;
	TwDefine("Viewer/GlobalizeDetails visible=true") ;
}

void TW_CALL globalizeDetailsCB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->m_pmesh->globalizeDetailVectors() ;
	mgw->updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
	if(mgw->distanceColoredMesh)
		mgw->updateVBOdata(Algo::Render::VBO::COLORS) ;
	if(mgw->renderNormals) mgw->initDLNormals() ;
	TwDefine("Viewer/GlobalizeDetails visible=false") ;
	TwDefine("Viewer/LocalizeDetails visible=true") ;
}

void TW_CALL setDistanceColoredRenderCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->distanceColoredMesh = *(GLboolean*)value ;
	if(mgw->distanceColoredMesh)
	{
		mgw->updateVBOdata(Algo::Render::VBO::COLORS) ;
		TwDefine("Viewer/Distance visible=true") ;
	}
	else
		TwDefine("Viewer/Distance visible=false") ;
	glutPostRedisplay() ;
}
void TW_CALL getDistanceColoredRenderCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(unsigned int*)value = mgw->distanceColoredMesh ;
}


/**********************************************************************************************
 *                                 MyGlutWin IMPLEMENTATION                                   *
 **********************************************************************************************/

MyGlutWin::MyGlutWin(int* argc, char **argv, int winX, int winY) :
	GlutWin_ATB(argc, argv, winX, winY),
	renderStyle(FLAT),
	renderObject(true),
	renderLines(false),
	renderNormals(false),
	vbo_render(NULL),
	selecting(false),
	deselecting(false),
	dragging(false),
	m_pmesh(NULL),
	inactiveMarker(myMap),
	distanceColoredMesh(false)
{
	normalScaleFactor = 1.0f ;
	faceShrinkage = 0.0f ;

	colClear = Geom::Vec4f(0.2f, 0.2f, 0.2f, 0.1f) ;
	colDif = Geom::Vec4f(0.8f, 0.9f, 0.7f, 1.0f) ;
	colSpec = Geom::Vec4f(0.9f, 0.9f, 0.9f, 1.0f) ;
	colNormal = Geom::Vec4f(1.0f, 0.0f, 0.0f, 1.0f) ;
	shininess = 80.0f ;

	setFocal(5.0f) ;

	dl_norm = glGenLists(1) ;
	if(this->shaderOk)
	{
		shaders[0].loadShaders("phong_vs.txt", "phong_ps.txt") ;
		shaders[1].loadShaders("flat_vs.txt", "flat_ps.txt", "flat_gs.txt") ;
	}

	activeDartsSelector = new SelectorUnmarked(inactiveMarker) ;
}

void MyGlutWin::init()
{
	remainingVertices = 25 ;
	nbQuantizationClasses = 256 ;
	selector = Algo::Decimation::S_QEM ;
	approximator = Algo::Decimation::A_QEM ;

	originalPosition = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "originalPosition") ;
	normal = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "normal") ;
	color =  myMap.addAttribute<Geom::Vec4f>(VERTEX_ORBIT, "color") ;

	selectionMarker = new CellMarker(myMap, VERTEX_CELL) ;
	origResVertexMarker = new CellMarker(myMap, VERTEX_CELL) ;

	origResVertexMarker->markAll() ;

	vbo_render = new Algo::Render::VBO::MapRender_VBO() ;
	updateVBOprimitives( Algo::Render::VBO::TRIANGLES | Algo::Render::VBO::LINES) ;
	updateVBOdata( Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
	usePrecomputedTableIndices = false ;
}

void MyGlutWin::initGUI()
{
	viewer_bar = TwNewBar("Viewer") ;
	TwDefine("Viewer position='16 16' size='220 450' valueswidth='85'") ;

	TwType tw_render_style = TwDefineEnum("Shading","Flat,Gouraud,Phong") ;
	TwAddVarCB(viewer_bar, "RenderStyle", tw_render_style, setRenderStyleCB, getRenderStyleCB, this, "") ;
	TwAddVarCB(viewer_bar, "Shininess", TW_TYPE_FLOAT, setShininessCB, getShininessCB, this, "min=1 max=255 step=1") ;
	TwAddVarCB(viewer_bar, "Focal", TW_TYPE_FLOAT, setFocalCB, getFocalCB, this, "min=0.6 max=5.0 step=0.1") ;
	TwAddVarCB(viewer_bar, "Show Object", TW_TYPE_BOOL8, setRenderObjectCB, getRenderObjectCB, this, "") ;
	TwAddVarCB(viewer_bar, "Show Lines", TW_TYPE_BOOL8, setRenderLinesCB, getRenderLinesCB, this, "") ;
	TwAddVarCB(viewer_bar, "Show Normals", TW_TYPE_BOOL8, setRenderNormalsCB, getRenderNormalsCB, this, "") ;
	TwAddVarCB(viewer_bar, "NormalScaleFactor", TW_TYPE_FLOAT, setNormalScaleFactorCB, getNormalScaleFactorCB, this, "min=0.1 max=5.0 step=0.1") ;
	TwDefine("Viewer/NormalScaleFactor visible=false") ;
	TwAddVarCB(viewer_bar, "FaceShrinkage", TW_TYPE_FLOAT, setFaceShrinkageCB, getFaceShrinkageCB, this, "min=0.0 max=0.9 step=0.01") ;

	TwAddSeparator(viewer_bar, "", "") ;

	TwType tw_Selector = TwDefineEnum("Selectors","MapOrder,Random,EdgeLength,QEM,MinDetail,Curvature,LightField") ;
	TwType tw_Approximator = TwDefineEnum("Approximators","QEM,MidEdge,HalfCollapse,CornerCutting,TangentPredict1,TangentPredict2,LightfieldFull") ;
	TwAddVarRW(viewer_bar, "Selector", tw_Selector, &selector, "") ;
	TwAddVarCB(viewer_bar, "Approximator", tw_Approximator, setApproximatorCB, getApproximatorCB, this, "") ;

	TwAddSeparator(viewer_bar, "", "") ;

	TwAddVarRW(viewer_bar, "NbVertices_%", TW_TYPE_UINT32, &remainingVertices, "min=0 max=100 step=1") ;
	TwAddButton(viewer_bar, "Start_Progressive_Mesh", startPM_CB, this, "") ;
	TwAddButton(viewer_bar, "Stop_Progressive_Mesh", stopPM_CB, this, "") ;
	TwDefine("Viewer/Stop_Progressive_Mesh visible=false") ;

	TwAddSeparator(viewer_bar, "", "") ;

	TwAddVarCB(viewer_bar, "FastViz", TW_TYPE_BOOL8, setFastVizCB, getFastVizCB, this, "") ;
	TwDefine("Viewer/FastViz visible=false") ;
	TwAddVarCB(viewer_bar, "Resolution_Level", TW_TYPE_UINT32, setResolutionLevelCB, getResolutionLevelCB, this, "min=0 max=100 step=1") ;
	TwDefine("Viewer/Resolution_Level visible=false") ;
	TwAddVarCB(viewer_bar, "Detail_%", TW_TYPE_UINT32, setDetailAmountCB, getDetailAmountCB, this, "min=0 max=150 step=1") ;
	TwDefine("Viewer/Detail_% visible=false") ;

	TwAddSeparator(viewer_bar, "", "") ;

	TwAddButton(viewer_bar, "RecomputeApproxAndDetails", recomputeApproxAndDetailsCB, this, "") ;
	TwDefine("Viewer/RecomputeApproxAndDetails visible=false") ;
	TwAddVarRW(viewer_bar, "NbQuantizationClasses", TW_TYPE_UINT32, &nbQuantizationClasses, "min=2") ;
	TwDefine("Viewer/NbQuantizationClasses visible=false") ;
	TwAddButton(viewer_bar, "QuantizeDetails", quantizeDetailsCB, this, "") ;
	TwDefine("Viewer/QuantizeDetails visible=false") ;
	TwAddButton(viewer_bar, "ResetDetails", resetDetailsCB, this, "") ;
	TwDefine("Viewer/ResetDetails visible=false") ;
	TwAddButton(viewer_bar, "LocalizeDetails", localizeDetailsCB, this, "") ;
	TwDefine("Viewer/LocalizeDetails visible=false") ;
	TwAddButton(viewer_bar, "GlobalizeDetails", globalizeDetailsCB, this, "") ;
	TwDefine("Viewer/GlobalizeDetails visible=false") ;
	TwAddVarCB(viewer_bar, "DistanceColored", TW_TYPE_BOOL8, setDistanceColoredRenderCB, getDistanceColoredRenderCB, this, "") ;
	TwDefine("Viewer/DistanceColored visible=false") ;
	TwAddVarRW(viewer_bar, "Distance", TW_TYPE_FLOAT, &meshDistanceToOriginal, "") ;
	TwDefine("Viewer/Distance visible=false") ;
}

void MyGlutWin::start_PM()
{
	if(!m_pmesh)
	{
		saveOriginalPositions() ;

		meshDistanceToOriginal = 0.0f ;

		std::cout << "Initializing Progressive Mesh.." << std::endl ;
        GLint t1 = glutGet(GLUT_ELAPSED_TIME) ;
		m_pmesh = new Algo::PMesh::ProgressiveMesh<PFP>(myMap, inactiveMarker, selector, approximator, position) ;
		if(m_pmesh->initOk())
		{
			m_pmesh->createPM(remainingVertices) ;
			GLint t2 = glutGet(GLUT_ELAPSED_TIME) ;
			GLfloat seconds = (t2 - t1) / 1000.0f ;
			std::cout << "..done (" << seconds << " sec)" << std::endl ;

			currentLevel = 100 ;

			updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
			updateVBOprimitives(Algo::Render::VBO::TRIANGLES | Algo::Render::VBO::LINES) ;
		}
		else
		{
			delete m_pmesh ;
			m_pmesh = NULL ;
			std::cout << "..failed (probable incompatible settings)" << std::endl ;
		}
	}
}

void MyGlutWin::stop_PM()
{
	if(m_pmesh)
	{
		m_pmesh->gotoLevel(0) ;

		restoreOriginalPositions() ;

		std::cout << "Removing Progressive Mesh.." << std::flush ;
		delete m_pmesh ;
		m_pmesh = NULL ;
		std::cout << "..done" << std::endl ;

		resetVBOprimitives() ;
		usePrecomputedTableIndices = false ;
		updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
		updateVBOprimitives(Algo::Render::VBO::TRIANGLES | Algo::Render::VBO::LINES) ;
		distanceColoredMesh = false ;
	}
}

void MyGlutWin::myRedraw()
{
	GLfloat black[4] = { 0.0f, 0.0f, 0.0f, 1.0f } ;
	GLfloat amb[4] = { 0.2f, 0.1f, 0.1f, 1.0f } ;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;

	glPushMatrix() ;

	float sc = 50. / gWidthObj ;
	glScalef(sc, sc, sc) ;
	glTranslatef(-gPosObj[0], -gPosObj[1], -gPosObj[2]) ;

	if(renderLines)
	{
		glDisable(GL_LIGHTING);
		glColor3f(0.0f,0.0f,0.0f);
		vbo_render->draw(Algo::Render::VBO::LINES) ;
	}

	if(renderNormals)
		glCallList(dl_norm) ;

	glEnable(GL_POLYGON_OFFSET_FILL) ;
	glPolygonOffset(1.0f, 1.0f) ;

	if(renderObject)
	{
		if(distanceColoredMesh && m_pmesh->currentLevel() == 0)
		{
			glEnable(GL_COLOR_MATERIAL) ;
		}
		else
		{
			glDisable(GL_COLOR_MATERIAL) ;
			glMaterialfv(GL_FRONT, GL_DIFFUSE, colDif.data()) ;
			glMaterialfv(GL_FRONT, GL_AMBIENT, amb) ;
			if(renderStyle != PHONG)
			{
				glMaterialfv(GL_FRONT, GL_SPECULAR, black) ;
				glMaterialf(GL_FRONT, GL_SHININESS, 1.0f) ;
			}
			else
			{
				glMaterialfv(GL_FRONT, GL_SPECULAR, colSpec.data()) ;
				glMaterialf(GL_FRONT, GL_SHININESS, shininess) ;
			}
		}

		glEnable(GL_LIGHTING) ;
		glEnable(GL_CULL_FACE) ;
		glPolygonMode(GL_FRONT, GL_FILL) ;
		if(renderStyle == FLAT)
		{
			glShadeModel(GL_FLAT) ;
			shaders[1].bind() ;
			glUniform1fARB(glGetUniformLocationARB(shaders[1].program_handler(),"explode"), faceShrinkage) ;
			vbo_render->draw(Algo::Render::VBO::TRIANGLES) ;
			shaders[1].unbind() ;
		}
		else
		{
			glShadeModel(GL_SMOOTH) ;
			if(renderStyle == PHONG)
				shaders[0].bind() ;
			vbo_render->draw(Algo::Render::VBO::TRIANGLES) ;
			if(renderStyle == PHONG)
				shaders[0].unbind() ;
		}
	}

	glDisable(GL_POLYGON_OFFSET_FILL) ;

	renderDetailVectors() ;

	glPopMatrix() ;

	glColor3f(1.0f, 1.0f, 1.0f) ;
	int x = getWinWidth() - 170 ;
	printString2D(
		x,
		20,
		"Keys : ('h' to show/hide)\n"
		"----\n"
		"'+'/'-' : navigate into PM\n"
		"'0' : go to coarsest level\n"
		"'1' : go to finest level"
	) ;
}

void MyGlutWin::myKeyboard(unsigned char keycode, int x, int y)
{
	switch(keycode)
	{
		case '-' : {
			if(m_pmesh)
			{
				if(currentLevel > 0)
					--currentLevel ;
				if(usePrecomputedTableIndices)
					updateVBOprimitives(Algo::Render::VBO::TRIANGLES | Algo::Render::VBO::LINES) ;
				else
				{
					m_pmesh->gotoLevel((float(currentLevel)/100.0f)*(m_pmesh->nbSplits())) ;
					updateVBOprimitives(Algo::Render::VBO::TRIANGLES | Algo::Render::VBO::LINES) ;
				}
				if(renderNormals) initDLNormals() ;
				glutPostRedisplay() ;
			}
			break ;
		}

		case '+' : {
			if(m_pmesh)
			{
				if(currentLevel < 100)
					++currentLevel ;
				if(usePrecomputedTableIndices)
					updateVBOprimitives(Algo::Render::VBO::TRIANGLES | Algo::Render::VBO::LINES) ;
				else
				{
					m_pmesh->gotoLevel((float(currentLevel)/100.0f)*(m_pmesh->nbSplits())) ;
					updateVBOprimitives(Algo::Render::VBO::TRIANGLES | Algo::Render::VBO::LINES) ;
				}
				if(renderNormals) initDLNormals() ;
				glutPostRedisplay() ;
			}
			break ;
		}

		case '0' : {
			if(m_pmesh)
			{
				currentLevel = 100 ;
				if(usePrecomputedTableIndices)
					updateVBOprimitives(Algo::Render::VBO::TRIANGLES | Algo::Render::VBO::LINES) ;
				else
				{
					m_pmesh->gotoLevel(m_pmesh->nbSplits()) ;
					updateVBOprimitives(Algo::Render::VBO::TRIANGLES | Algo::Render::VBO::LINES) ;
				}
				if(renderNormals) initDLNormals() ;
				glutPostRedisplay() ;
			}
			break ;
		}

		case '1' : {
			if(m_pmesh)
			{
				currentLevel = 0 ;
				if(usePrecomputedTableIndices)
					updateVBOprimitives(Algo::Render::VBO::TRIANGLES | Algo::Render::VBO::LINES) ;
				else
				{
					m_pmesh->gotoLevel(0) ;
					updateVBOprimitives(Algo::Render::VBO::TRIANGLES | Algo::Render::VBO::LINES) ;
				}
				if(renderNormals) initDLNormals() ;
				glutPostRedisplay() ;
			}
			break ;
		}

		case 's' : {
			if(usePrecomputedTableIndices)
				m_pmesh->gotoLevel((float(currentLevel)/100.0f)*(m_pmesh->nbSplits())) ;
			FunctorSelect* dartSelector ;
			if(m_pmesh)
				dartSelector = activeDartsSelector ;
			else
				dartSelector = &allDarts ;
			Algo::Export::exportOFF<PFP>(myMap, position, "export.off", *dartSelector) ;
			Algo::Export::exportPLY<PFP>(myMap, position, "export.ply", *dartSelector) ;
		}

//		case 'b' : {
//			if(!m_pmesh)
//			{
//				selector = Algo::PMesh::S_MinDetail ;
//				evaluateScheme(Algo::PMesh::A_CornerCutting, "quantifResult_CornerCutting") ;
//				evaluateScheme(Algo::PMesh::A_TangentPredict2, "quantifResult_TangentPredict2") ;
//			}
//		}
	}
}
/*
void MyGlutWin::evaluateScheme(Algo::PMesh::ApproximatorType type, const char* filename)
{
	std::ofstream out(filename, std::ios::out) ;
	approximator = type ;
	start_PM() ;
	for(unsigned int nbClasses = 50; nbClasses <= 500; nbClasses += 50)
	{
		out << nbClasses ;
		m_pmesh->quantizeDetailVectors(nbClasses) ;
		out << " " << m_pmesh->getDifferentialEntropy() << " " << m_pmesh->getDiscreteEntropy() ;
		computeDistanceToOriginal() ;
		out << " " << meshDistanceToOriginal ;
		m_pmesh->localizeDetailVectors() ;
		out << " " << m_pmesh->getDifferentialEntropy() << " " << m_pmesh->getDiscreteEntropy() ;
		computeDistanceToOriginal() ;
		out << " " << meshDistanceToOriginal << std::endl ;
		m_pmesh->resetDetailVectors() ;
		m_pmesh->globalizeDetailVectors() ;
	}
	stop_PM() ;
	out.close() ;
}
*/

void MyGlutWin::computeVBOprimitives()
{
	std::cout << "compute VBO primitive tables.." << std::flush ;
    GLint t1 = glutGet(GLUT_ELAPSED_TIME) ;
	// save all the available primitive tables for the VBOs
	for(unsigned int i = 0; i <= 100; ++i)
	{
		unsigned int l = (float(i)/100.0f)*(m_pmesh->nbSplits()) ;
		m_pmesh->gotoLevel(l) ;
		vbo_render->initTriangles<PFP>(myMap, *activeDartsSelector, triangleTableIndices[i]) ;
		vbo_render->initLines<PFP>(myMap, *activeDartsSelector, lineTableIndices[i]) ;
//		vbo_render->optimizedTriangles<PFP>(myMap, *activeDartsSelector, triangleTableIndices[i]) ;
//		vbo_render->optimizedLines<PFP>(myMap, *activeDartsSelector, lineTableIndices[i]) ;
	}
    GLint t2 = glutGet(GLUT_ELAPSED_TIME) ;
    GLfloat seconds = (t2 - t1) / 1000.0f ;
	std::cout << "..done (" << seconds << " sec)" << std::endl ;
}

void MyGlutWin::resetVBOprimitives()
{
	for(unsigned int i = 0; i <= 100; ++i)
	{
		triangleTableIndices[i].clear() ;
		lineTableIndices[i].clear() ;
	}
}

void MyGlutWin::updateVBOprimitives(int upType)
{
	if(upType & Algo::Render::VBO::TRIANGLES)
	{
		if(usePrecomputedTableIndices)
			vbo_render->initPrimitives( Algo::Render::VBO::TRIANGLES, triangleTableIndices[currentLevel]) ;
		else
			vbo_render->initPrimitives<PFP>(myMap, *activeDartsSelector, Algo::Render::VBO::TRIANGLES) ;
	}

	if(upType & Algo::Render::VBO::LINES)
	{
		if(usePrecomputedTableIndices)
			vbo_render->initPrimitives( Algo::Render::VBO::LINES, lineTableIndices[currentLevel]) ;
		else
			vbo_render->initPrimitives<PFP>(myMap, *activeDartsSelector, Algo::Render::VBO::LINES) ;
	}
}

void MyGlutWin::updateVBOdata(int upType, bool recompute)
{
	if(upType & Algo::Render::VBO::POSITIONS)
		vbo_render->updateData(Algo::Render::VBO::POSITIONS, position) ;

	if(upType & Algo::Render::VBO::NORMALS)
	{
		if(recompute)
			Algo::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;
		vbo_render->updateData(Algo::Render::VBO::NORMALS, normal) ;
		if(renderNormals)
			initDLNormals() ;
	}

	if(upType & Algo::Render::VBO::COLORS)
	{
		float dist = 0.0f ;
		float min = std::numeric_limits<float>::max() ;
		float max = std::numeric_limits<float>::min() ;

		AttribContainer& vertexAttr = myMap.getAttributeContainer(VERTEX_ORBIT) ;

		for(unsigned int i = vertexAttr.begin(); i < vertexAttr.end(); vertexAttr.next(i))
		{
			if(origResVertexMarker->isMarked(i))
			{
				PFP::VEC3 v = position[i] - originalPosition[i] ;
				PFP::REAL d = v.norm() ;
				if(d < min) min = d ;
				if(d > max) max = d ;
				dist += d ;
			}
		}
		std::cout << "max dist = " << max << std::endl ;
		meshDistanceToOriginal = dist ;

		float r = 0.0f, g = 0.0f, b = 0.0f ;
		for(unsigned int i = vertexAttr.begin(); i < vertexAttr.end(); vertexAttr.next(i))
		{
			if(origResVertexMarker->isMarked(i))
			{
				PFP::VEC3 vec = position[i] - originalPosition[i] ;
				PFP::REAL d = vec.norm() / max ;
				r = d < 0.5f ? 0.0f : (d - 0.5f) * 2.0f ;
				g = d < 0.5f ? d * 2.0f : 1.0f - ((d - 0.5f) * 2.0f) ;
				b = d > 0.5f ? 0.0f : 1.0f - (d * 2.0f) ;
				color[i] = Geom::Vec4f(r,g,b,1) ;
			}
		}
		vbo_render->updateData(Algo::Render::VBO::COLORS, color) ;
	}
}

void MyGlutWin::initDLNormals()
{
	glNewList(dl_norm, GL_COMPILE) ;
	render(NORMALS) ;
	glEndList() ;
}

void MyGlutWin::render(int renderMode)
{
	FunctorSelect* dartSelector ;
	if(m_pmesh)
		dartSelector = activeDartsSelector ;
	else
		dartSelector = &allDarts ;

	switch(renderMode)
	{
		case NORMALS : {
			glLineWidth(1.0f) ;
			glDisable(GL_LIGHTING) ;
			glColor3fv(colNormal.data()) ;
			Algo::Render::Direct::renderNormalVertices<PFP>(myMap, position, normal, normalScaleFactor * normalBaseSize, *dartSelector) ;
			break ; }
	}
}

void MyGlutWin::renderDetailVectors()
{
	if(m_pmesh)
	{
		bool found = false ;
		std::vector<Algo::Decimation::ApproximatorGen<PFP>*> approximators = m_pmesh->approximators() ;
		for(std::vector<Algo::Decimation::ApproximatorGen<PFP>*>::iterator it = approximators.begin();
			it != approximators.end() && !found;
			++it)
		{
			if((*it)->getApproximatedAttributeName() == "position" && (*it)->getPredictor())
			{
				Algo::Decimation::Approximator<PFP, PFP::VEC3>* posApprox =
					reinterpret_cast<Algo::Decimation::Approximator<PFP, PFP::VEC3>*>(*it) ;

				glPushMatrix() ;
				glTranslatef(gPosObj[0], gPosObj[1], gPosObj[2]) ;
				glLineWidth(1.0f) ;
				glDisable(GL_LIGHTING) ;
				glBegin(GL_LINES) ;
				glColor3f(1.0f, 0.0f, 0.0f) ;
				for(unsigned int i = 0; i < m_pmesh->splits().size(); ++i)
				{
					glVertex3f(0.0f, 0.0f, 0.0f) ;
					glVertex3fv(posApprox->getDetail(m_pmesh->splits()[i]->getEdge()).data()) ;
				}
				glEnd() ;
				glPopMatrix() ;
				found = true ;
			}
		}
	}
}

/**********************************************************************************************
 *                                      MAIN FUNCTION                                         *
 **********************************************************************************************/

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		std::cerr << "syntax: " << argv[0] << " filename" << std::endl ;
		return 1 ;
	}

	MyGlutWin* mgw = new MyGlutWin(&argc, argv, 1200, 800) ;

	char* filename = argv[1] ;

	GLint t1 = glutGet(GLUT_ELAPSED_TIME) ;
	std::vector<std::string> attrNames ;
	if(!Algo::Import::importMesh<PFP>(myMap, filename, attrNames))
	{
		std::cerr << "could not import "<< filename << std::endl ;
		exit(1);
	}
	mgw->position = myMap.getAttribute<PFP::VEC3>(VERTEX_ORBIT, attrNames[0]) ;
	GLint t2 = glutGet(GLUT_ELAPSED_TIME) ;
	GLfloat seconds = (t2 - t1) / 1000.0f ;
	std::cout << "import: " << seconds << " sec" << std::endl ;

//	myMap.check() ;
//	if(myMap.isTriangular())
//		std::cout << "triangular ok" << std::endl ;
//	else
//		std::cout << "triangular NOT ok" << std::endl ;

	std::cout << "nb vertices: " << myMap.getNbOrbits(VERTEX_ORBIT) << std::endl ;

	// compute width and position of object for centering
	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, mgw->position) ;
	mgw->gPosObj = bb.center() ;
	float tailleX = bb.size(0) ;
	float tailleY = bb.size(1) ;
	float tailleZ = bb.size(2) ;

	mgw->gWidthObj = std::max<float>(std::max<float>(tailleX, tailleY), tailleZ) ;
	mgw->normalBaseSize = std::min<float>(std::min<float>(tailleX,tailleY),tailleZ) / 50.0f ;

	mgw->init() ;
	mgw->initGUI() ;
	mgw->mainLoop() ;

	return 0 ;
}
