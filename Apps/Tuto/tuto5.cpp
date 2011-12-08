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

#include "tuto5.h"

//void Tuto5::balls_onoff(bool x)
//{
//	render_balls = !render_balls;
//	updateGL();
//	CGoGNerr << " balls_onoff  "<< CGoGNendl;
//}
//
//void Tuto5::vectors_onoff(bool x)
//{
//	render_vectors = !render_vectors;
//	updateGL();
//	CGoGNerr << " vectors_onoff  "<< CGoGNflush;
//}
//
//void Tuto5::text_onoff(bool x)
//{
//	render_text = !render_text;
//	updateGL();
//	CGoGNerr << " text_onoff  " << CGoGNflush;
//}

void Tuto5::topo_onoff(bool x)
{
	render_topo = !render_topo;
	updateGL();
	CGoGNerr << " topo_onoff  " << CGoGNflush;
}

//void Tuto5::slider_balls(int x)
//{
//	m_sprite->setSize(0.05f*x);
//	updateGL();
//}
//
//void Tuto5::slider_vectors(int x)
//{
//	m_lines->setScale(0.02*x);
//	updateGL();
//}
//
//void Tuto5::slider_text(int x)
//{
//	m_strings->setScale(0.02f*x);
//	updateGL();
//}

void Tuto5::animate()
{
//	transfoMatrix() = glm::rotate(transfoMatrix(), 0.5f, glm::vec3(0.5773f,0.5773f,0.5773f));
	transfoRotate( 0.5f, 0.5773f,0.5773f,0.5773f);
	updateGLMatrices();
}

//void Tuto5::storeVerticesInfo()
//{
//
//	CellMarker mv(myMap,VERTEX);
//	for (Dart d = myMap.begin(); d != myMap.end(); myMap.next(d))
//	{
//		if (!mv.isMarked(d))
//		{
//			mv.mark(d);
//			std::stringstream ss;
//			ss << d << " : "<< position[d];
//			m_strings->addString(ss.str(),position[d]);
//		}
//	}
//}

void Tuto5::cb_initGL()
{
//	Utils::GLSLShader::setCurrentOGLVersion(2);
	Utils::GLSLShader::setCurrentOGLVersion(1) ;

//	// create the render
//	m_render = new Algo::Render::GL2::MapRender();
//
//    m_render_topo = new Algo::Render::GL2::Topo3RenderMapD();
//
// 	// create VBO for position
//	m_positionVBO = new Utils::VBO();
//	m_positionVBO->updateData(position);
//
//	// using simple shader with color
//	m_shader = new Utils::ShaderSimpleColor();
//	m_shader->setAttributePosition(m_positionVBO);
//	m_shader->setColor(Geom::Vec4f(0.,1.,0.,0.));
//
//	m_sprite = new Utils::PointSprite();
//	m_sprite->setAttributePosition(m_positionVBO);
//
//	m_strings = new Utils::Strings3D(true, Geom::Vec3f(0.1f,0.0f,0.3f));
//	storeVerticesInfo();
//	m_strings->sendToVBO();
//
//	// copy de contenu de VBO a la creation
//	m_dataVBO = new Utils::VBO(*m_positionVBO);
//
//	m_lines = new Utils::ShaderVectorPerVertex();
//	m_lines->setAttributePosition(m_positionVBO);
//	m_lines->setAttributeVector(m_dataVBO);
//	m_lines->setScale(0.2f);
//	m_lines->setColor(Geom::Vec4f(0.0f, 1.0f, 0.2f, 0.0f));
//
//	// accede au buffer du VBO pour modification
//	PFP::VEC3* data = static_cast<PFP::VEC3*>(m_dataVBO->lockPtr());
//	for (unsigned int i=0; i< m_dataVBO->nbElts(); ++i)
//	{
//		data[i].normalize();
//	}
//	m_dataVBO->releasePtr();
//
//	registerShader(m_shader);
//	registerShader(m_strings);
//	registerShader(m_sprite);
//	registerShader(m_lines);
//
//	m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::TRIANGLES);
//	m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::LINES);
//	m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::POINTS);
//
//	m_render_topo->updateData<PFP>(myMap, allDarts, position,  0.9f, 0.9f, 0.9f);

	// timer example for animation
	m_timer = new QTimer( this );
	connect( m_timer, SIGNAL(timeout()), SLOT(animate()) );
}

