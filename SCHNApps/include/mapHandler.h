#ifndef _MAPHANDLER_H_
#define _MAPHANDLER_H_

#include <QString>
#include "types.h"

namespace CGoGN
{
	class GenericMap;
	namespace Utils
	{
		class VBO;
	}
}

class MapHandler
{
public:
	MapHandler(const QString& name, CGoGN::GenericMap* map);
	~MapHandler();

	const QString& getName() { return m_name; }
	void setName(const QString& name) { m_name = name; }

	CGoGN::GenericMap* getMap() { return m_map; }

	CGoGN::Utils::VBO* addVBO(const QString& name);
	void removeVBO(const QString& name);
	CGoGN::Utils::VBO* getVBO(const QString& name);
	CGoGN::Utils::VBO* findFirstVBOMatching(const QRegExp& regexp);
	QList<CGoGN::Utils::VBO*> findVBOsMatching(const QRegExp& regexp);

	int getNbVBO() { return h_vbo.count(); }

protected:
	QString m_name;
	CGoGN::GenericMap* m_map;

	VBOHash h_vbo;
};

#endif
