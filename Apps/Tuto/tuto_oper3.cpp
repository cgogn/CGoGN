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

#include "tuto_oper3.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Modelisation/polyhedron.h"
#include "Algo/Modelisation/tetrahedralization.h"
#include "Algo/Modelisation/primitives3d.h"
#include "Algo/Geometry/centroid.h"
#include "Algo/Geometry/normal.h"
#include "Algo/Import/import.h"
#include "Algo/Export/export.h"
#include "Topology/generic/mapBrowser.h"

using namespace CGoGN ;


int main(int argc, char **argv)
{
	//	// interface
	QApplication app(argc, argv);
	MyQT sqt;

    sqt.setDock(& sqt.dock);
    sqt.setCallBack( sqt.dock.listOper, SIGNAL(currentRowChanged(int)), SLOT(operation(int)) );
    sqt.setCallBack( sqt.dock.svg, SIGNAL(clicked()), SLOT(svg()) );
    sqt.setCallBack( sqt.dock.widthSlider, SIGNAL(valueChanged(int)), SLOT(width(int)) );
	sqt.setCallBack( sqt.dock.checkBox_hide, SIGNAL(toggled(bool)), SLOT(hide_onoff(bool)) );
	sqt.setCallBack( sqt.dock.checkBox_plane, SIGNAL(toggled(bool)), SLOT(clipping_onoff(bool)) );
	int n=3;
	if (argc==2)
		n = atoi(argv[1]);

	// example code itself
	sqt.createMap(n);

	sqt.width(5);

	// set help message in menu
	sqt.setHelpMsg("First Tuto: \nCreate two faces\nsew them\nand affect positions");
	// final show for redraw
	sqt.show();
	// and wait for the end
	return app.exec();
}

void MyQT::clipping_onoff(bool x)
{
	clip_volume = x;

	if (clip_volume)
	{
		Geom::Vec3f pos = m_PlanePick->getPosition();
		float pipo;
		Geom::Vec3f normal = m_PlanePick->getAxisScale(2, pipo); // 2 = Z axis = plane normal
		m_sh1->setClipPlaneParamsAll(clip_id1, normal, pos);
		m_sh2->setClipPlaneParamsAll(clip_id2, normal, pos);
	}
	else
	{
		m_sh1->setClipPlaneParamsAll(clip_id1, Geom::Vec3f(0,0,1), Geom::Vec3f(0,0,999999.9f));
		m_sh2->setClipPlaneParamsAll(clip_id2, Geom::Vec3f(0,0,1), Geom::Vec3f(0,0,999999.9f));
		m_sh1->setClipColorAttenuationFactorRelative(0.0f,0.0f);
		m_sh2->setClipColorAttenuationFactorRelative(0.0f,0.0f);
	}
	updateMap();
	updateGL();
}

void MyQT::hide_onoff(bool x)
{
	hide_clipping = !hide_clipping;
	updateMap();
	updateGL();
}

