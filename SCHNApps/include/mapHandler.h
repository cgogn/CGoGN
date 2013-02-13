#ifndef _MAPHANDLER_H_
#define _MAPHANDLER_H_

#include <QString>

#include "types.h"
#include "view.h"
#include "plugin.h"

#include "Topology/generic/genericmap.h"
#include "Topology/generic/attribmap.h"
#include "Topology/generic/functor.h"
#include "Topology/generic/attributeHandler.h"
#include "Utils/vbo.h"
#include "Algo/Render/GL2/mapRender.h"
#include "Algo/Geometry/boundingbox.h"

namespace CGoGN
{

namespace SCHNApps
{

class MapHandlerGen : public QObject
{
	Q_OBJECT

public:
	MapHandlerGen(const QString& name, Window* window, GenericMap* map);
	virtual ~MapHandlerGen();

	inline const QString& getName() const { return m_name; }

public slots:
	inline QString getName() { return m_name; }
	inline void setName(const QString& name) { m_name = name; }

	inline Window* getWindow() const { return m_window; }
	inline void setWindow(Window* w) { m_window = w; }

	inline GenericMap* getGenericMap() const { return m_map; }

	inline const qglviewer::Vec& getBBmin() const { return m_bbMin; }
	inline const qglviewer::Vec& getBBmax() const { return m_bbMax; }
	inline float getBBdiagSize() const { return m_bbDiagSize; }

	inline bool isUsed() const { return !l_views.empty(); }

public:
	virtual void draw(Utils::GLSLShader* shader, int primitive) = 0;

	inline void setPrimitiveDirty(int primitive) {	m_render->setPrimitiveDirty(primitive);	}

	/*********************************************************
	 * MANAGE ATTRIBUTES
	 *********************************************************/

	template <typename T, unsigned int ORBIT>
	AttributeHandler<T, ORBIT> getAttribute(const QString& nameAttr, bool onlyRegistered = true) const;

	template <typename T, unsigned int ORBIT>
	AttributeHandler<T, ORBIT> addAttribute(const QString& nameAttr, bool registerAttr = true);

	template <typename T, unsigned int ORBIT>
	inline void registerAttribute(const AttributeHandler<T, ORBIT>& ah);

	inline QString getAttributeTypeName(unsigned int orbit, const QString& nameAttr) const;

	inline const AttributeHash& getAttributesList(unsigned int orbit) const { return h_attribs[orbit];	}

	template <typename T, unsigned int ORBIT>
	inline void notifyAttributeModification(const AttributeHandler<T, ORBIT>& attr)
	{
		emit(attributeModified(ORBIT, QString::fromStdString(attr.name())));
	}

	inline void notifyConnectivityModification()
	{
		emit(connectivityModified());
	}

	/*********************************************************
	 * MANAGE VBOs
	 *********************************************************/

	template <typename ATTR_HANDLER>
	Utils::VBO* createVBO(const ATTR_HANDLER& attr);

	template <typename ATTR_HANDLER>
	void updateVBO(const ATTR_HANDLER& attr);

	Utils::VBO* getVBO(const QString& name) const;
	inline QList<Utils::VBO*> getVBOList() const { return h_vbo.values(); }
	void deleteVBO(const QString& name);

	/*********************************************************
	 * MANAGE LINKED VIEWS
	 *********************************************************/

	void linkView(View* view);
	void unlinkView(View* view);
	inline const QList<View*>& getLinkedViews() const { return l_views; }
	inline bool isLinkedToView(View* view) const { return l_views.contains(view); }

protected:
	QString m_name;
	Window* m_window;
	GenericMap* m_map;

	Algo::Render::GL2::MapRender* m_render;

	qglviewer::Vec m_bbMin;
	qglviewer::Vec m_bbMax;
	float m_bbDiagSize;

	QList<View*> l_views;

	VBOHash h_vbo;
	AttributeHash h_attribs[NB_ORBITS];

signals:
	void connectivityModified();

	void attributeAdded(unsigned int orbit, const QString& nameAttr);
	void attributeModified(unsigned int orbit, QString nameAttr);

	void vboAdded(Utils::VBO* vbo);
	void vboRemoved(Utils::VBO* vbo);
};

template <typename PFP>
class MapHandler : public MapHandlerGen
{
public:
	MapHandler(const QString& name, Window* window, typename PFP::MAP* map) :
		MapHandlerGen(name, window, map)
	{}

	~MapHandler()
	{
		if (m_map)
			delete m_map;
	}

	void draw(Utils::GLSLShader* shader, int primitive);

	inline typename PFP::MAP* getMap() { return static_cast<typename PFP::MAP*>(m_map); }

	void updateBB(const VertexAttribute<typename PFP::VEC3>& position);
};

} // namespace SCHNApps

} // namespace CGoGN

#include "mapHandler.hpp"

#endif
