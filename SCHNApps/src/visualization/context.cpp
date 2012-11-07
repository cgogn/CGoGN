#include "context.h"





Context::Context(QWidget* w, const QGLFormat & format) : QGLContext(format, w){}



void Context::setDevice(QWidget* w){
	QGLContext::setDevice(w);
}

