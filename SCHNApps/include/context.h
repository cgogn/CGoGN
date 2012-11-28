#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <QGLContext>

class Context : public QGLContext
{
public:
	Context(const QGLFormat& format);

	void setDevice(QWidget* w);
};

#endif
