#ifndef _SIMPLEVISU_H_
#define _SIMPLEVISU_H_


#include "visualPlugin.h"

#include "ui_simpleVisu.h"

#include <iostream>

#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"

#include "Algo/Import/import.h"
#include "Algo/Export/export.h"

#include "Algo/Render/GL2/mapRender.h"
#include "Algo/Render/GL2/topoRender.h"

#include "Utils/Shaders/shaderPhong.h"
#include "Utils/Shaders/shaderFlat.h"
#include "Utils/Shaders/shaderSimpleColor.h"
#include "Utils/Shaders/shaderVectorPerVertex.h"
#include "Utils/pointSprite.h"
#include "Utils/text3d.h"
#include "Utils/vboRender.h"

#include "Utils/Qt/qtInputs.h"

#include "Algo/Geometry/boundingbox.h"
#include "Algo/Geometry/normal.h"

#include <QHash>
#include <QPair>

using namespace CGoGN ;

struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap2 MAP ;
};

typedef PFP::MAP MAP ;
typedef PFP::VEC3 VEC3 ;


struct MyContainer{
	MAP* map;
	QPair<VBOHandler*, VBOHandler*> vbos;
	QPair<VertexAttribute<VEC3>, VertexAttribute<VEC3> > attributes;
	Algo::Render::GL2::MapRender* render;
	bool valid;

	MyContainer(MAP* myMap, VBOHandler* vbo1, VBOHandler* vbo2, VertexAttribute<VEC3> a1, VertexAttribute<VEC3> a2, Algo::Render::GL2::MapRender* r) :
		map(myMap), vbos(vbo1,vbo2), attributes(a1,a2), render(r),valid(true)
	{}
};


class MyWidget : public QWidget, public Ui::Form{
	Q_OBJECT
public:
	MyWidget(QWidget* parent=0) : QWidget(parent) {this->setupUi(this);}
};

class SimpleVisu : public VisualPlugin{
	Q_OBJECT
	Q_INTERFACES(Plugin)
public:
	SimpleVisu();
	~SimpleVisu(){}



//	void cb_updateMatrix(View* view);

	void cb_redraw(Scene* scene);

	void cb_initGL(Scene* scene);

	bool activate();
	void disable();


	void cb_recievedMap(MapHandler* map);

	void cb_removingMap(MapHandler* map);

	void cb_removingScene(Scene* scene);


protected:
	enum renderMode { FLAT, PHONG } ;

	Geom::Vec4f colDif ;
	Geom::Vec4f colSpec ;
	Geom::Vec4f colClear ;
	Geom::Vec4f colNormal ;

	float shininess ;

	Geom::BoundingBox<PFP::VEC3> bb ;
	float normalBaseSize ;
	float normalScaleFactor ;
	float vertexBaseSize ;
	float vertexScaleFactor ;
	float faceShrinkage ;

	int m_renderStyle ;
	bool m_drawVertices ;
	bool m_drawEdges ;
	bool m_drawFaces ;
	bool m_drawNormals ;
	bool m_drawTopo ;

	VertexAttribute<VEC3> m_position ;
	VertexAttribute<VEC3> m_normal ;

//	Algo::Render::GL2::MapRender* m_render ;

	VBOHandler* m_positionVBO ;
	VBOHandler* m_normalVBO ;
	bool initOK;

	Utils::ShaderPhong* m_phongShader ;
	Utils::ShaderFlat* m_flatShader ;
	Utils::ShaderVectorPerVertex* m_vectorShader ;
	Utils::ShaderSimpleColor* m_simpleColorShader ;
	Utils::PointSprite* m_pointSprite ;

	MyWidget* m_widget;

	QHash<Scene*, MyContainer> h_info;

	QList<QPair<Scene*,MapHandler*> > l_waitingAssoc;


//	void initVBOs(Scene* scene);

public slots:
	void slot_drawVertices(bool b) ;
	void slot_verticesSize(int i) ;
	void slot_drawEdges(bool b) ;
	void slot_drawFaces(bool b) ;
	void slot_faceLighting(int i) ;
	void slot_drawTopo(bool b) ;
	void slot_drawNormals(bool b) ;
	void slot_normalsSize(int i) ;
};


#endif
