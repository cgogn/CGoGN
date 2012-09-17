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


#include "volumeExplorer.h"
#include <iostream>

#include "Algo/Modelisation/primitives3d.h"
#include "Algo/Modelisation/polyhedron.h"
#include "Algo/Import/import.h"
#include "Algo/Geometry/volume.h"

#include "Utils/chrono.h"


PFP::MAP myMap;
VertexAttribute<PFP::VEC3> position ;
VolumeAttribute<PFP::VEC3> color ;

void MyQT::volumes_onoff(bool x)
{
	render_volumes = !render_volumes;
	updateGL();
}

void MyQT::edges_onoff(bool x)
{
	render_edges = !render_edges;
	updateGL();
}

void MyQT::topo_onoff(bool x)
{
	render_topo = !render_topo;
	if (render_topo)
	{
		SelectorDartNoBoundary<PFP::MAP> nb(myMap);
		m_topo_render->updateData<PFP>(myMap, position, 0.8f, m_explode_factorf-0.05f, m_explode_factor, nb);
	}

	updateGL();
}

void MyQT::clipping_onoff(bool x)
{
	clip_volume = x;

	if (clip_volume)
	{
		Geom::Vec3f pos = m_PlanePick->getPosition();
		float pipo;
		Geom::Vec3f normal = m_PlanePick->getAxisScale(2, pipo); // 2 = Z axis = plane normal
		float d = -(pos*normal);
		m_explode_render->setClippingPlane(Geom::Vec4f(normal[0],normal[1],normal[2],d));
		m_topo_render->shader1()->setClipPlaneParamsAll(clip_id1, normal, pos);
		m_topo_render->shader2()->setClipPlaneParamsAll(clip_id2, normal, pos);
	}
	else
	{
		m_explode_render->setNoClippingPlane();
		m_topo_render->shader1()->setClipPlaneParamsAll(clip_id1, Geom::Vec3f(0,0,1), Geom::Vec3f(0,0,999999.9f));
		m_topo_render->shader2()->setClipPlaneParamsAll(clip_id2, Geom::Vec3f(0,0,1), Geom::Vec3f(0,0,999999.9f));
		m_topo_render->shader1()->setClipColorAttenuationFactorRelative(0.0f,0.0f);
		m_topo_render->shader2()->setClipColorAttenuationFactorRelative(0.0f,0.0f);
	}
	updateGL();
}

void MyQT::hide_onoff(bool x)
{
	hide_clipping = !hide_clipping;
	updateGL();
}


void MyQT::slider_explode(int x)
{
	m_explode_factor = 0.01f*(x+1)-0.0001f;
	m_explode_render->setExplodeVolumes(m_explode_factor);
	updateGL();
}

void MyQT::slider_explodeF(int x)
{
	m_explode_factorf = 0.01f*(x+1);
	m_explode_render->setExplodeFaces(m_explode_factorf);
	updateGL();
}


void MyQT::slider_pressed()
{
	render_topoTemp = render_topo;
	render_topo = false;
	updateGL();
}


void MyQT::slider_released()
{

	render_topo = render_topoTemp;
	if (render_topo)
	{
		SelectorDartNoBoundary<PFP::MAP> nb(myMap);
		m_topo_render->updateData<PFP>(myMap, position, 0.8f, m_explode_factorf-0.05f, m_explode_factor, nb);
	}
	updateGL();
}


