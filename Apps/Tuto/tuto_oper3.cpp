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
#include "Algo/Modelisation/primitives3d.h"
#include "Algo/Geometry/centroid.h"
#include "Algo/Geometry/normal.h"
#include "Algo/Import/import.h"
#include "Algo/Export/export.h"

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
			PFP::VEC3 Q = Algo::Geometry::faceCentroid<PFP>(myMap,m_selected,position);
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
			PFP::VEC3 Q = Algo::Geometry::volumeCentroid<PFP>(myMap,m_selected,position);
			Dart x = myMap.collapseVolume(m_selected);
			dm.markAll();
			position[x]= Q;
			m_selected = NIL;
			updateMap();
		}
		break;
	case 10:
		CGoGNout <<"split vertex"<<CGoGNendl;
		if (!m_selecteds.empty())
		{
			std::cout << "nb darts before = " << myMap.getNbDarts() << std::endl;
			Dart dit = m_selecteds.front();
			PFP::VEC3 c1 = Algo::Geometry::volumeCentroid<PFP>(myMap, dit, position);
			Dart dres = myMap.splitVertex(m_selecteds);
			position[dres] = position[dit]*0.5f - c1*0.5f;
			m_selecteds.clear();
			dm.markAll();
			updateMap();
			std::cout << "nb darts after = " << myMap.getNbDarts() << std::endl;
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
	position = myMap.getAttribute<PFP::VEC3>(VERTEX, "position");
	if (!position.isValid())
		position = myMap.addAttribute<PFP::VEC3>(VERTEX, "position");
	Algo::Modelisation::Primitive3D<PFP> prim(myMap, position);
	prim.hexaGrid_topo(n,n,n);
	prim.embedHexaGrid(1.0f,1.0f,1.0f);

    //  bounding box of scene
	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
	setParamObject(bb.maxSize(), bb.center().data()) ;
	m_shift = bb.maxSize()/200.0f;

	// first show for be sure that GL context is binded
	show();

	// render the topo of the map without boundary darts

	m_render_topo->setDartWidth(3.0f);
	m_render_topo->setInitialDartsColor(0.0f,0.0f,0.0f);
	m_render_topo->updateData<PFP>(myMap, position, m_ex1,m_ex2,m_ex3, nb);


}



void MyQT::updateMap()
{
	m_render_topo->updateData<PFP>(myMap, position, m_ex1,m_ex2,m_ex3, nb);
}

// initialization GL callback
void MyQT::cb_initGL()
{
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	m_render_topo = new Algo::Render::GL2::Topo3Render() ;
}

// redraw GL callback (clear and swap already done)
void MyQT::cb_redraw()
{
	glEnable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( 1.0f, 1.0f );

	m_render_topo->drawTopo();

	glDisable( GL_POLYGON_OFFSET_FILL );

	if (m_selected != NIL)
		m_render_topo->overdrawDart(m_selected, 11, 1.0f, 0.0f, 0.0f);

	if (m_selected2 != NIL)
		m_render_topo->overdrawDart(m_selected2, 11, 0.0f, 1.0f, 0.0f);

	for (std::vector<Dart>::iterator it = m_selecteds.begin(); it != m_selecteds.end(); ++it)
	{
		m_render_topo->overdrawDart(*it, 11, 0.0f, 0.0f, 1.0f);
	}
}

void MyQT::cb_mousePress(int button, int x, int y)
{
	if (Shift())
	{
		Dart d = m_render_topo->picking<PFP>(myMap, x,y, nb);
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
	}

	if (Control())
	{
		Dart d = m_render_topo->picking<PFP>(myMap, x,y, nb);
		if (button == Qt::LeftButton)
		{
			if (d != Dart::nil())
				m_selecteds.push_back(d);
		}
		updateGL();
	}

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
		m_render_topo->updateData<PFP>(myMap, position, m_ex1,m_ex2,m_ex3, nb);
		updateGL();
		break;
	case 'W':
		m_ex1 = 0.9f;
		m_ex2 = 0.9f;
		m_render_topo->updateData<PFP>(myMap, position, m_ex1,m_ex2,m_ex3, nb);
		updateGL();
		break;

//	case 'c':
//		for (Dart d=myMap.begin(); d!=myMap.end(); myMap.next(d))
//		{
//			if (!myMap.isBoundaryMarked(d))
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
//			if (!myMap.isBoundaryMarked(d))
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
//			if (!myMap.isBoundaryMarked(d))
//			{
//				colorDarts[d] =  Geom::Vec3f(0.0f,0.0f,0.0f);
//				m_render_topo->setDartColor(d,0.0f,0.0f,0.0f);
//			}
//		}
//		break;
	case Qt::Key_Up:
		if (m_selected!=NIL)
			position[m_selected][1] +=m_shift;
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
	if (m_selected!=NIL)
		m_render_topo->setDartColor(m_selected,0.8f,0.0f,0.0f);
	if (m_selected2!=NIL)
		m_render_topo->setDartColor(m_selected2,0.0f,0.8f,0.0f);

	std::string filename = selectFileSave("snapshot file", ".", "(*.svg)");
	m_render_topo->svgout2D(filename, modelViewMatrix(),projectionMatrix());
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
	std::string filename = selectFileSave("Export SVG file ",".","(*.off)");
	Algo::Export::exportOFF<PFP>(myMap,position,filename.c_str());
}

void MyQT::importMesh(std::string& filename)
{
	myMap.clear(true) ;

	size_t pos = filename.rfind(".");    // position of "." in filename
	std::string extension = filename.substr(pos);

	if (extension == std::string(".map"))
	{
		myMap.loadMapBin(filename);
		position = myMap.getAttribute<PFP::VEC3>(VERTEX, "position") ;
	}
	else if (extension == std::string(".node"))
	{
		std::vector<std::string> attrNames ;
		if(!Algo::Import::importMeshV<PFP>(myMap, filename, attrNames, Algo::Import::ImportVolumique::NODE))
		{
			std::cerr << "could not import " << filename << std::endl ;
			return ;
		}
		position = myMap.getAttribute<PFP::VEC3>(VERTEX, attrNames[0]) ;
	}
	else if(extension == std::string(".tet"))
	{
		std::vector<std::string> attrNames ;
		if(!Algo::Import::importMeshV<PFP>(myMap, filename, attrNames, Algo::Import::ImportVolumique::TET))
		{
			std::cerr << "could not import " << filename << std::endl ;
			return ;
		}
		position = myMap.getAttribute<PFP::VEC3>(VERTEX, attrNames[0]) ;
	}
	else if(extension == std::string(".off"))
	{
		std::vector<std::string> attrNames ;
		if(!Algo::Import::importMeshV<PFP>(myMap, filename, attrNames, Algo::Import::ImportVolumique::OFF))
		{
			std::cerr << "could not import " << filename << std::endl ;
			return ;
		}
		position = myMap.getAttribute<PFP::VEC3>(VERTEX, attrNames[0]) ;
	}
	else
	{
		std::cerr << "could not import " << filename << std::endl ;
	}

	m_selected  = NIL;
	m_selected2 = NIL;

	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
	setParamObject(bb.maxSize(), bb.center().data()) ;
	m_shift = bb.maxSize()/200.0f;

	updateMap();
	updateGLMatrices() ;
}


void MyQT::width(int w)
{
	m_ex3 = 0.9f - 0.025f*w;
	m_render_topo->updateData<PFP>(myMap, position, m_ex1,m_ex2,m_ex3, nb);
	updateGL();
}