void MyQT::operation(int x)
{
	switch(x)
	{
	case 0:
		CGoGNout <<"delete vertex"<<CGoGNendl;
		if (m_selected != NIL)
		{
			dm.markAll();
			m_selected=myMap.deleteVertex(m_selected);
			m_selected = NIL;
			updateMap();
		}
		break;
	case 1:
		CGoGNout <<"cut edge"<<CGoGNendl;
		if (m_selected != NIL)
		{
			dm.markAll();
			PFP::VEC3 Q = position[myMap.phi1(m_selected)];
			myMap.cutEdge(m_selected);
			position[myMap.phi1(m_selected)] = (position[m_selected] + Q)/2.0f;
			updateMap();
		}
		break;
	case 2:
		CGoGNout <<"uncut edge"<<CGoGNendl;
		if (m_selected != NIL)
		{
			dm.markAll();
			myMap.uncutEdge(m_selected);
			updateMap();
		}
		break;
	case 3:
		CGoGNout <<"delete edge"<<CGoGNendl;
		if (m_selected != NIL)
		{
			if (myMap.deleteEdgePreCond(m_selected))
			{
				dm.markAll();
				myMap.deleteEdge(m_selected);
				m_selected = NIL;
				updateMap();
			}
			else
			{
				CGoGNerr << "Could not delete this edge" << CGoGNendl;
			}
		}
		break;
	case 4:
		CGoGNout <<"collapse edge"<<CGoGNendl;
		if (m_selected != NIL)
		{
			PFP::VEC3 Q = (position[myMap.phi1(m_selected)] + position[m_selected])/2.0f;
			Dart x = myMap.collapseEdge(m_selected);
			dm.markAll();
			position[x]= Q;
			m_selected = NIL;
			updateMap();
		}
		break;
	case 5:
		CGoGNout <<"split face"<<CGoGNendl;
		if (m_selected != NIL)
		{
			myMap.splitFace(m_selected,m_selected2);
			dm.markAll();
			updateMap();
		}
		break;
	case 6:
		CGoGNout <<"merge volume"<<CGoGNendl;
		if (m_selected != NIL)
		{
			myMap.mergeVolumes(m_selected);
			dm.markAll();
			updateMap();
		}
		break;

	case 7:
		CGoGNout <<"split volume"<<CGoGNendl;
		if (!m_selecteds.empty())
		{
			std::cout << "start" << std::endl;
			for(std::vector<Dart>::iterator it = m_selecteds.begin() ; it != m_selecteds.end() ; ++it)
				std::cout << *it << " et phi2() = " << myMap.phi2(*it) <<  std::endl;
			std::cout << "end" << std::endl;

			myMap.splitVolume(m_selecteds);
			m_selecteds.clear();

			dm.markAll();
			updateMap();
		}
		break;
	case 8:
		CGoGNout <<"collapse face"<<CGoGNendl;
		if (m_selected != NIL)
		{
			PFP::VEC3 Q = Algo::Surface::Geometry::faceCentroid<PFP>(myMap,m_selected,position);
			Dart x = myMap.collapseFace(m_selected);
			dm.markAll();
			position[x]= Q;
			m_selected = NIL;
			updateMap();
		}
		break;
	case 9:
		CGoGNout <<"collapse volume"<<CGoGNendl;
		if (m_selected != NIL)
		{
			PFP::VEC3 Q = Algo::Surface::Geometry::volumeCentroid<PFP>(myMap,m_selected,position);
			Dart x = myMap.collapseVolume(m_selected);
			dm.markAll();
			position[x]= Q;
			m_selected = NIL;
			updateMap();
		}
		break;
	case 10:
		CGoGNout <<"split vertex"<<CGoGNendl;
		if (!m_selecteds.empty() && m_selected != NIL)
		{
			Dart dit = m_selecteds.front();
			PFP::VEC3 Q = (position[myMap.phi1(m_selected)] + position[m_selected])/2.0f;
			//PFP::VEC3 c1 = Algo::Geometry::volumeCentroid<PFP>(myMap, dit, position);
			Dart dres = myMap.splitVertex(m_selecteds);
			//Dart dres = Algo::Modelisation::Tetrahedralization::splitVertex<PFP>(myMap, m_selecteds);
			position[dres] = position[dit] + Q*0.25f;
			//position[dit] = position[dit] - c1*0.5f;
			m_selecteds.clear();
			m_selected = NIL;
			dm.markAll();
			updateMap();
			std::cout << "nb darts after = " << myMap.getNbDarts() << std::endl;
		}
		break;
	case 11 :
		CGoGNout <<"unsew volumes"<<CGoGNendl;
		if (m_selected != NIL)
		{
			myMap.unsewVolumes(m_selected);
			dm.markAll();
			m_selected = NIL;
			updateMap();
		}
		break;
	default:
		break;
	}

	updateGL();
	dock.listOper->setCurrentRow(-1);
}