void MyQT::cb_Open()
{
	std::string filters("all (*.*);; trian (*.trian);; ctm (*.ctm);; off (*.off);; ply (*.ply)") ;
	std::string filename = selectFile("Open Mesh", "", filters) ;
	if (filename.empty())
		return ;

	myMap.clear(true);

	std::vector<std::string> attrNames ;

	size_t pos = filename.rfind(".");    // position of "." in filename
	std::string extension = filename.substr(pos);

	if(extension == std::string(".tet"))
	{
		if(!Algo::Import::importTet<PFP>(myMap,filename,attrNames))
		{
			CGoGNerr << "could not import " << filename << CGoGNendl ;
			return;
		}
		else
			position = myMap.getAttribute<PFP::VEC3, VERTEX>(attrNames[0]) ;
	}

	if(extension == std::string(".node"))
	{
		if(!Algo::Import::importMeshV<PFP>(myMap, filename, attrNames, Algo::Import::ImportVolumique::NODE))
		{
			std::cerr << "could not import " << filename << std::endl ;
			return ;
		}
		else
			position = myMap.getAttribute<PFP::VEC3,VERTEX>(attrNames[0]) ;
	}


	if(extension == std::string(".off"))
	{
		if(!Algo::Import::importMeshToExtrude<PFP>(myMap, filename, attrNames))
		{
			std::cerr << "could not import " << filename << std::endl ;
			return ;
		}
		else
		{
			position = myMap.getAttribute<PFP::VEC3, VERTEX>(attrNames[0]) ;
			myMap.closeMap();
		}
	}

	color = myMap.addAttribute<PFP::VEC3, VOLUME>("color");

	TraversorCell<PFP::MAP, VOLUME> tra(myMap);
	float maxV = 0.0f;
	for (Dart d = tra.begin(); d != tra.end(); d = tra.next())
	{
		float v = Algo::Geometry::tetrahedronVolume<PFP>(myMap, d, position);
		color[d] = PFP::VEC3(v,0,0);
		if (v>maxV)
			maxV=v;
	}
	for (unsigned int i = color.begin(); i != color.end(); color.next(i))
	{
		color[i][0] /= maxV;
		color[i][2] = 1.0f - color[i][0];
	}

	SelectorDartNoBoundary<PFP::MAP> nb(myMap);
	m_topo_render->updateData<PFP>(myMap, position,  0.8f, 0.8f, 0.8f, nb);
	m_explode_render->updateData<PFP>(myMap, position, color);

	updateGL() ;
}


void MyQT::cb_initGL()
{
	// choose to use GL version 2
	Utils::GLSLShader::setCurrentOGLVersion(2);

	// create the renders
    m_topo_render = new Algo::Render::GL2::Topo3Render();
    m_explode_render = new Algo::Render::GL2::ExplodeVolumeRender(true,true);

	SelectorDartNoBoundary<PFP::MAP> nb(myMap);
	m_topo_render->updateData<PFP>(myMap, position,  0.8f, 0.8f, 0.8f, nb);
	m_explode_render->updateData<PFP>(myMap, position, color);
	m_explode_render->setExplodeVolumes(0.8f);
	m_explode_render->setExplodeFaces(0.9f);
	m_explode_render->setAmbiant(Geom::Vec4f(0.2f,0.2f,0.2f,1.0f));
	m_explode_render->setBackColor(Geom::Vec4f(0.9f,0.9f,0.9f,1.0f));
	m_explode_render->setColorLine(Geom::Vec4f(0.8f,0.2f,0.2f,1.0f));

	registerShader(m_explode_render->shaderFaces());
	registerShader(m_explode_render->shaderLines());

    m_PlanePick = new Utils::Pickable(Utils::Pickable::GRID,1);
	m_frame = new Utils::FrameManipulator();
	m_frame->setSize(m_WidthObj/2.0f);


	m_topo_render->shader1()->insertClippingCode();
	m_topo_render->shader2()->insertClippingCode();

	clip_id1 = m_topo_render->shader1()->addClipPlane();
	clip_id2 = m_topo_render->shader2()->addClipPlane();

	m_topo_render->shader1()->setClipPlaneParamsAll(clip_id1, Geom::Vec3f(0,0,1), m_PosObj);
	m_topo_render->shader2()->setClipPlaneParamsAll(clip_id2, Geom::Vec3f(0,0,1), m_PosObj);
	m_explode_render->setClippingPlane(Geom::Vec4f(0,0,1,m_PosObj*Geom::Vec3f(0,0,-1)));

}



void MyQT::cb_redraw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LIGHTING);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);

	if (render_topo)
		m_topo_render->drawTopo();

	if (render_edges)
	{
		glLineWidth(2.0f);
		m_explode_render->drawEdges();
	}

	glDisable(GL_POLYGON_OFFSET_FILL);

	if (render_volumes)
	{
		m_explode_render->drawFaces();
	}

	if (clip_volume && !hide_clipping)
	{
		m_frame->draw();
		m_PlanePick->draw();
	}
}


