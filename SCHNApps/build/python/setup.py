from distutils.core import setup, Extension

ext = Extension(
	'PySCHNApps',
	library_dirs=['/usr/lib', 'usr/local/lib', '../../lib'],
	libraries=['python2.7','boost_python-py27', 'SCHNApps'],
	include_dirs=['../../include', '../../build/Release', '../../../include', '/usr/include/qt4', '/usr/include/libxml2'],
	sources = ['window_py.cpp']
)

setup(
	name = 'PySCHNApps',
	version = '1.0',
	description = 'SCHNApps from Python',
	ext_modules = [ext]
)