void MyQT::createMap(int n)
{
	myMap.clear(true);
	position = myMap.getAttribute<VEC3, VERTEX>("position");
	if (!position.isValid())
		position = myMap.addAttribute<VEC3, VERTEX>("position");
//	Algo::Volume::Modelisation::Primitive3D<PFP> prim(myMap, position);
//	prim.hexaGrid_topo(n,n,n);
//	prim.embedHexaGrid(1.0f,1.0f,1.0f);

//	Algo::Surface::Modelisation::Polyhedron<PFP> poly(myMap, position);
//	poly.cylinder_topo(6,1,true,true);
//	poly.embedCylinder(6.0,6.0,5.0);
//	myMap.closeMap();

	Dart d = Algo::Surface::Modelisation::embedPrism<PFP>(myMap, position, 5, true,6.0,6.0,5.0);
	Dart d2 = Algo::Surface::Modelisation::embedPyramid<PFP>(myMap, position, 4, true,6.0,5.0);

	myMap.sewVolumes(myMap.phi2(d),d2);
	position[myMap.phi_1(myMap.phi2(d2))] += VEC3(9.0,-5.0,-2.5);

	myMap.check();

    //  bounding box of scene
	bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
	setParamObject(bb.maxSize(), bb.center().data()) ;
	m_shift = bb.maxSize()/200.0f;

	// first show for be sure that GL context is binded
	show();

	// render the topo of the map without boundary darts

	m_render_topo->setDartWidth(3.0f);
	m_render_topo->setInitialDartsColor(0.0f,0.0f,0.0f);
	m_render_topo->updateData<PFP>(myMap, position, m_ex1,m_ex2,m_ex3/*, nb*/);

//	SelectorDartBoundary<PFP::MAP> sdb(myMap);
//	MapBrowserSelector mbs(myMap,sdb);
//	myMap.setBrowser(&mbs);
//	m_render_topo_boundary->updateData<PFP>(myMap,position,m_ex1,m_ex2);
//	myMap.setBrowser(NULL);
	m_render_topo_boundary->updateDataBoundary<PFP>(myMap,position,m_ex1,m_ex2,m_shift);
}

void MyQT::updateMap()
{
	std::cout << "updateMap"<< std::endl;
	m_render_topo->updateData<PFP>(myMap, position, m_ex1,m_ex2,m_ex3/*, nb*/);
	m_render_topo_boundary->updateDataBoundary<PFP>(myMap,position,m_ex1,m_ex2,m_shift);

}

// initialization GL callback
void MyQT::cb_initGL()
{
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	m_render_topo = new Algo::Render::GL2::Topo3Render() ;

	m_render_topo_boundary = new Algo::Render::GL2::TopoRender();
	m_render_topo_boundary->setInitialDartsColor(0.0f,0.9f,0.0f);

    m_PlanePick = new Utils::Pickable(Utils::Pickable::GRID,1);
	m_frame = new Utils::FrameManipulator();
	m_frame->setSize(bb.maxSize());

	m_sh1 = static_cast<Utils::ClippingShader*>(m_render_topo->shader1());
	m_sh2 = static_cast<Utils::ClippingShader*>(m_render_topo->shader2());

	m_sh1->insertClippingCode();
	m_sh2->insertClippingCode();

	clip_id1 = m_sh1->addClipPlane();
	clip_id2 = m_sh2->addClipPlane();

	m_sh1->setClipPlaneParamsAll(clip_id1, Geom::Vec3f(0,0,1), bb.center());
	m_sh2->setClipPlaneParamsAll(clip_id2, Geom::Vec3f(0,0,1), bb.center());

}

// redraw GL callback (clear and swap already done)
void MyQT::cb_redraw()
{
	glEnable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( 1.0f, 1.0f );

	m_render_topo->drawTopo();

	m_render_topo_boundary->drawTopo();

	glDisable( GL_POLYGON_OFFSET_FILL );

	if (m_selected != NIL)
		m_render_topo->overdrawDart(m_selected, 11, 1.0f, 0.0f, 0.0f);

	if (m_selected2 != NIL)
		m_render_topo->overdrawDart(m_selected2, 11, 0.0f, 1.0f, 0.0f);

	for (std::vector<Dart>::iterator it = m_selecteds.begin(); it != m_selecteds.end(); ++it)
	{
		m_render_topo->overdrawDart(*it, 11, 0.0f, 0.0f, 1.0f);
	}

	if (clip_volume && !hide_clipping)
	{
		m_frame->draw();
		m_PlanePick->draw();
	}
}

