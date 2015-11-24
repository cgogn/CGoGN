#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

MapHandlerGen::MapHandlerGen(const QString& name, SCHNApps* s, GenericMap* map) :
	m_showBB(true),
	m_name(name),
	m_schnapps(s),
	m_map(map),
	m_frame(NULL),
	m_transfoMatrix(1.0f),
	m_transfoMatrixInv(1.0f),
	m_bbVertexAttribute(NULL),
	m_bbDrawer(NULL),
	m_bbColor(0,1,0),
	m_render(NULL),
	m_topoRender(NULL)
{
	m_frame = new qglviewer::ManipulatedFrame();
	connect(m_frame, SIGNAL(manipulated()), this, SLOT(frameModified()));
}

MapHandlerGen::~MapHandlerGen()
{
	if(m_frame)
		delete m_frame;
	if(m_bbDrawer)
		delete m_bbDrawer;
	if(m_render)
		delete m_render;
	foreach(CGoGN::Utils::VBO* vbo, m_vbo)
		delete vbo;

	deleteTopoRender();
}





QString MapHandlerGen::getName()
{
	return m_name;
}

SCHNApps* MapHandlerGen::getSCHNApps() const
{
	return m_schnapps;
}

bool MapHandlerGen::isSelectedMap() const
{
	return m_schnapps->getSelectedMap() == this;
}

GenericMap* MapHandlerGen::getGenericMap() const
{
	return m_map;
}

/*********************************************************
* MANAGE FRAME
*********************************************************/

qglviewer::ManipulatedFrame* MapHandlerGen::getFrame() const
{
	return m_frame;
}

glm::mat4 MapHandlerGen::getFrameMatrix() const
{
	GLdouble m[16];
	m_frame->getMatrix(m);
	glm::mat4 matrix;
	for (unsigned int i = 0; i < 4; ++i)
	{
		for (unsigned int j = 0; j < 4; ++j)
			matrix[i][j] = (float)m[i * 4 + j];
	}
	return matrix;
}

void MapHandlerGen::frameModified()
{
	DEBUG_EMIT("frameModified");
	emit(boundingBoxModified());
}




/*********************************************************
* MANAGE BOUNDING BOX
*********************************************************/

void MapHandlerGen::showBB(bool b)
{
	m_showBB = b;
	foreach(View* view, l_views)
		view->updateGL();
}

bool MapHandlerGen::isBBshown() const
{
	return m_showBB;
}

void  MapHandlerGen::setBBColor(const QString& color)
{
	QColor col(color);
	m_bbColor[0] = col.redF();
	m_bbColor[1] = col.greenF();
	m_bbColor[2] = col.blueF();
	updateBB();
}

void MapHandlerGen::setBBVertexAttribute(const QString& name)
{
	m_bbVertexAttribute = m_map->getAttributeVectorGen(VERTEX, name.toStdString());
	updateBB();
	// for update of interface
	if (m_schnapps->getSelectedMap() == this)
	{
		m_schnapps->setSelectedMap("NONE");
		m_schnapps->setSelectedMap(this->getName());
	}
}

AttributeMultiVectorGen* MapHandlerGen::getBBVertexAttribute() const
{
	return m_bbVertexAttribute;
}

QString MapHandlerGen::getBBVertexAttributeName() const
{
	if (m_bbVertexAttribute)
		return QString::fromStdString(m_bbVertexAttribute->getName());
	else
		return QString();
}

float MapHandlerGen::getBBdiagSize() const
{
	return m_bbDiagSize;
}

Utils::Drawer* MapHandlerGen::getBBDrawer() const
{
	return m_bbDrawer;
}



/*********************************************************
 * MANAGE ATTRIBUTES
 *********************************************************/

void MapHandlerGen::notifyAttributeModification(const AttributeHandlerGen& attr)
{
	QString nameAttr = QString::fromStdString(attr.name());

	if (m_vbo.contains(nameAttr))
		m_vbo[nameAttr]->updateData(attr);

	if (m_bbVertexAttribute && m_bbVertexAttribute->getName() == attr.name())
		updateBB();

	DEBUG_EMIT("attributeModified");
	emit(attributeModified(attr.getOrbit(), nameAttr));

	foreach(View* view, l_views)
		view->updateGL();
}

