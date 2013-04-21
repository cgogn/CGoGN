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

#include "tuto_oper2.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Modelisation/polyhedron.h"
#include "Algo/Geometry/centroid.h"
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
		CGoGNout <<"split vertex"<<CGoGNendl;
		if ((m_selected != NIL) && (m_selected2 != NIL))
		{
			dm.markAll();
			myMap.splitVertex(m_selected,m_selected2);
			PFP::VEC3 c1 = Algo::Surface::Geometry::faceCentroid<PFP>(myMap, m_selected, position);
			PFP::VEC3 c2 = Algo::Surface::Geometry::faceCentroid<PFP>(myMap, m_selected2, position);
			position[m_selected] = position[m_selected] * 0.7f + c1*0.3f;
			position[m_selected2] = position[m_selected2] * 0.7f + c2*0.3f;
			updateMap();
		}
		break;
	case 1:
		CGoGNout <<"delete vertex"<<CGoGNendl;
		if (m_selected != NIL)
		{
			dm.markAll();
			m_selected=myMap.deleteVertex(m_selected);
			updateMap();
		}
		break;
	case 2:
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
	case 3:
		CGoGNout <<"uncut edge"<<CGoGNendl;
		if (m_selected != NIL)
		{
			myMap.uncutEdge(m_selected);
			updateMap();
		}
		break;
	case 4:
		CGoGNout <<"collapse edge"<<CGoGNendl;
		if (m_selected != NIL)
		{
			PFP::VEC3 M = (position[m_selected] + position[myMap.phi1(m_selected)])/2.0f;
			m_selected = myMap.collapseEdge(m_selected,true);
			position[m_selected] = M;
			updateMap();
		}
		break;
	case 5:
		CGoGNout <<"flip edge"<<CGoGNendl;
		if (m_selected != NIL)
		{
			myMap.flipEdge(m_selected);
			updateMap();
		}
		break;
	case 6:
		CGoGNout <<"flip back edge"<<CGoGNendl;
		if (m_selected != NIL)
		{
			myMap.flipBackEdge(m_selected);
			updateMap();
		}
		break;
	case 7:
		CGoGNout <<"split face"<<CGoGNendl;
		if (m_selected != NIL)
		{
			myMap.splitFace(m_selected,m_selected2);
			updateMap();
		}
		break;
	case 8:
		CGoGNout <<"merge face"<<CGoGNendl;
		if (m_selected != NIL)
		{
			myMap.mergeFaces(m_selected);
			updateMap();
			m_selected=NIL;
		}
		break;
	case 9:
		CGoGNout <<"delete face"<<CGoGNendl;
		if (m_selected != NIL)
		{
			myMap.deleteFace(m_selected);
			updateMap();
			m_selected=NIL;
			m_selected2=NIL;
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
	position = myMap.addAttribute<VEC3, VERTEX>("position");
	colorDarts = myMap.addAttribute<VEC3, DART>("color");

	Algo::Surface::Modelisation::Polyhedron<PFP> grid(myMap,position);
	grid.grid_topo(n,n);
	grid.embedGrid(1.,1.,0.);

    //  bounding box of scene
	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
	setParamObject(bb.maxSize(), bb.center().data()) ;
	m_shift = bb.maxSize()/200.0f;

	// first show for be sure that GL context is binded
	show();

	// render the topo of the map without boundary darts

	dm.markAll();

	m_render_topo->setDartWidth(4.0f);
	m_render_topo->setInitialDartsColor(0.0f,0.0f,0.0f);
	m_render_topo->setInitialBoundaryDartsColor(0.0f,0.0f,0.0f);
	m_render_topo->updateData<PFP>(myMap, position, 0.9f, 0.9f,true); // nb

	for (Dart d=myMap.begin(); d!=myMap.end(); myMap.next(d))
	{
		if (dm.isMarked(d) && (!myMap.isBoundaryMarked2(d)))
		{
			int n = rand();
			float r = float(n&0x7f)/255.0f + 0.25f;
			float g = float((n>>8)&0x7f)/255.0f + 0.25f;
			float b = float((n>>16)&0x7f)/255.0 + 0.25f;
			colorDarts[d] =  Geom::Vec3f(r,g,b);
			m_render_topo->setDartColor(d,r,g,b);
		}
	}
}

void MyQT::updateMap()
{
	m_render_topo->setInitialBoundaryDartsColor(0.0f,0.0f,0.0f);
	m_render_topo->updateData<PFP>(myMap, position, 0.9f, 0.9f,true); // nb
	for (Dart d=myMap.begin(); d!=myMap.end(); myMap.next(d))
	{
		if (dm.isMarked(d) && (!myMap.isBoundaryMarked2(d)))
		{
			const Geom::Vec3f& C = colorDarts[d];
			if (C*C != 0.0f)
				m_render_topo->setDartColor(d,C[0],C[1],C[2]);
		}
	}
}

