#ifndef _MAPHANDLER_H_
#define _MAPHANDLER_H_

#include <QString>
#include "types.h"

#include "view.h"

namespace CGoGN
{
	class GenericMap;
	namespace Utils
	{
		class VBO;
	}
}

class MapHandler : public QObject
{
	Q_OBJECT

public:
	MapHandler(const QString& name, Window* window, CGoGN::GenericMap* map);
	~MapHandler();

	const QString& getName() { return m_name; }
	void setName(const QString& name) { m_name = name; }

	Window* getWindow() { return m_window; }
	void setWindow(Window* w) { m_window = w; }

	CGoGN::GenericMap* getMap() { return m_map; }

	CGoGN::Utils::VBO* addVBO(const QString& name);
	void removeVBO(const QString& name);
	CGoGN::Utils::VBO* getVBO(const QString& name);
	CGoGN::Utils::VBO* findFirstVBOMatching(const QRegExp& regexp);
	QList<CGoGN::Utils::VBO*> findVBOsMatching(const QRegExp& regexp);

	int getNbVBO() { return h_vbo.count(); }

	/*********************************************************
	 * MANAGE LINKED VIEWS
	 *********************************************************/

	bool linkView(View* view);
	void unlinkView(View* view);
	bool isLinkedToView(View* view) { return l_views.contains(view); }
	QList<View*> getLinkedViews() { return l_views; }

protected:
	QString m_name;
	Window* m_window;
	CGoGN::GenericMap* m_map;

	QList<View*> l_views;

	VBOHash h_vbo;

public slots:
	void cb_viewRemoved(View* view);
};

#endif