void MyQT::cb_mousePress(int button, int x, int y)
{
	if (Shift())
	{
		Dart d = m_render_topo->picking<PFP>(myMap, x,y/*,nb*/);
		if (button == Qt::LeftButton)
		{
			if (d != Dart::nil())
				m_selected = d;
		}
		if (button == Qt::RightButton)
		{
			if (d != Dart::nil())
				m_selected2 = d;
		}
		updateGL();

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

	if (Control())
	{
//		Dart d = m_render_topo->picking<PFP>(myMap, x,y/*, nb*/);
//		if (button == Qt::LeftButton)
//		{
//			if (d != Dart::nil())
//				m_selecteds.push_back(d);
//		}
//		updateGL();


		if (button == Qt::LeftButton)
		{
			Dart d = m_render_topo_boundary->picking<PFP>(myMap, x,y,true);
			if (d != Dart::nil())
			{
				Dart e = myMap.phi2(d);
				std::cout << "Dart "<< d.index << " / phi2:" << e.index << std::endl;
			}
		}
		if (button == Qt::RightButton)
		{
			Dart d = m_render_topo->picking<PFP>(myMap, x,y/*, nb*/);
			if (d != Dart::nil())
			{
				Dart e = myMap.phi2(d);
				std::cout << "Dart "<< d.index << " / phi2:" << e.index << std::endl;
			}
		}


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

	m_sh1->setClipPlaneParamsAll(clip_id1, normal, pos);
	m_sh2->setClipPlaneParamsAll(clip_id2, normal, pos);

	m_begX = x;
	m_begY = y;
	updateGL();
	return;

}

void MyQT::cb_keyPress(int keycode)
{
	switch(keycode)
	{

	case ' ':
		m_selecteds.clear();
		m_selected=NIL;
		m_selected2=NIL;
		updateGL();
		break;
	case '2':
		createMap(2);
		updateMap();
		updateGL();
		break;
	case '3':
		createMap(3);
		updateMap();
		updateGL();
		break;
	case '4':
		createMap(4);
		updateMap();
		updateGL();
		break;
	case 'c':
		myMap.check();
		break;
	case 'a':
		m_selected = myMap.phi1(m_selected);
		updateGL();
		break;
	case 'z':
		m_selected = myMap.phi_1(m_selected);
		updateGL();
		break;
	case 'e':
		m_selected = myMap.phi2(m_selected);
		updateGL();
		break;
	case 'r':
		m_selected = myMap.phi3(m_selected);
		updateGL();
		break;
	case 'q':
		m_selected2 = myMap.phi1(m_selected2);
		updateGL();
		break;
	case 's':
		m_selected2 = myMap.phi_1(m_selected2);
		updateGL();
		break;
	case 'd':
		m_selected2 = myMap.phi2(m_selected2);
		updateGL();
		break;
	case 'f':
		m_selected2 = myMap.phi3(m_selected2);
		updateGL();
		break;
	case 'w':
		m_ex1 = 0.99f;
		m_ex2 = 0.99f;
		m_render_topo->updateData<PFP>(myMap, position, m_ex1,m_ex2,m_ex3/*, nb*/);
		updateGL();
		break;
	case 'W':
		m_ex1 = 0.95f;
		m_ex2 = 0.95f;
		m_render_topo->updateData<PFP>(myMap, position, m_ex1,m_ex2,m_ex3/*, nb*/);
		updateGL();
		break;

//	case 'c':
//		for (Dart d=myMap.begin(); d!=myMap.end(); myMap.next(d))
//		{
//			if (!myMap.isBoundaryMarked3(d))
//			{
//				int n = random();
//				float r = float(n&0x7f)/255.0f + 0.25f;
//				float g = float((n>>8)&0x7f)/255.0f + 0.25f;
//				float b = float((n>>16)&0x7f)/255.0 + 0.25f;
//				colorDarts[d] =  Geom::Vec3f(r,g,b);
//				m_render_topo->setDartColor(d,r,g,b);
//			}
//		}
//		break;
//	case 'g':
//		for (Dart d=myMap.begin(); d!=myMap.end(); myMap.next(d))
//		{
//			if (!myMap.isBoundaryMarked3(d))
//			{
//				colorDarts[d] =  Geom::Vec3f(0.5f,0.5f,0.5f);
//				m_render_topo->setDartColor(d,0.5f,0.5f,0.5f);
//			}
//		}
//		break;
//
//	case 'b':
//		for (Dart d=myMap.begin(); d!=myMap.end(); myMap.next(d))
//		{
//			if (!myMap.isBoundaryMarked3(d))
//			{
//				colorDarts[d] =  Geom::Vec3f(0.0f,0.0f,0.0f);
//				m_render_topo->setDartColor(d,0.0f,0.0f,0.0f);
//			}
//		}
//		break;
	case Qt::Key_Up:
		if (m_selected!=NIL)
			position[m_selected][1] += m_shift;
		updateMap();
		updateGL();
		break;
	case Qt::Key_Down:
		if (m_selected!=NIL)
			position[m_selected][1] -= m_shift;
		updateMap();
		updateGL();
		break;
	case Qt::Key_Left:
		if (m_selected!=NIL)
			position[m_selected][0] -= m_shift;
		updateMap();
		updateGL();
		break;
	case Qt::Key_Right:
		if (m_selected!=NIL)
			position[m_selected][0] += m_shift;
		updateMap();
		updateGL();
		break;
	}
	updateGL();
}

void MyQT::svg()
{
	if (m_selected != NIL)
		m_render_topo->setDartColor(m_selected, 0.8f, 0.0f, 0.0f);
	if (m_selected2 != NIL)
		m_render_topo->setDartColor(m_selected2, 0.0f, 0.8f, 0.0f);

	std::string filename = selectFileSave("snapshot file", ".", "(*.svg)");
	m_render_topo->svgout2D(filename, modelViewMatrix(), projectionMatrix());
}

void MyQT::cb_Open()
{
	std::string filters("all (*.*);; map (*.map)") ;
	std::string filename = selectFile("Open Mesh", "", filters) ;
	if (!filename.empty())
		importMesh(filename);
	updateGL();
}

void MyQT::cb_Save()
{
	std::string filename = selectFileSave("Export MAP file ",".","(*.map)");
	//Algo::Surface::Export::exportOFF<PFP>(myMap, position, filename.c_str()); // ???
	if(!myMap.saveMapBin(filename))
		std::cout << "could not save file : " << filename << std::endl;
}

void MyQT::importMesh(std::string& filename)
{
	myMap.clear(true) ;

	size_t pos = filename.rfind(".");    // position of "." in filename
	std::string extension = filename.substr(pos);
	std::vector<std::string> attrNames ;

	if (extension == std::string(".map"))
	{
		myMap.loadMapBin(filename);
		position = myMap.getAttribute<VEC3, VERTEX>("position") ;
	}
	else if(!Algo::Volume::Import::importMesh<PFP>(myMap, filename, attrNames))
	{
		std::cerr << "could not import " << filename << std::endl ;
		return ;
	}
	else
		position = myMap.getAttribute<PFP::VEC3,VERTEX>(attrNames[0]) ;

	m_selected  = NIL;
	m_selected2 = NIL;

	m_render_topo->updateData<PFP>(myMap, position, m_ex1,m_ex2,m_ex3/*, nb*/);

//	SelectorDartBoundary<PFP::MAP> sdb(myMap);
//	MapBrowserSelector mbs(myMap,sdb);
//	myMap.setBrowser(&mbs);
//	m_render_topo_boundary->updateData<PFP>(myMap,position,m_ex1,m_ex2);
//	myMap.setBrowser(NULL);
	m_render_topo_boundary->updateDataBoundary<PFP>(myMap,position,m_ex1,m_ex2,m_shift);

	bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
	setParamObject(bb.maxSize(), bb.center().data()) ;
	m_shift = bb.maxSize()/200.0f;

	m_frame->setSize(bb.maxSize());

	updateMap();
	updateGLMatrices() ;
}

void MyQT::width(int w)
{
	m_ex3 = 0.9f - 0.025f*w;
	m_render_topo->updateData<PFP>(myMap, position, m_ex1,m_ex2,m_ex3/*, nb*/);
	updateGL();
}
