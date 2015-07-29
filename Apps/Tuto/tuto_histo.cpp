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

#include "tuto_histo.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Import/import.h"

#include "Algo/Geometry/area.h"


#include "Topology/generic/traversor/traversorCell.h"

#include <cmath>
#include <sstream>

using namespace CGoGN ;



int main(int argc, char **argv)
{
	//	// interface
	QApplication app(argc, argv);
	MyQT sqt;
	// copy output tout Qt console of application (shift enter)
	CGoGNout.toConsole(&sqt);

	// example code itself
	sqt.createMap(std::string(argv[1]));
	// set help message in menu
	sqt.setHelpMsg("Tuto Histogram: \nLoad mesh & compute histogram & quantiles\n+/- increase/decrease \
number of classes\no/p increase/decrease number of quantiles\nw/x change \
min/max\nc center the histogram on 0\nh show/hide histogram\nq show/hide quantiles\
File/Save for exporting histogram in svg format");

	// final show for redraw
	sqt.show();
	// and wait for the end
	return app.exec();
}

void MyQT::createMap(const std::string& filename)
{
	myMap.clear(true) ;

	size_t pos = filename.rfind(".");    // position of "." in filename
	std::string extension = filename.substr(pos);

	std::vector<std::string> attrNames ;
	if(!Algo::Surface::Import::importMesh<PFP>(myMap, filename.c_str(), attrNames))
	{
		CGoGNerr << "could not import " << filename << CGoGNendl ;
		return;
	}
	VertexAttribute<VEC3, MAP> position = myMap.getAttribute<VEC3, VERTEX, MAP>(attrNames[0]) ;

	// attribute on which we make the histogram
	VertexAttribute<float, MAP> area = myMap.addAttribute<float, VERTEX, MAP>("area");

	// attribute color to generate from histo
	VertexAttribute<VEC3, MAP> colorF = myMap.addAttribute<VEC3, VERTEX, MAP>("colorF");

	// compute the area attribute
	Algo::Surface::Geometry::computeOneRingAreaVertices<PFP>(myMap,position,area);

	// just some tricks to obtain relatives value
	float amax = 0;
	for (unsigned int i = area.begin(); i != area.end(); area.next(i))
		if (area[i] > amax) amax = area[i];
	for (unsigned int i = area.begin(); i != area.end(); area.next(i))
		area[i] -= amax / 4;


	// create a colormap class for histogram coloring
	l_cm = new ColMap();

	// create the histogram
	l_histo = new Algo::Histogram::Histogram(*l_cm);

	// init data with specific conversion (direction init  is: l_histo->initData(area);)
	AttConv ac(area);
	l_histo->initDataConvert(ac);

	// compute the histogram
	l_nbc = 20;
	l_histo->populateHisto(l_nbc);

	//compute the quantiles
	l_nbq = 10;
	l_histo->populateQuantiles(l_nbq);

	// compute color attribute from histogram (histogram can alsdo directly update a VBO see below)
//	l_histo->histoColorize(colorF);


	// create a popup window
	l_popup = new Utils::QT::QtPopUp();

	// create the widget to view histogram
	l_histodraw = new Algo::Histogram::RenderHistogram(l_popup, *l_histo);

	// some simple parameters
	l_histodraw->setQuantilesDraw(true);
	l_histodraw->setHistoPosition(true);
	l_histodraw->setOpacity(0.6f);

	// create a table of color
	std::vector<Geom::Vec3f> colors;
	Utils::createTableColor01(colors,10,Utils::color_map_blue_green_red);

	// and us it to color the quantiles
	l_histodraw->setQuantilesColors(colors);

	// add the widget to the popup
	l_popup->addWidget(l_histodraw,0,0);
	l_popup->show();

	// connect the clicked signal of histogram widget to a slot
	QObject::connect( l_histodraw, SIGNAL( clicked(unsigned int,unsigned int) ), this, SLOT( clickHisto(unsigned int,unsigned int) ) );


    //  bounding box of scene
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

    // send BB info to interface for centering on GL screen
	setParamObject(lWidthObj, lPosObj.data());

	// first show for be sure that GL context is binded
	show();

	// update of position VBO 	(context GL necessary)
	m_positionVBO->updateData(position);

	//	m_colorVBO2->updateData(colorF);
	// histogram can directly update a VBO (warning GL context must be accessible, after the first show!)
	l_histo->histoColorizeVBO(*m_colorVBO2);

	// construct rendering primities
	m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::TRIANGLES);


}