// initialization GL callback
void MyQT::cb_initGL()
{
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	m_render_topo = new Algo::Render::GL2::TopoRender(0.01f) ;
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
}

void MyQT::cb_mousePress(int button, int x, int y)
{
	if (Shift())
	{
		Dart d = m_render_topo->picking<PFP>(myMap, x,y); // nb
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
}

void MyQT::cb_keyPress(int keycode)
{
	switch(keycode)
	{
	case 'c':
		for (Dart d = myMap.begin(); d != myMap.end(); myMap.next(d))
		{
			if (!myMap.isBoundaryMarked2(d))
			{
				int n = rand();
				float r = float(n&0x7f)/255.0f + 0.25f;
				float g = float((n>>8)&0x7f)/255.0f + 0.25f;
				float b = float((n>>16)&0x7f)/255.0 + 0.25f;
				colorDarts[d] =  Geom::Vec3f(r,g,b);
				m_render_topo->setDartColor(d,r,g,b);
			}
		}
		break;
	case 'g':
		for (Dart d = myMap.begin(); d != myMap.end(); myMap.next(d))
		{
			if (!myMap.isBoundaryMarked2(d))
			{
				colorDarts[d] =  Geom::Vec3f(0.5f,0.5f,0.5f);
				m_render_topo->setDartColor(d,0.5f,0.5f,0.5f);
			}
		}
		break;

	case 'b':
		for (Dart d = myMap.begin(); d != myMap.end(); myMap.next(d))
		{
			if (!myMap.isBoundaryMarked2(d))
			{
				colorDarts[d] =  Geom::Vec3f(0.0f,0.0f,0.0f);
				m_render_topo->setDartColor(d,0.0f,0.0f,0.0f);
			}
		}
		break;
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
	if (m_selected!=NIL)
		m_render_topo->setDartColor(m_selected,0.8f,0.0f,0.0f);
	if (m_selected2!=NIL)
		m_render_topo->setDartColor(m_selected2,0.0f,0.8f,0.0f);

	std::string filename = selectFileSave("snapshot file", ".", "(*.svg)");
	m_render_topo->svgout2D(filename, modelViewMatrix(),projectionMatrix());
}

void MyQT::cb_Open()
{
	std::string filters("all (*.*);; trian (*.trian);; off (*.off);; ply (*.ply);; map (*.map)") ;
	std::string filename = selectFile("Open Mesh", "", filters) ;
	if (!filename.empty())
		importMesh(filename);
	updateGL();
}

void MyQT::cb_Save()
{
	std::string filename = selectFileSave("Export Off file ",".","(*.off)");
	Algo::Surface::Export::exportOFF<PFP>(myMap,position,filename.c_str());
	//std::cout << "could not save file : " << filename << std::endl;
}

void MyQT::importMesh(std::string& filename)
{
	myMap.clear(true) ;

	size_t pos = filename.rfind(".");    // position of "." in filename
	std::string extension = filename.substr(pos);

	if (extension == std::string(".map"))
	{
		myMap.loadMapBin(filename);
		position = myMap.getAttribute<VEC3, VERTEX>("position") ;
	}
	else
	{
		std::vector<std::string> attrNames ;
		if(!Algo::Surface::Import::importMesh<PFP>(myMap, filename.c_str(), attrNames))
		{
			CGoGNerr << "could not import " << filename << CGoGNendl ;
			return;
		}
		position = myMap.getAttribute<VEC3, VERTEX>(attrNames[0]) ;
	}

	colorDarts = myMap.getAttribute<VEC3, DART>("color");
	if (!colorDarts.isValid())
	{
		colorDarts = myMap.addAttribute<VEC3, DART>("color");
		for (Dart d=myMap.begin(); d!=myMap.end(); myMap.next(d))
		{
			if (dm.isMarked(d) && (!myMap.isBoundaryMarked2(d)))
			{
				int n = rand();
				float r = float(n&0x7f)/255.0f + 0.25f;
				float g = float((n>>8)&0x7f)/255.0f + 0.25f;
				float b = float((n>>16)&0x7f)/255.0 + 0.25f;
				colorDarts[d] =  Geom::Vec3f(r,g,b);
				m_render_topo->setDartColor(d,r,g,b);
			}
		}
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
	m_render_topo->setDartWidth(w);
	m_render_topo->setRelationWidth(w);
	updateGL();
}

