#ifndef _VIZUHANDLER_H_
#define _VIZUHANDLER_H_


#include <QHash>



namespace CGoGN {
	class GenericMap;
	namespace Utils{
		class VBO;
	}
}

class VBOHandler;

class MapHandler{
public:
	MapHandler(CGoGN::GenericMap* map);
	~MapHandler();

	CGoGN::GenericMap* map(){ return m_map;}

	VBOHandler* findVBO(QString name);
	VBOHandler* findFirstVBOMatching(QRegExp regexp);
	QList<VBOHandler*> findVBOsMatching(QRegExp regexp);

	bool addVBO(VBOHandler* vboH);
	VBOHandler* addNewVBO(QString vboName);

	VBOHandler* takeVBO(VBOHandler* vbo);

	int countVBO(){ return l_vbo.count();}

protected:
	CGoGN::GenericMap* m_map;
	QList<VBOHandler*> l_vbo;
};


#endif
