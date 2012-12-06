#include "system.h"
#include "window.h"

//QSplashScreen* System::splash;

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	QSplashScreen* splash = new QSplashScreen(QPixmap(":splash/cgogn/splash.png"));
	splash->show();
	splash->showMessage("Welcome to SCHNApps", Qt::AlignBottom | Qt::AlignCenter);

	Window window;
	window.setAppPath(app.applicationDirPath());
	window.show();

	splash->finish(&window);
	delete splash;
	
	return app.exec();
}
