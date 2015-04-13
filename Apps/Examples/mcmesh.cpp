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

#include "mcmesh.h"

MCMesh::MCMesh() :
	m_drawEdges(true),
	m_drawFaces(true),
	m_render(NULL),
	m_flatShader(NULL),
	m_simpleColorShader(NULL),
	m_linesShader(NULL)
{}

void MCMesh::initGUI()
{
    setDock(&dock);

    dock.check_drawEdges->setChecked(true);
    dock.check_drawFaces->setChecked(true);

	setCallBack( dock.check_drawEdges, SIGNAL(toggled(bool)), SLOT(slot_drawEdges(bool)) );
	setCallBack( dock.check_drawFaces, SIGNAL(toggled(bool)), SLOT(slot_drawFaces(bool)) );
}

void MCMesh::cb_initGL()
{
	Geom::Vec4f colClear(0.2f, 0.2f, 0.2f, 0.1f);
	Geom::Vec4f colDif(0.8f, 0.9f, 0.7f, 1.0f);
	Geom::Vec4f colSpec(0.9f, 0.9f, 0.9f, 1.0f);

	m_render = new Algo::Render::GL2::MapRender();

	m_positionVBO = new Utils::VBO();
	m_flatShader = new Utils::ShaderFlat();
	m_flatShader->setAttributePosition(m_positionVBO);
	m_flatShader->setAmbiant(colClear);
	m_flatShader->setDiffuse(colDif);
	m_flatShader->setExplode(1.0f);

	m_simpleColorShader = new Utils::ShaderSimpleColor();
	m_simpleColorShader->setAttributePosition(m_positionVBO);

	m_colorVBO = new Utils::VBO();
	m_linesShader = new Utils::ShaderBoldLines();
	m_linesShader->setAttributePosition(m_positionVBO);
//	m_linesShader->setAttributeColor(m_colorVBO);

	registerShader(m_flatShader);
	registerShader(m_simpleColorShader);
	registerShader(m_linesShader);

	m_dr = new Utils::Drawer();
	m_dr->newList();
	m_dr->pointSize(4.0f);
	m_dr->lineWidth(25.0f);
	m_dr->begin(GL_LINES);
	m_dr->color3f(1.0,0.0,0.0);
	m_dr->vertex3f(15.0,15.0,65.0);
	m_dr->vertex3f(110.0,110.0,65.0);
	m_dr->color3f(0.0,1.0,0.0);
	m_dr->vertex3f(110.0,15.0,75.0);
	m_dr->vertex3f(15.0,110.0,75.0);
	m_dr->end();
	m_dr->endList();

}

void MCMesh::cb_redraw()
{
glEnable(GL_BLEND);
	m_dr->callList(0.5f);
	if (m_drawEdges)
//	{
//		glLineWidth(1.0f);
//		if (m_drawFaces)
//		{
//			Geom::Vec4f c(0.0f, 0.0f, 0.0f, 0.0f);
//			m_simpleColorShader->setColor(c);
//		}
//		else
//		{
//			Geom::Vec4f c(1.0f, 1.0f, 0.0f, 0.0f);
//			m_simpleColorShader->setColor(c);
//		}
//		m_render->draw(m_simpleColorShader, Algo::Render::GL2::LINES);
//	}
	{
//		glLineWidth(1.0f);
//		m_linesShader->setClippingPlane(Geom::Vec4f(0.03,0.03,-1.1,64));

		m_linesShader->setLineWidth(12.0f);
		if (m_drawFaces)
		{
			Geom::Vec4f c(0.0f, 0.0f, 0.0f, 0.0f);
			m_linesShader->setColor(c);
		}
		else
		{
			Geom::Vec4f c(1.0f, 1.0f, 0.0f, 0.0f);
			m_linesShader->setColor(c);
		}
		m_render->draw(m_linesShader, Algo::Render::GL2::LINES);
	}


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LIGHTING);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	if (m_drawFaces)
	{
		m_flatShader->setExplode(1.0f);
		m_render->draw(m_flatShader, Algo::Render::GL2::TRIANGLES);

	}
	else
	{
		if (m_drawEdges)
		{
			Geom::Vec4f c(0.0f, 0.0f, 0.0f, 0.0f);
			m_simpleColorShader->setColor(c);
			m_render->draw(m_simpleColorShader, Algo::Render::GL2::TRIANGLES);
		}
	}
	glDisable(GL_POLYGON_OFFSET_FILL);
}