void Tuto5::cb_redraw()
{
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	glEnable(GL_LIGHTING);
//
//	m_render->draw(m_shader, Algo::Render::GL2::POINTS);
//
//	glLineWidth(2.0f);
//	m_shader->setColor(Geom::Vec4f(1.,1.,0.,0.));
//	m_render->draw(m_shader, Algo::Render::GL2::LINES);
//
//	glEnable(GL_POLYGON_OFFSET_FILL);
//	glPolygonOffset(1.0f, 1.0f);

	if (render_topo)
	{
		glDisable(GL_LIGHTING);
		glLineWidth(1.0f);
//		Algo::Render::GL1::renderTopoGMD3<PFP>(m, position, true, true, true, true, 0.9f, 0.9f, 0.9f, 0.9f);
		Algo::Render::GL1::renderTopoMD3<PFP>(m, position, true, true, true, 0.9f, 0.9f, 0.9f);
//		m_render_topo->drawTopo();
	}

//	Dart d = myMap.phi2(myMap.begin());
//	m_render_topo->overdrawDart(d, 5, 1.0f, 0.0f, 1.0f);
//	 d = myMap.phi1(myMap.begin());
//	m_render_topo->overdrawDart(d, 5, 1.0f, 0.0f, 1.0f);
//
//	glDisable(GL_POLYGON_OFFSET_FILL);
//
//	if (render_text)
//		m_strings->drawAll(Geom::Vec3f(0.0f, 1.0f, 1.0f));
//
//	if (render_balls)
//	{
//		m_sprite->predraw(Geom::Vec3f(1.0f, 0.0f ,0.0f));
//		m_render->draw(m_sprite, Algo::Render::GL2::POINTS);
//		m_sprite->postdraw();
//	}
//
//	if (render_vectors)
//	{
//		glLineWidth(1.0f);
//		m_render->draw(m_lines, Algo::Render::GL2::POINTS);
//	}
}

void Tuto5::cb_mousePress(int button, int x, int y)
{
//	if (Shift())
//	{
//		Dart d = m_render_topo->picking<PFP>(myMap, allDarts, x,y);
//		if (d != Dart::nil())
//		{
//			CGoGNout << "Dart "<< d << " clicked" << CGoGNendl;
//		}
//		else
//		{
//			statusMsg("");
//		}
//	}
}

void Tuto5::cb_keyPress(int code)
{
//	if (code == 's')
//	{
//		std::string filename = selectFileSave("Export SVG file ");
//		CGoGNout << "Exporting "<< filename << CGoGNendl;
//		Algo::Render::SVG::SVGOut svg(filename, modelViewMatrix(), projectionMatrix());
////		svg.renderLinesToSVG<PFP>(myMap,position);
//		svg.setColor(Geom::Vec3f(1.,0.,0.));
//		svg.renderFacesToSVG<PFP>(myMap,position,0.8f);
//		//svg destruction close the file
//	}
	if (code  == 't')
	{
		if (m_timer->isActive())
			m_timer->stop();
		else
			m_timer->start(1000/30); // 30 fps
	}
}

Dart Tuto5::embedCube1()
{
	Dart d = Algo::Modelisation::Polyhedron<PFP>::createPolyhedron(m,6);

	m.closeMap();

	Dart dres = d;
	position[d] = PFP::VEC3(0,0,0);
	position[m.phi1(d)] = PFP::VEC3(1,0,0);
	position[m.phi1(m.phi1(d))] = PFP::VEC3(1,0,1);
	position[m.phi_1(d)] = PFP::VEC3(0,0,1);
	d = m.phi_1(m.phi2(m.phi_1(m.phi_1(m.phi2(m.phi_1(d))))));
	position[d] = PFP::VEC3(1,1,0);
	position[m.phi1(d)] = PFP::VEC3(0,1,0);
	position[m.phi1(m.phi1(d))] = PFP::VEC3(0,1,1);
	position[m.phi_1(d)] = PFP::VEC3(1,1,1);

	return m.phi2(m.phi1(dres));
}