void  MyQT::cb_mousePress(int button, int x, int y)
{
	if (!Shift())
		return;

	if (hide_clipping || !clip_volume)
		return;

	m_begX = x;
	m_begY = y;

	// get ray of selection
	Geom::Vec3f rayA,rayB;
	float dist = getOrthoScreenRay(x,y,rayA,rayB);
	Geom::Vec3f AB = rayB-rayA;

	unsigned int fr_picked =0;
	// picking the frame -> axis
	fr_picked = m_frame->pick(rayA,AB,dist);

	if (fr_picked != 0)
	{
		m_pickedAxis=fr_picked;
		m_frame->highlight(m_pickedAxis);
		m_frame->storeProjection(m_pickedAxis);
		updateGL();
	}
}

void  MyQT::cb_mouseRelease(int button, int x, int y)
{

	if (hide_clipping || !clip_volume)
		return;

	m_pickedAxis=0;
	m_frame->highlight(m_pickedAxis);
	updateGL();

}

void  MyQT::cb_mouseMove(int buttons, int x, int y)
{
	if (!Shift())
		return;

	if (hide_clipping || !clip_volume)
		return;

	// rotation selected ?
	if (Utils::FrameManipulator::rotationAxis(m_pickedAxis))
	{
		if (buttons&1)
		{
			float angle = m_frame->angleFromMouse(x,y,x-m_begX, y-m_begY);
			m_frame->rotate(m_pickedAxis, angle);
		}
		else if (buttons&2)
			m_frame->rotateInScreen(x-m_begX, y-m_begY);

		m_PlanePick->transfo() = m_frame->transfo();
	}
	// translation selected
	else if (Utils::FrameManipulator::translationAxis(m_pickedAxis))
	{
		if (buttons&1)
		{
			float dist =  m_frame->distanceFromMouse(x-m_begX, y-m_begY);
			m_frame->translate(m_pickedAxis, dist);
		}
		else if (buttons&2)
			m_frame->translateInScreen(x-m_begX, y-m_begY);

		m_PlanePick->transfo() = m_frame->transfo();
	}
	// scale selected
	else if (Utils::FrameManipulator::scaleAxis(m_pickedAxis) )
	{
		float scale = m_frame->scaleFromMouse(x-m_begX, y-m_begY);
		m_frame->scale(m_pickedAxis, scale );
		m_PlanePick->transfo() = m_frame->transfo();
	}

	Geom::Vec3f pos = m_PlanePick->getPosition();
	float pipo;
	Geom::Vec3f normal = m_PlanePick->getAxisScale(2, pipo); // 2 = Z axis = plane normal
	float d = -(pos*normal);
	m_explode_render->setClippingPlane(Geom::Vec4f(normal[0],normal[1],normal[2],d));

	m_topo_render->shader1()->setClipPlaneParamsAll(clip_id1, normal, pos);
	m_topo_render->shader2()->setClipPlaneParamsAll(clip_id2, normal, pos);

	m_begX = x;
	m_begY = y;
	updateGL();
	return;

}

