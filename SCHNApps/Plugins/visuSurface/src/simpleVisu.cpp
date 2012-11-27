#include "simpleVisu.h"



bool init=false;

SimpleVisu::SimpleVisu() :
	m_renderStyle(FLAT),
	m_drawVertices(false),
	m_drawEdges(false),
	m_drawFaces(true),
	m_drawNormals(false),
	m_drawTopo(false),
	initOK(false),
	m_phongShader(NULL),
	m_flatShader(NULL),
	m_vectorShader(NULL),
	m_simpleColorShader(NULL),
	m_pointSprite(NULL)
{
	normalScaleFactor = 1.0f ;
	vertexScaleFactor = 0.1f ;
	faceShrinkage = 1.0f ;

	colClear = Geom::Vec4f(0.2f, 0.2f, 0.2f, 0.1f) ;
	colDif = Geom::Vec4f(0.8f, 0.9f, 0.7f, 1.0f) ;
	colSpec = Geom::Vec4f(0.9f, 0.9f, 0.9f, 1.0f) ;
	colNormal = Geom::Vec4f(1.0f, 0.0f, 0.0f, 1.0f) ;
	shininess = 80.0f ;
}


void SimpleVisu::cb_recievedMap(MapHandler* map){
	std::cout << "recived MAP" << std::endl;

	int t;
	if((t=l_map.size())<=l_recievedScene.size()){
		std::cout << "t=" << t << std::endl;
		Scene* s= l_recievedScene[t-1];
		std::cout << "scene " << s->getName().toStdString() << std::endl;
		l_waitingAssoc.push_back(QPair<Scene*,MapHandler*>(s,map));
	}

	if(l_scene.size()!=0){
		updateGL();
	}
}

void SimpleVisu::cb_removingMap(MapHandler* map){
	int i= l_map.indexOf(map);

	if(i>=0 && i<l_recievedScene.size()){
		Scene* s;
		l_recievedScene.push_back((s=l_recievedScene.takeAt(i)));

		h_info.remove(s);
	}
}

void SimpleVisu::cb_removingScene(Scene* scene){
	int i= l_recievedScene.indexOf(scene);

	if(i>=0 && i<l_map.size()){
		MapHandler* vh;
		l_map.push_back((vh=l_map.takeAt(i)));
	}
}