void MapHandlerGen::notifyConnectivityModification()
{
	if (m_render)
	{
		m_render->setPrimitiveDirty(Algo::Render::GL2::POINTS);
		m_render->setPrimitiveDirty(Algo::Render::GL2::LINES);
		m_render->setPrimitiveDirty(Algo::Render::GL2::TRIANGLES);
		m_render->setPrimitiveDirty(Algo::Render::GL2::BOUNDARY);
	}

	for(unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		foreach (CellSelectorGen* cs, m_cellSelectors[orbit])
			cs->rebuild();
	}

	DEBUG_EMIT("connectivityModified");
	emit(connectivityModified());

	foreach(View* view, l_views)
		view->updateGL();
}

void MapHandlerGen::clear(bool removeAttrib)
{
	if (m_render)
	{
		m_render->setPrimitiveDirty(Algo::Render::GL2::POINTS);
		m_render->setPrimitiveDirty(Algo::Render::GL2::LINES);
		m_render->setPrimitiveDirty(Algo::Render::GL2::TRIANGLES);
		m_render->setPrimitiveDirty(Algo::Render::GL2::BOUNDARY);
	}

	for(unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		foreach (CellSelectorGen* cs, m_cellSelectors[orbit])
			cs->rebuild();
	}

	std::vector<std::string> attrs[NB_ORBITS];

	for (unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		if(m_map->isOrbitEmbedded(orbit))
		{
			AttributeContainer& cont = m_map->getAttributeContainer(orbit);
			cont.getAttributesNames(attrs[orbit]);
		}
	}

	m_map->clear(removeAttrib);

	for (unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		for (unsigned int i = 0; i < attrs[orbit].size(); ++i)
		{
			QString name = QString::fromStdString(attrs[orbit][i]);
			if (removeAttrib)
			{
				if (orbit == VERTEX)
					deleteVBO(name);
				DEBUG_EMIT("attributeRemoved");
				emit(attributeRemoved(orbit, name));
			}
			else
			{
				if (orbit == VERTEX)
					updateVBO(name);
				DEBUG_EMIT("attributeModified");
				emit(attributeModified(orbit, name));
			}
		}
	}

	DEBUG_EMIT("connectivityModified");
	emit(connectivityModified());
}

/*********************************************************
 * MANAGE VBOs
 *********************************************************/

Utils::VBO* MapHandlerGen::createVBO(const AttributeMultiVectorGen* attr)
{
	if(attr)
	{
		// RECORDING
		QTextStream* rec = m_schnapps->pythonStreamRecorder();
		if (rec)
			*rec << this->getName() << ".createVBO(\"" << QString(attr->getName().c_str()) << "\");" << endl;

		QString name = QString::fromStdString(attr->getName());
		Utils::VBO* vbo = getVBO(name);
		if(!vbo)
		{
			vbo = new Utils::VBO(attr->getName());
			vbo->updateData(attr);
			m_vbo.insert(name, vbo);
			DEBUG_EMIT("vboAdded");
			emit(vboAdded(vbo));
		}
		else
			vbo->updateData(attr);
		return vbo;
	}
	else
		return NULL;
}

Utils::VBO* MapHandlerGen::createVBO(const AttributeHandlerGen& attr)
{
	return createVBO(attr.getDataVectorGen());
}

Utils::VBO* MapHandlerGen::createVBO(const QString& name)
{
	return createVBO(m_map->getAttributeVectorGen(VERTEX, name.toUtf8().constData()));
}

void MapHandlerGen::updateVBO(const AttributeMultiVectorGen* attr)
{
	if(attr)
	{
		Utils::VBO* vbo = getVBO(QString::fromStdString(attr->getName()));
		if(vbo)
			vbo->updateData(attr);
	}
}

void MapHandlerGen::updateVBO(const AttributeHandlerGen& attr)
{
	return updateVBO(attr.getDataVectorGen());
}