void MyQT::cb_keyPress(int keycode)
{
	switch(keycode)
	{
    	case '+' :
    		l_nbc++;
    		l_histo->populateHisto(l_nbc);
    		l_histodraw->repaint();
			l_histo->histoColorizeVBO(*m_colorVBO2);
			updateGL();
       		break;
       	case '-' :
       		if (l_nbc>0)
       			l_nbc--;
    		l_histo->populateHisto(l_nbc);
    		l_histodraw->repaint();
			l_histo->histoColorizeVBO(*m_colorVBO2);
			updateGL();
    		break;

       	case 'p' :
       		l_nbq++;
			l_histo->populateQuantiles(l_nbq);
			l_histodraw->repaint();
			break;
		case 'o' :
			if (l_nbq>0)
				l_nbq--;
			l_histo->populateQuantiles(l_nbq);
			l_histodraw->repaint();
		break;
		case 'q':
			l_histodraw->setQuantilesDraw(!l_histodraw->getQuantilesDraw());
			l_histodraw->repaint();
			break;
		case 'h':
			l_histodraw->setHistoDraw(!l_histodraw->getHistoDraw());
			l_histodraw->repaint();
			break;

       	case 'c' :
			l_histo->centerOnZero();
			l_histo->populateHisto(l_nbc);
			l_histodraw->repaint();
			l_histo->histoColorizeVBO(*m_colorVBO2);
			updateGL();
			break;

       	case 'w' :
       	{
       		double av = (l_histo->getMin() + l_histo->getMax() )/2.0;
       		l_histo->setMin(av*0.1 + l_histo->getMin()*0.9);
       		l_histo->setMax(av*0.1 + l_histo->getMax()*0.9);
			l_histo->populateHisto(l_nbc);
			l_histodraw->repaint();
			l_histo->histoColorizeVBO(*m_colorVBO2);
			updateGL();
       	}
			break;
       	case 'x' :
       	{
       		double av = (l_histo->getMin() + l_histo->getMax() )/2.0;
       		l_histo->setMin(-0.1*av + l_histo->getMin()*1.1);
       		l_histo->setMax(-0.1*av + l_histo->getMax()*1.1);
			l_histo->populateHisto(l_nbc);
			l_histodraw->repaint();
			l_histo->histoColorizeVBO(*m_colorVBO2);
			updateGL();
       	}
			break;


    	default:
    		break;
    }

}


void MyQT::cb_initGL()
{
	// create the render
	m_render = new Algo::Render::GL2::MapRender();

	// VBOs
	m_positionVBO = new Utils::VBO();
	m_colorVBO2 = new Utils::VBO();

	//shader
	m_shader2 = new Utils::ShaderColorPerVertex();
	m_shader2->setAttributePosition(m_positionVBO);
	registerShader(m_shader2);
}


void MyQT::cb_redraw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// draw faces with pervertex color rendering
	m_shader2->setAttributeColor(m_colorVBO2);
	m_render->draw(m_shader2, Algo::Render::GL2::TRIANGLES);

	glDisable(GL_POLYGON_OFFSET_FILL);
}

void MyQT::clickHisto(unsigned int i,unsigned int j)
{
	std::cout << "CLICK on column Histo: " << i << " / Quantiles: " << j <<std::endl;

	if (i != Algo::Histogram::RenderHistogram::NONE)
	{
		std::vector<unsigned int > vc;
		l_histo->cellsOfHistogramColumn(i,vc);
		std::cout << "Cells of histo: "<< std::endl;
		for (unsigned int k = 0; k < vc.size(); ++k)
			std::cout << vc[k] << "/";
		std::cout << std::endl;

		CellMarker<MAP, VERTEX> cm(myMap);
		std::cout << l_histo->markCellsOfHistogramColumn(i,cm) << " marked cells" << std::endl;
	}

	if (j != Algo::Histogram::RenderHistogram::NONE)
	{
		std::vector<unsigned int > vc;
		l_histo->cellsOfHistogramColumn(j,vc);
		std::cout << "Cells of quantile: "<< std::endl;
		for (unsigned int k=0; k <vc.size(); ++k)
			std::cout << vc[k]<<"/";
		std::cout << std::endl;
	}
}

void MyQT::cb_Save()
{
	std::string filename = selectFileSave("export svg",".","*.svg") ;
	if (filename.empty())
		return ;
	l_histodraw->svgExport(filename);
}

void MyQT::cb_exit()
{
	if (m_render != NULL)
		delete m_render;
	if (m_shader2 != NULL)
		delete m_shader2;
	if (m_positionVBO != NULL)
		delete m_positionVBO;
	if (l_cm != NULL)
		delete l_cm;
	if (l_histo != NULL)
		delete l_histo;
	if (l_histodraw != NULL)
		delete l_histodraw;
	if (l_popup != NULL)
		delete l_popup;
}