void SimpleVisu::cb_redraw(Scene* scene){
	while(!l_waitingAssoc.isEmpty()){
		QPair<Scene*,MapHandler*> p= l_waitingAssoc.takeFirst();
		Scene* s= p.first;
		if(h_info.find(s)==h_info.end()){
			MapHandler* vh= p.second;

			VBOHandler* vboPosition, *vboNormal;
			if(!(vboPosition=vh->findVBO("positionVBO"))){
				vboPosition= vh->addNewVBO("positionVBO");
			}
			if(!(vboNormal=vh->findVBO("normalVBO"))){
				vboNormal= vh->addNewVBO("normalVBO");
			}

			PFP::MAP* map= (PFP::MAP*)vh->map();

			VertexAttribute<VEC3> position= map->getAttribute<PFP::VEC3, VERTEX>("position");
			VertexAttribute<VEC3> normal= map->getAttribute<PFP::VEC3, VERTEX>("normal") ;
			if(!normal.isValid()){
				normal= map->addAttribute<PFP::VEC3, VERTEX>("normal");

				if(position.isValid()){
					Algo::Geometry::computeNormalVertices<PFP>(*map, position, normal) ;
				}
			}

			Algo::Render::GL2::MapRender* render= NULL;
			bool valid= position.isValid() && normal.isValid();
			if(valid){
				vboPosition->updateData(position);
				vboNormal->updateData(normal) ;

				render= new Algo::Render::GL2::MapRender();

				render->initPrimitives<PFP>(*map, allDarts, Algo::Render::GL2::POINTS) ;
				render->initPrimitives<PFP>(*map, allDarts, Algo::Render::GL2::LINES) ;
				render->initPrimitives<PFP>(*map, allDarts, Algo::Render::GL2::TRIANGLES) ;

				Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(*map, position) ;
				s->firstViewFitSphere(bb.center()[0], bb.center()[1], bb.center()[2], bb.maxSize());
			}

			MyContainer mc(map,vboPosition,vboNormal,position,normal,render);
			mc.valid=valid;

			h_info.insert(s,mc);
		}
	}

	QHash<Scene*, MyContainer>::iterator it;
	if(scene && (it=h_info.find(scene))!=h_info.end() && it->valid){
		VBOHandler* vboPosition= it->vbos.first;
		VBOHandler* vboNormal= it->vbos.second;
		Algo::Render::GL2::MapRender* render= it->render;

		m_phongShader->setAttributePosition(vboPosition) ;
		m_phongShader->setAttributeNormal(vboNormal) ;
		m_flatShader->setAttributePosition(vboPosition) ;
		m_vectorShader->setAttributePosition(vboPosition) ;
		m_vectorShader->setAttributeVector(vboNormal) ;
		m_simpleColorShader->setAttributePosition(vboPosition) ;
		m_pointSprite->setAttributePosition(vboPosition) ;


		if(m_drawVertices)
		{
			float size = vertexScaleFactor ;
			m_pointSprite->setSize(size) ;
			m_pointSprite->predraw(Geom::Vec3f(0.0f, 0.0f, 1.0f)) ;
			render->draw(m_pointSprite, Algo::Render::GL2::POINTS) ;
			m_pointSprite->postdraw() ;
		}

		if(m_drawEdges)
		{
			glLineWidth(1.0f) ;
			render->draw(m_simpleColorShader, Algo::Render::GL2::LINES) ;
		}

		if(m_drawFaces)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) ;
			glEnable(GL_LIGHTING) ;
			glEnable(GL_POLYGON_OFFSET_FILL) ;
			glPolygonOffset(1.0f, 1.0f) ;
			switch(m_renderStyle)
			{
				case FLAT :
					m_flatShader->setExplode(faceShrinkage) ;
					render->draw(m_flatShader, Algo::Render::GL2::TRIANGLES) ;
					break ;
				case PHONG :
						render->draw(m_phongShader, Algo::Render::GL2::TRIANGLES) ;
					break ;
			}
			glDisable(GL_POLYGON_OFFSET_FILL) ;
		}


		if(m_drawNormals)
		{
			float size = normalBaseSize * normalScaleFactor ;
			m_vectorShader->setScale(size) ;
			glLineWidth(1.0f) ;
			render->draw(m_vectorShader, Algo::Render::GL2::POINTS) ;
		}

	}

//	if(scene){
//		for(int i=0; i<l_map.size(); ++i){
//			if(!initOK){
//				initVBOs(scene);
//			}
//
//			PFP::MAP* myMap= (PFP::MAP*)l_map[i];
//			if(!fited){
//
//				Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>((*myMap), m_position) ;
//				scene->firstViewFitSphere(bb.center()[0], bb.center()[1], bb.center()[2], bb.maxSize());
//
//				fited= true;
//			}
//
//			if(m_drawVertices /*&& position_ok*/)
//			{
//				float size = vertexScaleFactor ;
//				m_pointSprite->setSize(size) ;
//				m_pointSprite->predraw(Geom::Vec3f(0.0f, 0.0f, 1.0f)) ;
//				m_render->draw(m_pointSprite, Algo::Render::GL2::POINTS) ;
//				m_pointSprite->postdraw() ;
//			}
//
//			if(m_drawEdges /*&& position_ok*/)
//			{
//				glLineWidth(1.0f) ;
//				m_render->draw(m_simpleColorShader, Algo::Render::GL2::LINES) ;
//			}
//
//			if(m_drawFaces /*&& position_ok*/)
//			{
//				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) ;
//				glEnable(GL_LIGHTING) ;
//				glEnable(GL_POLYGON_OFFSET_FILL) ;
//				glPolygonOffset(1.0f, 1.0f) ;
//				switch(m_renderStyle)
//				{
//					case FLAT :
//						m_flatShader->setExplode(faceShrinkage) ;
//						m_render->draw(m_flatShader, Algo::Render::GL2::TRIANGLES) ;
//						break ;
//					case PHONG :
////						if(normal_ok){
//							m_render->draw(m_phongShader, Algo::Render::GL2::TRIANGLES) ;
////						}
//						break ;
//				}
//				glDisable(GL_POLYGON_OFFSET_FILL) ;
//			}
//
//
//			if(m_drawNormals /*&& normal_ok && position_ok*/)
//			{
//				float size = normalBaseSize * normalScaleFactor ;
//				m_vectorShader->setScale(size) ;
//				glLineWidth(1.0f) ;
//				m_render->draw(m_vectorShader, Algo::Render::GL2::POINTS) ;
//			}
//		}
//	}
}