int main(int argc, char **argv)
{
	if (argc>1)
	{
		std::vector<std::string> attrNames ;
		std::string filename(argv[1]);
		size_t pos = filename.rfind(".");    // position of "." in filename
		std::string extension = filename.substr(pos);

		if(extension == std::string(".tet"))
		{
			if(!Algo::Import::importTet<PFP>(myMap,argv[1],attrNames))
			{
				CGoGNerr << "could not import " << argv[1] << CGoGNendl ;
				return 1;
			}
			else
				position = myMap.getAttribute<PFP::VEC3, VERTEX>(attrNames[0]) ;
		}

		if(extension == std::string(".node"))
		{
			if(!Algo::Import::importMeshV<PFP>(myMap, argv[1], attrNames, Algo::Import::ImportVolumique::NODE))
			{
				std::cerr << "could not import " << argv[1] << std::endl ;
				return 1;
			}
			else
				position = myMap.getAttribute<PFP::VEC3,VERTEX>(attrNames[0]) ;
		}


		if(extension == std::string(".off"))
		{
			if(!Algo::Import::importMeshToExtrude<PFP>(myMap, argv[1], attrNames))
			{
				std::cerr << "could not import "  << std::endl ;
				return 1;
			}
			else
			{
				position = myMap.getAttribute<PFP::VEC3, VERTEX>(attrNames[0]) ;
				myMap.closeMap();
			}
		}

		color = myMap.addAttribute<PFP::VEC3, VOLUME>("color");

		TraversorCell<PFP::MAP, VOLUME> tra(myMap);
		float maxV = 0.0f;
		for (Dart d = tra.begin(); d != tra.end(); d = tra.next())
		{
			float v = Algo::Geometry::tetrahedronVolume<PFP>(myMap, d, position);
			color[d] = PFP::VEC3(v,0,0);
			if (v>maxV)
				maxV=v;
		}
		for (unsigned int i = color.begin(); i != color.end(); color.next(i))
		{
			color[i][0] /= maxV;
			color[i][2] = 1.0f - color[i][0];
		}
	}
	else
	{
		position = myMap.addAttribute<PFP::VEC3, VERTEX>("position");
		Algo::Modelisation::Primitive3D<PFP> prim(myMap, position);
		int nb = 8;
		prim.hexaGrid_topo(nb,nb,nb);
		prim.embedHexaGrid(1.0f,1.0f,1.0f);

		color = myMap.addAttribute<PFP::VEC3, VOLUME>("color");
		TraversorW<PFP::MAP> tra(myMap);
		for (Dart d = tra.begin(); d != tra.end(); d = tra.next())
			color[d] = position[d] + PFP::VEC3(0.5,0.5,0.5);
	}
    // un peu d'interface
	QApplication app(argc, argv);
	MyQT sqt;

	// interface de tuto5.ui
    Utils::QT::uiDockInterface dock;
    sqt.setDock(&dock);


	//  bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    sqt.m_WidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    sqt.m_PosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

    // envoit info BB a l'interface
	sqt.setParamObject(sqt.m_WidthObj, sqt.m_PosObj.data());

	sqt.setCallBack( dock.checkBox_volumes, SIGNAL(toggled(bool)), SLOT(volumes_onoff(bool)) );
	sqt.setCallBack( dock.checkBox_edges, SIGNAL(toggled(bool)), SLOT(edges_onoff(bool)) );
	sqt.setCallBack( dock.checkBox_topo, SIGNAL(toggled(bool)), SLOT(topo_onoff(bool)) );

	sqt.setCallBack( dock.checkBox_hide, SIGNAL(toggled(bool)), SLOT(hide_onoff(bool)) );
	sqt.setCallBack( dock.checkBox_plane, SIGNAL(toggled(bool)), SLOT(clipping_onoff(bool)) );
	sqt.setCallBack( dock.slider_explode, SIGNAL(valueChanged(int)), SLOT(slider_explode(int)) );

	sqt.setCallBack( dock.slider_explode, SIGNAL(sliderPressed()), SLOT(slider_pressed()) );
	sqt.setCallBack( dock.slider_explode, SIGNAL(sliderReleased()), SLOT(slider_released()) );

	sqt.setCallBack( dock.slider_explode_face, SIGNAL(valueChanged(int)), SLOT(slider_explodeF(int)) );

	sqt.setCallBack( dock.slider_explode_face, SIGNAL(sliderPressed()), SLOT(slider_pressed()) );
	sqt.setCallBack( dock.slider_explode_face, SIGNAL(sliderReleased()), SLOT(slider_released()) );



	sqt.show();
	dock.slider_explode->setValue(80);
	dock.slider_explode_face->setValue(80);
	sqt.clipping_onoff(true);

	Utils::Chrono ch;

	std::cout << "Compute Volume ->"<< std::endl;
	ch.start();
	float vol = Algo::Geometry::totalVolume<PFP>(myMap, position);
	vol += Algo::Geometry::totalVolume<PFP>(myMap, position);
	vol += Algo::Geometry::totalVolume<PFP>(myMap, position);
	vol += Algo::Geometry::totalVolume<PFP>(myMap, position);
	std::cout << ch.elapsed()<< " ms  val="<<vol<< std::endl;

	ch.start();
	vol = Algo::Geometry::Parallel::totalVolume<PFP>(myMap, position);
	vol += Algo::Geometry::Parallel::totalVolume<PFP>(myMap, position);
	vol += Algo::Geometry::Parallel::totalVolume<PFP>(myMap, position);
	vol += Algo::Geometry::Parallel::totalVolume<PFP>(myMap, position);
	std::cout << ch.elapsed()<< " ms //  val="<<vol<< std::endl;


	// et on attend la fin.
	return app.exec();
}
