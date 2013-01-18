#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

#include "plugin.h"
#include "window.h"

using namespace boost::python;

BOOST_PYTHON_MODULE(PySCHNApps)
{
	class_<CGoGN::SCHNApps::Window>("Window")
		.def(init<>())
        .def("launch", &CGoGN::SCHNApps::Window::launch)
        .def("loadPlugin", &CGoGN::SCHNApps::Window::loadPlugin_py)
	;
}