void SimpleVisu::cb_initGL(Scene* scene){
	if(scene && h_info.find(scene)==h_info.end()){
		int t;
		if(((t=l_recievedScene.size())>0) && t<=l_map.size()){
			MapHandler* vh= l_map[l_recievedScene.size()-1];

			VBOHandler* vboPosition, *vboNormal;
			if(!(vboPosition=vh->findVBO("positionVBO"))){
				vboPosition= vh->addNewVBO("positionVBO");
			}
			if(!(vboNormal=vh->findVBO("normalVBO"))){
				vboNormal= vh->addNewVBO("normalVBO");
			}

			PFP::MAP* map= (PFP::MAP*)vh->map();

			VertexAttribute<VEC3> position= map->getAttribute<PFP::VEC3, VERTEX>("position");
			VertexAttribute<VEC3> normal= map->getAttribute<PFP::VEC3, VERTEX>("normal") ;
			if(!normal.isValid()){
				normal= map->addAttribute<PFP::VEC3, VERTEX>("normal");

				if(position.isValid()){
					Algo::Geometry::computeNormalVertices<PFP>(*map, position, normal) ;
				}
			}

			Algo::Render::GL2::MapRender* render= NULL;
			bool valid= position.isValid() && normal.isValid();
			if(valid){
				vboPosition->updateData(position);
				vboNormal->updateData(normal) ;

				render= new Algo::Render::GL2::MapRender();

				render->initPrimitives<PFP>(*map, allDarts, Algo::Render::GL2::POINTS) ;
				render->initPrimitives<PFP>(*map, allDarts, Algo::Render::GL2::LINES) ;
				render->initPrimitives<PFP>(*map, allDarts, Algo::Render::GL2::TRIANGLES) ;

				Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(*map, position) ;
				scene->firstViewFitSphere(bb.center()[0], bb.center()[1], bb.center()[2], bb.maxSize());
			}

			MyContainer mc(map,vboPosition,vboNormal,position,normal,render);
			mc.valid=valid;

			h_info.insert(scene,mc);
		}
	}
	if(!init && scene){
//		m_render = new Algo::Render::GL2::MapRender() ;


		m_phongShader = new Utils::ShaderPhong() ;
//		m_phongShader->setAttributePosition(m_positionVBO) ;
//		m_phongShader->setAttributeNormal(m_normalVBO) ;
		m_phongShader->setAmbiant(colClear) ;
		m_phongShader->setDiffuse(colDif) ;
		m_phongShader->setSpecular(colSpec) ;
		m_phongShader->setShininess(shininess) ;

		m_flatShader = new Utils::ShaderFlat() ;
//		m_flatShader->setAttributePosition(m_positionVBO) ;
		m_flatShader->setAmbiant(colClear) ;
		m_flatShader->setDiffuse(colDif) ;
		m_flatShader->setExplode(faceShrinkage) ;

		m_vectorShader = new Utils::ShaderVectorPerVertex() ;
//		m_vectorShader->setAttributePosition(m_positionVBO) ;
//		m_vectorShader->setAttributeVector(m_normalVBO) ;
		m_vectorShader->setColor(colNormal) ;

		m_simpleColorShader = new Utils::ShaderSimpleColor() ;
//		m_simpleColorShader->setAttributePosition(m_positionVBO) ;
		Geom::Vec4f c(0.1f, 0.1f, 0.1f, 1.0f) ;
		m_simpleColorShader->setColor(c) ;

		m_pointSprite = new Utils::PointSprite() ;
//		m_pointSprite->setAttributePosition(m_positionVBO) ;

		GLSLShader::registerShader(NULL,m_phongShader) ;
		GLSLShader::registerShader(NULL,m_flatShader) ;
		GLSLShader::registerShader(NULL,m_vectorShader) ;
		GLSLShader::registerShader(NULL,m_simpleColorShader) ;
		GLSLShader::registerShader(NULL,m_pointSprite) ;

		init=true;
	}
}

