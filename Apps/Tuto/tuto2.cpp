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

#include <iostream>

#include "Topology/generic/parameters.h"
#include "Topology/map/map2.h"
#include "Topology/generic/mapBrowser.h"

#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/GL2/mapRender.h"
#include "Utils/Shaders/shaderSimpleColor.h"

#include "Utils/drawer.h"

#include "tuto2.h"

using namespace CGoGN ;

struct PFP: public PFP_STANDARD
{
	// definition de la carte
	typedef Map2 MAP;
};

PFP::MAP myMap;

SelectorTrue allDarts;

PFP::TVEC3 position ;
PFP::TVEC3 normal ;
AttributeHandler<Geom::Vec4f> color ;

void MyQT::cb_initGL()
{
	// choose to use GL version 2
	Utils::GLSLShader::setCurrentOGLVersion(2);

	// create the render
	m_render = new Algo::Render::GL2::MapRender();

	// create VBO for position
	m_positionVBO = new Utils::VBO();

	// using simple shader with color
	m_shader = new Utils::ShaderSimpleColor();
	m_shader->setAttributePosition(m_positionVBO);

	m_color = Geom::Vec4f(0.,1.,0.,0.);
	m_shader->setColor(m_color);

	registerShader(m_shader);

	m_with_lines = true;
	m_line_width = 4.0;
}

void MyQT::cb_redraw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LIGHTING);
	if (m_shader)
	{
		if (m_with_lines)
		{
			glLineWidth(m_line_width);
			m_shader->setColor(Geom::Vec4f(1.,1.,0.,0.));
			m_render->draw(m_shader, Algo::Render::GL2::LINES);
		}

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);

		m_shader->setColor(m_color);
		m_render->draw(m_shader, Algo::Render::GL2::TRIANGLES);

		glDisable(GL_POLYGON_OFFSET_FILL);
	}
}

void MyQT::lines_slot(bool x)
{
	m_with_lines = x;
	updateGL();
}

void MyQT::line_width_slot(int x)
{

	m_line_width=x;
	updateGL();
}

void MyQT::color_slot()
{
	QColor col = QColorDialog::getColor ();
	if (col.isValid())
	{

		m_color = Geom::Vec4f(float(col.red())/255.0f,float(col.green())/255.0f,float(col.blue())/255.0f,0.0f);
		updateGL();
	}
	else
	{
		CGoGNout << "Cancel Color" << CGoGNendl;
	}
}

// Algorithme qui parcours une carte et affiche l'attribut position de chaque brin
template<typename PFP>
void TestDeParcoursAFF(typename PFP::MAP& m, MapBrowser& mb, const typename PFP::TVEC3& pos)
{
	for (Dart d = mb.begin(); d != mb.end(); mb.next(d))
	{
		typename PFP::VEC3 P = pos[d];
		CGoGNout << "P "<< P << CGoGNendl;
	}
}

int main(int argc, char **argv)
{
	/// Utilisation des MapBrowsers

	/// on reprend la carte de tuto1

	Dart d2 = myMap.newFace(3);
	Dart d3 = myMap.newFace(4);
	myMap.sewFaces(d2, d3);

	position = myMap.addAttribute<Geom::Vec3f>(VERTEX, "position");

	position[d2] = PFP::VEC3(0.0f, 0.0f, 0.0f);
	d2 = myMap.phi1(d2);
	position[d2] = PFP::VEC3(2.0f, 0.0f, 0.0f);
	d2 = myMap.phi1(d2);
	position[d2] = PFP::VEC3(1.0f, 3.0f, 0.0f);
	d2 = myMap.phi1(d2);
	d3 = myMap.phi<11>(d3);
	position[d3] = PFP::VEC3(0.0f, -2.0f, 0.0f);
	d3 = myMap.phi1(d3);
	position[d3] = PFP::VEC3(2.0f, -2.0f, 0.0f);
	d3 = myMap.phi1(d3);


	// MapBrowser: la carte elle meme
	CGoGNout << "Parcours avec la carte" << CGoGNendl;
	TestDeParcoursAFF<PFP>(myMap, myMap, position);

	// MapBrowserLinkedAuto
	// sous-carte dans une liste avec attribut gere par le browser
	CGoGNout << "Parcours avec le browser (les sommets)"<<CGoGNendl;

	// creation d'un browser avec attribut cree a la volee
	MapBrowserLinkedAuto<PFP::MAP>mbl(myMap);

	// on ajoute un brin par sommet dans le browser
	myMap.foreach_orbit(VERTEX, mbl);

	// et on parcours la sous-carte avec ce browser
	TestDeParcoursAFF<PFP>(myMap, mbl, position);


	// MapBrowserLinkedAttr
	// sous-carte dans une liste avec attribut gere par l'appelant
	CGoGNout << "Parcours avec le browser (le triangle)"<<CGoGNendl;

	// on cree un attribut Dart pour la liste
	AutoAttributeHandler<Dart> tableLink(myMap, DART);
	// le browser
	MapBrowserLinkedAttr<PFP::MAP>mbl2(tableLink);
	// que l'on remplit a la main
	Dart d = d2;
	mbl2.add(d);
	d =  myMap.phi1(d);
	mbl2.add(d);
	d =  myMap.phi1(d);
	mbl2.add(d);

	// et on parcours la sous-carte avec ce browser
	TestDeParcoursAFF<PFP>(myMap, mbl2, position);


	// MapBrowserSelector
	// sous-carte  avec des brins marques (equivalent de l'utilisation de good dans les algos)
	CGoGNout << "Parcours avec le browser selector"<<CGoGNendl;

	// on marque 2 brins pour le test
	DartMarker mk(myMap);
	mk.mark(d2);
	mk.mark(d3);

	// le selector qui selectionne les "bons" brins (les autres sont reconduits a la frontiere ;)
	SelectorMarked selector(mk);

//	// le browser
//	MapBrowserSelector<PFP::MAP>mbsel(myMap,selector);

//	// et on parcours la sous-carte avec ce browser
//	TestDeParcoursAFF<PFP>(myMap,mbsel,position);


	// interface:
	QApplication app(argc, argv);
	MyQT sqt;

    Utils::QT::uiDockInterface dock;
    sqt.setDock(&dock);

	sqt.setCallBack( dock.checkLines, SIGNAL(toggled(bool)), SLOT(lines_slot(bool)) );
	sqt.setCallBack( dock.color_button , SIGNAL(pressed()), SLOT(color_slot()) );
	sqt.setCallBack( dock.dial_line_width , SIGNAL(valueChanged(int)), SLOT(line_width_slot(int)) );

	// message d'aide
	sqt.setHelpMsg("Second Tuto");

	dock.number_of_darts->display(int(myMap.getNbDarts()));

    //  bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

    // envoit info BB a l'interface
	sqt.setParamObject(lWidthObj,lPosObj.data());

	// show 1 pour GL context
	sqt.show();

	// update du VBO position (context GL necessaire)
	sqt.m_positionVBO->updateData(position);

	// update des primitives du renderer
	SelectorTrue allDarts;
	sqt.m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::TRIANGLES);
	sqt.m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::LINES);

	// show final pour premier redraw
	sqt.show();

	// et on attend la fin.
	return app.exec();
}
