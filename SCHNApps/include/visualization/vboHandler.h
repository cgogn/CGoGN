#ifndef _VBOHANDLER_H_
#define _VBOHANDLER_H_

#include "Utils/vbo.h"

#include <QString>
#include <QList>

using namespace CGoGN;

using namespace Utils;

class Scene;
class MapHandler;

class VBOHandler : public VBO{
public:
	VBOHandler(QString name) : VBO(), m_name(name){}
	VBOHandler(VBO vbo, QString name) : VBO(vbo), m_name(name){}

	bool isShared(){return l_vizuHandler.size()>1;}
	bool shareWith(MapHandler* vh);
	void unshareWith(MapHandler* vh);

	QString getName(){return m_name;}

	bool operator==(QString const & name);
protected:
	QString m_name;
	QList<MapHandler*> l_vizuHandler;
};

#endif
