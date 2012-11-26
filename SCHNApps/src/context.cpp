#include "context.h"

Context::Context(const QGLFormat& format, QWidget* w) : QGLContext(format, w)
{}

void Context::setDevice(QWidget* w)
{
	QGLContext::setDevice(w);
}
