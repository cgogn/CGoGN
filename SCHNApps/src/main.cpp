#include <QSplashScreen>
#include "PythonQt/PythonQt.h"
#include "PythonQt/gui/PythonQtScriptingConsole.h"
#include "window.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	QSplashScreen* splash = new QSplashScreen(QPixmap(":splash/cgogn/splash.png"));
	splash->show();
	splash->showMessage("Welcome to SCHNApps", Qt::AlignBottom | Qt::AlignCenter);

	// init PythonQt and Python itself
	PythonQt::init();

	QStringList classNames;
	classNames.append("Plugin");
	classNames.append("View");
	classNames.append("MapHandlerGen");
	classNames.append("Camera");
	PythonQt::self()->registerQObjectClassNames(classNames);

	// get a smart pointer to the __main__ module of the Python interpreter
	PythonQtObjectPtr pythonContext = PythonQt::self()->getMainModule();
	PythonQtScriptingConsole pythonConsole(NULL, pythonContext);

	CGoGN::SCHNApps::Window schnapps(app.applicationDirPath(), pythonContext, pythonConsole);
	schnapps.show();

	pythonContext.addObject("schnapps", &schnapps);
	pythonContext.evalFile(app.applicationDirPath() + QString("/init.py"));

	splash->finish(&schnapps);
	delete splash;

	return app.exec();
}