//void SimpleVisu::initVBOs(Scene* scene){
//	if(!initOK && !l_map.isEmpty()){
//		PFP::MAP* map= (PFP::MAP*)l_map.first();
//
//		m_position= map->getAttribute<PFP::VEC3, VERTEX>("position");
//		m_normal= map->getAttribute<PFP::VEC3, VERTEX>("normal") ;
//
//
//		if(!m_normal.isValid()){
//			map->addAttribute<PFP::VEC3, VERTEX>("normal");
//
//			if(m_position.isValid()){
//				Algo::Geometry::computeNormalVertices<PFP>(*map, m_position, m_normal) ;
//			}
//		}
//
//		QPair<VertexAttribute<VEC3>,VertexAttribute<VEC3> > attribs(m_position, m_normal);
//
//		m_positionVBO->updateData(m_position);
//		m_normalVBO->updateData(m_normal) ;
//
//
//
////		m_render->initPrimitives<PFP>(*map, allDarts, Algo::Render::GL2::POINTS) ;
////		m_render->initPrimitives<PFP>(*map, allDarts, Algo::Render::GL2::LINES) ;
////		m_render->initPrimitives<PFP>(*map, allDarts, Algo::Render::GL2::TRIANGLES) ;
//
//
//		initOK=true;
//	}
//}

bool SimpleVisu::activate(){
	m_widget= new MyWidget(m_window);
	addWidgetInDockTab(m_widget,"Simple Visu");

	connect( m_widget->check_drawVertices, SIGNAL(toggled(bool)), this, SLOT(slot_drawVertices(bool)) ) ;
	connect( m_widget->slider_verticesSize, SIGNAL(valueChanged(int)), this, SLOT(slot_verticesSize(int)) ) ;
	connect( m_widget->check_drawEdges, SIGNAL(toggled(bool)), this, SLOT(slot_drawEdges(bool)) ) ;
	connect( m_widget->check_drawFaces, SIGNAL(toggled(bool)), this, SLOT(slot_drawFaces(bool)) ) ;
	connect( m_widget->combo_faceLighting, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_faceLighting(int)) ) ;
	connect( m_widget->check_drawNormals, SIGNAL(toggled(bool)),this,  SLOT(slot_drawNormals(bool)) ) ;


	return true;
}

void SimpleVisu::disable(){
	removeTabInDock(m_widget);

	GLSLShader::unregisterShader(NULL,m_pointSprite);
	GLSLShader::unregisterShader(NULL,m_simpleColorShader) ;
	GLSLShader::unregisterShader(NULL,m_vectorShader) ;
	GLSLShader::unregisterShader(NULL,m_flatShader) ;
	GLSLShader::unregisterShader(NULL,m_phongShader) ;
	delete m_pointSprite;
	delete m_simpleColorShader;
	delete m_vectorShader;
	delete m_flatShader;
	delete m_phongShader;
}

void SimpleVisu::slot_drawVertices(bool b)
{
	m_drawVertices = b ;
	updateGL() ;
}

void SimpleVisu::slot_verticesSize(int i)
{
	vertexScaleFactor = i / 500.0f ;
	updateGL() ;
}

void SimpleVisu::slot_drawEdges(bool b)
{
	m_drawEdges = b ;
	updateGL() ;
}

void SimpleVisu::slot_drawFaces(bool b)
{
	m_drawFaces = b ;
	updateGL() ;
}

void SimpleVisu::slot_faceLighting(int i)
{
	m_renderStyle = i ;
	updateGL() ;
}

void SimpleVisu::slot_drawTopo(bool b)
{
	m_drawTopo = b ;
	updateGL() ;
}

void SimpleVisu::slot_drawNormals(bool b)
{
	m_drawNormals = b ;
	updateGL() ;
}



#ifndef DEBUG
	Q_EXPORT_PLUGIN2(SimpleVisu, SimpleVisu)
#else
	Q_EXPORT_PLUGIN2(SimpleVisuD, SimpleVisu)
#endif

