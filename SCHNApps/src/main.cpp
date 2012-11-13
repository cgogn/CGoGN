#include "system.h"

#include "window.h"

QSplashScreen* System::splash;

QString System::app_path= QString();

int main(int argc, char* argv[]){
	QApplication app(argc, argv);
	
	System::app_path= app.applicationDirPath();


	System::splash= new QSplashScreen(QPixmap(":splash/cgogn/splash.png"));

	System::splash->show();

	Window window;
	window.showMaximized();

	sleep(1);
	System::splash->finish(&window);

	delete System::splash;
	
	return app.exec();
}
