#include <QSplashScreen>
#include "PythonQt/PythonQt.h"
#include "PythonQt/PythonQtScriptingConsole.h"
#include "window.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	QSplashScreen* splash = new QSplashScreen(QPixmap(":splash/cgogn/splash.png"));
	splash->show();
	splash->showMessage("Welcome to SCHNApps", Qt::AlignBottom | Qt::AlignCenter);

	// init PythonQt and Python itself
	PythonQt::init();

	// get a smart pointer to the __main__ module of the Python interpreter
	PythonQtObjectPtr pythonContext = PythonQt::self()->getMainModule();

	CGoGN::SCHNApps::Window window(app.applicationDirPath(), pythonContext);
	window.show();

	pythonContext.addObject("window", &window);

	splash->finish(&window);
	delete splash;

	PythonQtScriptingConsole pythonConsole(NULL, pythonContext);
	pythonConsole.show();

	return app.exec();
}
