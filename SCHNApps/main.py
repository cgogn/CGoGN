
import sys
from PyQt4 import QtGui
import PySCHNApps


if __name__ == '__main__':
	app = QtGui.QApplication(sys.argv)

	window = PySCHNApps.Window()
	window.launch()
#	window.loadPlugin("/home/kraemer/Dev/CGoGN/SCHNApps/lib/libRenderPlugin.so")

	sys.exit(app.exec_())
