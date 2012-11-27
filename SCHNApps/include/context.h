#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include "types.h"

#include <QGLContext>

class Context : public QGLContext
{
public:
	Context(const QGLFormat& format, QWidget* w);

	void setDevice(QWidget* w);
};

#endif
