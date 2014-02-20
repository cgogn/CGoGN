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

#include "test_traversor2.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Tiling/Surface/square.h"
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
	sqt.setCallBack( sqt.dock.listTravers, SIGNAL(currentRowChanged(int)), SLOT(traversors(int)) );
	sqt.setCallBack( sqt.dock.withBoundary, SIGNAL(clicked()), SLOT(updateMap()) );
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


void MyQT::traversors(int x)
{
	// update all color to grey
	for (Dart d = myMap.begin(); d != myMap.end(); myMap.next(d))
	{
		colorDarts[d] =  Geom::Vec3f(0.5f,0.5f,0.5f);
		m_render_topo->setDartColor(d,0.5f,0.5f,0.5f);
	}

	switch(x)
	{
	case 0:
	{
		Traversor2VVaE<MAP> trav(myMap,m_selected);
		for(Dart b=trav.begin(); b!= trav.end(); b=trav.next())
			m_render_topo->setDartColor(b,0.0f,1.0f,0.0f);
	}
		break;
	case 1:
	{
		Traversor2VVaF<MAP> trav(myMap,m_selected);
		for(Dart b=trav.begin(); b!= trav.end(); b=trav.next())
			m_render_topo->setDartColor(b,0.0f,1.0f,0.0f);
	}
		break;
	case 2:
	{
		Traversor2EEaV<MAP> trav(myMap,m_selected);
		for(Dart b=trav.begin(); b!= trav.end(); b=trav.next())
			m_render_topo->setDartColor(b,0.0f,1.0f,0.0f);
	}
		break;
	case 3:
	{
		Traversor2EEaF<MAP> trav(myMap,m_selected);
		float c=0.0f;
		for(Dart b=trav.begin(); b!= trav.end(); b=trav.next())
			m_render_topo->setDartColor(b,0.0f,1.0f,c);
	}
		break;
	case 4:
	{
		Traversor2FFaV<MAP> trav(myMap,m_selected);
		for(Dart b=trav.begin(); b!= trav.end(); b=trav.next())
			m_render_topo->setDartColor(b,0.0f,1.0f,0.0f);
	}
		break;
	case 5:
	{
		Traversor2FFaE<MAP> trav(myMap,m_selected);
		for(Dart b=trav.begin(); b!= trav.end(); b=trav.next())
			m_render_topo->setDartColor(b,0.0f,1.0f,0.0f);
	}
		break;
	case 6:
	{
		Traversor2FV<MAP> trav(myMap,m_selected);
		for(Dart b=trav.begin(); b!= trav.end(); b=trav.next())
			m_render_topo->setDartColor(b,0.0f,1.0f,0.0f);
	}
		break;
	case 7:
	{
		Traversor2FE<MAP> trav(myMap,m_selected);
		for(Dart b=trav.begin(); b!= trav.end(); b=trav.next())
			m_render_topo->setDartColor(b,0.0f,1.0f,0.0f);
	}
		break;
	case 8:
	{
		Traversor2EV<MAP> trav(myMap,m_selected);
		for(Dart b=trav.begin(); b!= trav.end(); b=trav.next())
			m_render_topo->setDartColor(b,0.0f,1.0f,0.0f);
	}
		break;
	case 9:
	{
		Traversor2EF<MAP> trav(myMap,m_selected);
		for(Dart b=trav.begin(); b!= trav.end(); b=trav.next())
			m_render_topo->setDartColor(b,0.0f,1.0f,0.0f);
	}
		break;
	case 10:
	{
		Traversor2VE<MAP> trav(myMap,m_selected);
		for(Dart b=trav.begin(); b!= trav.end(); b=trav.next())
			m_render_topo->setDartColor(b,0.0f,1.0f,0.0f);
	}
		break;
	case 11:
	{
		Traversor2VF<MAP> trav(myMap,m_selected);
		for(Dart b=trav.begin(); b!= trav.end(); b=trav.next())
			m_render_topo->setDartColor(b,0.0f,1.0f,0.0f);
	}
		break;
	default:
		break;
	}

	updateGL();
}

void MyQT::createMap(int n)
{
	position = myMap.addAttribute<VEC3, VERTEX>("position");
	colorDarts = myMap.addAttribute<VEC3, DART>("color");

    Algo::Surface::Tilings::Square::Grid<PFP> grid(myMap, n, n, true);
    grid.embedIntoGrid(position, 1.,1.,0.);

    //  bounding box of scene
	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
	setParamObject(bb.maxSize(), bb.center().data()) ;
	m_shift = bb.maxSize()/200.0f;

	// first show for be sure that GL context is binded
	show();

	dm.markAll();

	m_render_topo->setDartWidth(4.0f);
	m_render_topo->setInitialDartsColor(0.5f,0.5f,0.5f);
	m_render_topo->setInitialBoundaryDartsColor(0.3f,0.3f,0.3f);

	m_render_topo->updateData<PFP>(myMap, position, 0.9f, 0.9f, dock.withBoundary->isChecked());

	for (Dart d=myMap.begin(); d!=myMap.end(); myMap.next(d))
	{
		if (dm.isMarked(d) && (!myMap.isBoundaryMarked2(d)))
		{
			colorDarts[d] =  Geom::Vec3f(0.5f,0.5f,0.5f);
			m_render_topo->setDartColor(d,0.5f,0.5f,0.5f);
		}
	}
}

void MyQT::updateMap()
{
	m_render_topo->setInitialBoundaryDartsColor(0.0f,0.0f,0.0f);
	m_render_topo->updateData<PFP>(myMap, position, 0.9f, 0.9f, dock.withBoundary->isChecked());
	for (Dart d=myMap.begin(); d!=myMap.end(); myMap.next(d))
	{
		if (dm.isMarked(d) && (!myMap.isBoundaryMarked2(d)))
		{
			const Geom::Vec3f& C = colorDarts[d];
			if (C*C != 0.0f)
				m_render_topo->setDartColor(d,C[0],C[1],C[2]);
		}
	}
	updateGL();
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

			if (d != Dart::nil())
				std::cout << "DART="<<d.index << std::endl;


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
			colorDarts[d] =  Geom::Vec3f(0.5f,0.5f,0.5f);
			m_render_topo->setDartColor(d,0.5f,0.5f,0.5f);
		}
		break;

	case 'h':
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

	default:
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