Dart Tuto5::embedCube2()
{
	Dart d = Algo::Modelisation::Polyhedron<PFP>::createPolyhedron(m,6);

	m.closeMap();

	Dart dres = d;
	position[d] = PFP::VEC3(2,0,0);
	position[m.phi1(d)] = PFP::VEC3(3,0,0);
	position[m.phi1(m.phi1(d))] = PFP::VEC3(3,0,1);
	position[m.phi_1(d)] = PFP::VEC3(2,0,1);
	d = m.phi_1(m.phi2(m.phi_1(m.phi_1(m.phi2(m.phi_1(d))))));
	position[d] = PFP::VEC3(3,1,0);
	position[m.phi1(d)] = PFP::VEC3(2,1,0);
	position[m.phi1(m.phi1(d))] = PFP::VEC3(2,1,1);
	position[m.phi_1(d)] = PFP::VEC3(3,1,1);

	return m.phi2(m.phi_1(dres));
}


Dart Tuto5::embedMapCollapse()
{

	Dart dd = Algo::Modelisation::Polyhedron<PFP>::createPolyhedron(m,4);
	m.closeMap();
	Dart t1 = dd;
	// plongement
	position[t1] = PFP::VEC3(0.0f, 0.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(0.0f, -2.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(0.f, -1.f, -1.f);
	t1 = m.phi_1(m.phi2(dd));
	position[t1] = PFP::VEC3(-0.5f, -1.f, 0.f);

	Dart ee = Algo::Modelisation::Polyhedron<PFP>::createPolyhedron(m,4);
	m.closeMap();
	m.sewVolumes(dd,ee);
	t1 = ee;
	// plongement
	position[t1] = PFP::VEC3(0.0f, -2.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(0.0f, 0.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(0.f, -1.f, -1.f);
	t1 = m.phi_1(m.phi2(ee));
	position[t1] = PFP::VEC3(1.f, -1.f, 0.f);

	Dart ff = Algo::Modelisation::Polyhedron<PFP>::createPolyhedron(m,4);
	m.closeMap();
	m.sewVolumes(m.phi2(dd),ff);
	t1 = ff;
	// plongement
	position[t1] = PFP::VEC3(0.0f, 0.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(0.0f, -2.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(-0.5f, -1.f, 0.f);
	t1 = m.phi_1(m.phi2(ff));
	position[t1] = PFP::VEC3(0.f, -1.f, 1.f);

	Dart gg = Algo::Modelisation::Polyhedron<PFP>::createPolyhedron(m,4);
	m.closeMap();
	m.sewVolumes(gg,m.phi2(ee));
	m.sewVolumes(m.phi2(gg),m.phi2(ff));
	t1 = gg;
	// plongement
	position[t1] = PFP::VEC3(0.0f, -2.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(0.0f, 0.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(1.f, -1.f, 0.f);
	t1 = m.phi_1(m.phi2(gg));
	position[t1] = PFP::VEC3(0.f, -1.f, 1.f);

	Dart hh = Algo::Modelisation::Polyhedron<PFP>::createPolyhedron(m,4);
	m.closeMap();
	m.sewVolumes(m.phi2(m.phi_1(dd)),hh);
	t1 = hh;
	// plongement
	position[t1] = PFP::VEC3(0.0f, -1.0f, -1.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(0.0f, 0.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(-0.5f, -1.f, 0.f);
	t1 = m.phi_1(m.phi2(hh));
	position[t1] = PFP::VEC3(0.f, -0.2f, -0.8f);

	Dart jj = Algo::Modelisation::Polyhedron<PFP>::createPolyhedron(m,4);
	m.closeMap();
	m.sewVolumes(jj,m.phi<12>(ee));
	t1 = jj;
	// plongement
	position[t1] = PFP::VEC3(0.0f, 0.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(0.0f, -1.0f, -1.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(1.f, -1.f, 0.f);
	t1 = m.phi_1(m.phi2(jj));
	position[t1] = PFP::VEC3(0.f, -0.2f, -0.8f);

	Dart kk = Algo::Modelisation::Polyhedron<PFP>::createPolyhedron(m,4);
	m.closeMap();
	m.sewVolumes(kk,m.phi<12>(dd));
	t1 = kk;
	// plongement
	position[t1] = PFP::VEC3(0.0f, -2.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(0.0f, -1.0f, -1.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(-0.5f, -1.f, 0.f);
	t1 = m.phi_1(m.phi2(kk));
	position[t1] = PFP::VEC3(0.f, -1.8f, -0.6f);

	Dart ll = Algo::Modelisation::Polyhedron<PFP>::createPolyhedron(m,4);
	m.closeMap();
	m.sewVolumes(ll,m.phi2(m.phi_1(ee)));
	t1 = ll;
	// plongement
	position[t1] = PFP::VEC3(0.0f, -1.0f, -1.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(0.0f, -2.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(1.f, -1.f, 0.f);
	t1 = m.phi_1(m.phi2(ll));
	position[t1] = PFP::VEC3(0.f, -1.8f, -0.6f);

	/*4 tetra du pourtout de devant */
	Dart mm = Algo::Modelisation::Polyhedron<PFP>::createPolyhedron(m,4);
	m.closeMap();
	m.sewVolumes(mm,m.phi2(m.phi_1(ff)));
	t1 = mm;
	// plongement
	position[t1] = PFP::VEC3(-0.5f, -1.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(0.0f, 0.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(0.f, -1.f, 1.f);
	t1 = m.phi_1(m.phi2(mm));
	position[t1] = PFP::VEC3(0.f, -0.2f, 0.8f);

	Dart ww = Algo::Modelisation::Polyhedron<PFP>::createPolyhedron(m,4);
	m.closeMap();
	m.sewVolumes(ww,m.phi<12>(gg));
	t1 = ww;
	// plongement
	position[t1] = PFP::VEC3(0.f, 0.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(1.0f, -1.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(0.f, -1.f, 1.f);
	t1 = m.phi_1(m.phi2(ww));
	position[t1] = PFP::VEC3(0.f, -0.2f, 0.8f);

	Dart xx = Algo::Modelisation::Polyhedron<PFP>::createPolyhedron(m,4);
	m.closeMap();
	m.sewVolumes(xx,m.phi<12>(ff));
	t1 = xx;
	// plongement
	position[t1] = PFP::VEC3(0.f, -2.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(-0.5f, -1.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(0.f, -1.f, 1.f);
	t1 = m.phi_1(m.phi2(xx));
	position[t1] = PFP::VEC3(0.f, -1.6f, 0.6f);

	Dart cc = Algo::Modelisation::Polyhedron<PFP>::createPolyhedron(m,4);
	m.closeMap();
	m.sewVolumes(cc,m.phi2(m.phi_1(gg)));
	t1 = cc;
	// plongement
	position[t1] = PFP::VEC3(1.f, -1.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(0.f, -2.0f, 0.0f);
	t1 = m.phi1(t1);
	position[t1] = PFP::VEC3(0.f, -1.f, 1.f);
	t1 = m.phi_1(m.phi2(cc));
	position[t1] = PFP::VEC3(0.f, -1.6f, 0.6f);

	return dd;
}

int main(int argc, char **argv)
{
    // un peu d'interface
	QApplication app(argc, argv);
	Tuto5 sqt;
	
	sqt.position = sqt.m.addAttribute<PFP::VEC3>(VERTEX, "position");

	Dart t1 = Algo::Modelisation::Polyhedron<PFP>::createTetra(sqt.m) ;
	Dart t2 = Algo::Modelisation::Polyhedron<PFP>::createTetra(sqt.m) ;
	sqt.m.sewVolumes(t1, t2, false) ;
	sqt.m.closeHole(sqt.m.phi2(t1)) ;

	Algo::Modelisation::Primitive3D<PFP> primCat(sqt.m, sqt.position);
	Dart d = primCat.hexaGrid_topo(3, 3, 3);
	primCat.embedHexaGrid(1, 1, 1);

//	Dart d1 = sqt.embedMapCollapse();
//
//	sqt.m.check() ;
//	std::cout << "nb darts -> " << sqt.m.getNbDarts() << std::endl ;
//
//
//	sqt.m.collapseEdge(d1);
//	sqt.m.check() ;
//	std::cout << "nb darts -> " << sqt.m.getNbDarts() << std::endl ;

//	Dart d1 = embedCube1() ;
//	Dart d2 = embedCube2() ;

//	Dart dd = sqt.m.phi2(d1) ;

//	sqt.m.check() ;
//	std::cout << "nb darts -> " << sqt.m.getNbDarts() << std::endl ;

//	sqt.m.sewVolumes(d1, d2);

//	sqt.m.check() ;
//	std::cout << "nb darts -> " << sqt.m.getNbDarts() << std::endl ;

//	sqt.m.mergeVolumes(d1) ;

//	sqt.m.check() ;
//	std::cout << "nb darts -> " << sqt.m.getNbDarts() << std::endl ;

//	std::vector<Dart> v ;
//	v.push_back(dd) ;
//	dd = sqt.m.phi1(sqt.m.phi2(sqt.m.phi1(dd))) ;
//	v.push_back(dd) ;
//	dd = sqt.m.phi1(sqt.m.phi2(sqt.m.phi1(dd))) ;
//	v.push_back(dd) ;
//	dd = sqt.m.phi1(sqt.m.phi2(sqt.m.phi1(dd))) ;
//	v.push_back(dd) ;
//
//	sqt.m.splitVolume(v) ;

//	sqt.m.check() ;
//	std::cout << "nb darts -> " << sqt.m.getNbDarts() << std::endl ;

//	Dart f = sqt.m.phi2(v.front()) ;
//	Dart f3 = sqt.m.phi3(f) ;
//	sqt.m.unsewVolumes(f) ;

//	sqt.m.check() ;
//	std::cout << "nb darts -> " << sqt.m.getNbDarts() << std::endl ;

//	sqt.m.sewVolumes(f, f3) ;

//	sqt.m.check() ;
//	std::cout << "nb darts -> " << sqt.m.getNbDarts() << std::endl ;

//	sqt.m.splitFace(f, sqt.m.phi1(sqt.m.phi1(f))) ;

//	sqt.m.check() ;
//	std::cout << "nb darts -> " << sqt.m.getNbDarts() << std::endl ;

//	PFP::VEC3 p = sqt.position[f] + sqt.position[sqt.m.phi_1(f)] ;
//	p /= 2.0 ;
//	sqt.m.cutEdge(sqt.m.phi_1(f)) ;
//	sqt.position[sqt.m.phi_1(f)] = p ;

//	sqt.m.check() ;
//	std::cout << "nb darts -> " << sqt.m.getNbDarts() << std::endl ;

//	sqt.m.uncutEdge(sqt.m.phi_1(sqt.m.phi_1(f))) ;

//	sqt.m.check() ;
//	std::cout << "nb darts -> " << sqt.m.getNbDarts() << std::endl ;

//	sqt.m.deleteVolume(f) ;

//	sqt.m.check() ;
//	std::cout << "nb darts -> " << sqt.m.getNbDarts() << std::endl ;
//	Algo::Modelisation::Primitive3D<PFP> prim(myMap, position);

	// interface de tuto5.ui
    Utils::QT::uiDockInterface dock;
    sqt.setDock(&dock);

// 	// message d'aide
//	sqt.setHelpMsg("Enter pour dock on/off\nShift Enter pour console on/off\nShift Click gauche pour selectionner un brin");
//
//	CGoGNout.toStatusBar(&sqt);
//	CGoGNout << "CGoGNOut StatusBar" << Geom::Vec3f(2.5f, 2.2f, 4.3f) << CGoGNendl;
//
//	CGoGNout.toConsole(&sqt);
//
//	CGoGNout << "CGoGNOut dans la console" << Geom::Vec3f(2.5f, 2.2f, 4.3f) << CGoGNendl;
//
//	CGoGNout.toStatusBar(NULL);
	
	//  bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(sqt.m, sqt.position);
    float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

    // envoit info BB a l'interface
	sqt.setParamObject(lWidthObj, lPosObj.data());

//	sqt.setCallBack( dock.checkBox_balls, SIGNAL(toggled(bool)), SLOT(balls_onoff(bool)) );
//	sqt.setCallBack( dock.checkBox_vectors, SIGNAL(toggled(bool)), SLOT(vectors_onoff(bool)) );
//	sqt.setCallBack( dock.checkBox_text, SIGNAL(toggled(bool)), SLOT(text_onoff(bool)) );
	sqt.setCallBack( dock.checkBox_topo, SIGNAL(toggled(bool)), SLOT(topo_onoff(bool)) );

//	sqt.setCallBack( dock.slider_balls, SIGNAL(valueChanged(int)), SLOT(slider_balls(int)) );
//	sqt.setCallBack( dock.slider_vectors, SIGNAL(valueChanged(int)), SLOT(slider_vectors(int)) );
//	sqt.setCallBack( dock.slider_text, SIGNAL(valueChanged(int)), SLOT(slider_text(int)) );

	sqt.show();

//	sqt.slider_balls(50);
//	sqt.slider_vectors(50);
//	sqt.slider_text(50);

//	GLint texSize;
//	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);
//	CGoGNdbg.toConsole(&sqt);
//	CGoGNerr.toConsole(&sqt);
//	CGoGNdbg << " TextureSize " <<  texSize << CGoGNendl;
//	CGoGNerr << " test ERROR  " <<  5*7 << CGoGNflush;

	// et on attend la fin.
	return app.exec();
}