void MapHandlerGen::updateVBO(const QString& name)
{
	updateVBO(m_map->getAttributeVectorGen(VERTEX, name.toUtf8().constData()));
}

Utils::VBO* MapHandlerGen::getVBO(const QString& name) const
{
	if (m_vbo.contains(name))
		return m_vbo[name];
	else
		return NULL;
}

void MapHandlerGen::deleteVBO(const QString& name)
{
	if (m_vbo.contains(name))
	{
		Utils::VBO* vbo = m_vbo[name];
		m_vbo.remove(name);
		DEBUG_EMIT("vboRemoved");
		emit(vboRemoved(vbo));
		delete vbo;
	}
}

/*********************************************************
 * MANAGE CELL SELECTORS
 *********************************************************/

void MapHandlerGen::removeCellSelector(unsigned int orbit, const QString& name)
{
	CellSelectorGen* cs = getCellSelector(orbit, name);
	if (cs)
	{
		m_cellSelectors[orbit].remove(name);
		DEBUG_EMIT("cellSelectorRemoved");
		emit(cellSelectorRemoved(orbit, name));

		disconnect(cs, SIGNAL(selectedCellsChanged()), this, SLOT(selectedCellsChanged()));

		delete cs;
	}
}

CellSelectorGen* MapHandlerGen::getCellSelector(unsigned int orbit, const QString& name) const
{
	if (m_cellSelectors[orbit].contains(name))
		return m_cellSelectors[orbit][name];
	else
		return NULL;
}

void MapHandlerGen::selectedCellsChanged()
{
	CellSelectorGen* cs = static_cast<CellSelectorGen*>(QObject::sender());
	DEBUG_EMIT("selectedCellsChanged");
	emit(selectedCellsChanged(cs));
}

void MapHandlerGen::updateMutuallyExclusiveSelectors(unsigned int orbit)
{
	QList<CellSelectorGen*> mex;
	foreach(CellSelectorGen* cs, m_cellSelectors[orbit])
	{
		if(cs->isMutuallyExclusive())
			mex.append(cs);
	}
	foreach(CellSelectorGen* cs, m_cellSelectors[orbit])
		cs->setMutuallyExclusiveSet(mex);
}


/*********************************************************
 * MANAGE LINKED VIEWS
 *********************************************************/

void MapHandlerGen::linkView(View* view)
{
	if(view && !l_views.contains(view))
		l_views.push_back(view);
}

void MapHandlerGen::unlinkView(View* view)
{
	l_views.removeOne(view);
}


void MapHandlerGen::deleteTopoRender()
{
	if (m_topoRender)
		delete m_topoRender;
}



/*********************************************************
* MANAGE TRANSFO
*********************************************************/


void MapHandlerGen::setScaling(float sx, float sy, float sz)
{
	m_transfoMatrix[0][0] = sx;
	m_transfoMatrix[1][1] = sy;
	m_transfoMatrix[2][2] = sz;

	foreach(View* view, l_views)
		view->updateGL();


	// replace by inverse compute if other transfo than scale
	m_transfoMatrixInv[0][0] = 1.0f/sx;
	m_transfoMatrixInv[1][1] = 1.0f/sy;
	m_transfoMatrixInv[2][2] = 1.0f/sz;

}




QString MapHandlerGen::frameToString()
{
	QString res;
	QTextStream str(&res);
	const GLdouble* mat = m_frame->matrix();
	for (int i = 0; i < 16; ++i)
		str << mat[i] << " ";
	return res;
}

void MapHandlerGen::frameFromString(QString frame)
{
	QTextStream str(&frame);

	GLdouble mat[16];
	for (int i = 0; i < 16; ++i)
		str >> mat[i];

	m_frame->setFromMatrix(mat);

	frameModified();
}

void MapHandlerGen::frameReset()
{
	GLdouble mat[16];
	// Identity
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			if (i==j)
				mat[i*4+j] = 1;
			else
				mat[i*4+j] = 0;

	m_frame->setFromMatrix(mat);

	frameModified();
}






} // namespace SCHNApps

} // namespace CGoGN
