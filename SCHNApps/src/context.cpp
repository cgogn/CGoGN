#include "context.h"

Context::Context(const QGLFormat& format) : QGLContext(format)
{}

void Context::setDevice(QWidget* w)
{
	QGLContext::setDevice(w);
}
