#ifndef _VBOHANDLER_H_
#define _VBOHANDLER_H_

#include "Utils/vbo.h"

class VBOHandler
{
public:
	VBOHandler(const QString& name, CGoGN::Utils::VBO* vbo) :
		m_name(name),
		m_vbo(vbo)
	{}

	~VBOHandler()
	{
		delete m_vbo;
	}

	const QString& getName() { return m_name; }
	void setName(const QString& name) { m_name = name; }

	CGoGN::Utils::VBO* getVBO() { return m_vbo; }

protected:
	QString m_name;
	CGoGN::Utils::VBO* m_vbo;
};

#endif