void MCMesh::cb_Open()
{
//	std::string filters("all (*.*);; trian (*.trian);; ctm (*.ctm);; off (*.off);; ply (*.ply)");
//	std::string filename = selectFile("Open Mesh", "", filters);
//	if (filename.empty())
//		return;
//
//	updateGL();
}

void MCMesh::slot_drawEdges(bool b)
{
	m_drawEdges = b;
	updateGL();
}

void MCMesh::slot_drawFaces(bool b)
{
	m_drawFaces = b;
	updateGL();
}

void MCMesh::MC()
{
	myMap.clear(false);
	// elargir l'image pour le calcul de la courbure
	SAlgo::MC::Image<DATATYPE>* myImgFr = myImg->addFrame(1);

	SAlgo::MC::WindowingGreater<DATATYPE> myWindFunc;
	myWindFunc.setIsoValue(DATATYPE(127));

	position = myMap.getAttribute<VEC3, VERTEX, MAP>("position");
	if (!position.isValid())
		position = myMap.addAttribute<VEC3, VERTEX, MAP>("position");

	// instanciation du mc
	SAlgo::MC::MarchingCube<DATATYPE, SAlgo::MC::WindowingGreater,PFP> mc(myImgFr, &myMap, position, myWindFunc, false);
	mc.simpleMeshing();

	delete myImgFr;
	
	updateRender();
	updateGL();
}

void MCMesh::updateRender()
{
//	SelectorDartNoBoundary<PFP::MAP> nb(myMap);
	//TODO	add Browser for no border
	m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::LINES);
	m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::TRIANGLES);

	m_positionVBO->updateData(position);
	m_colorVBO->updateData(position);
	Geom::Vec3f *ptr = reinterpret_cast<Geom::Vec3f*>(m_colorVBO->lockPtr());
	for (unsigned int i=0; i<m_colorVBO->nbElts();++i)
		*ptr++ /= 130.0f;
	m_colorVBO->releasePtr();

	bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);

	setParamObject(bb.maxSize(), bb.center().data());
	updateGLMatrices();
}

void MCMesh::fromFile(char* fname)
{
	myImg = new SAlgo::MC::Image<DATATYPE>();
	//myImg->loadInrgz(fname);
	myImg->loadVox(fname);
	CGoGNout << "Image chargee"<<CGoGNendl;
	CGoGNout << myImg->getWidthX() <<"x"<< myImg->getWidthY() <<"x"<< myImg->getWidthZ() << "voxels"<<CGoGNendl;
}

void MCMesh::sphere()
{
	DATATYPE *img = new DATATYPE[128*128*128];
	DATATYPE *ptr = img;
	for (int x=0; x<128; ++x)
	{
		for (int y=0; y<128; ++y)
		{
			for (int z=0; z<128; ++z)
			{
				Geom::Vec3f V(x-64, y-64, z-64);
				if (V.norm() < 30)
					*ptr++ = 255;
				else
					*ptr++ = 0;
			}
		}
	}
	
	myImg = new SAlgo::MC::Image<DATATYPE>(img,128,128,128,1.0f,1.0f,1.0f,false);
}

/**********************************************************************************************
 *                                      MAIN FUNCTION                                         *
 **********************************************************************************************/

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	MCMesh sqt;
	sqt.setGeometry(0, 0, 1000, 800);
 	sqt.show();

	sqt.valLabel = DATATYPE(0);

	if(argc == 2)
	{
		sqt.fromFile(argv[1]);
	}
	else
	{
		sqt.sphere();
	}

	sqt.initGUI();

	sqt.show();
	
	sqt.MC();

	return app.exec();
}
