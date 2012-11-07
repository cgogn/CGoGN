#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <QWidget>
#include <QSplashScreen>


#include <unistd.h>
#include <stdarg.h>
#include <iostream>

#include "types.h"

#include "ui_questionSessionDialog.h"
#include "ui_infoDialog.h"


class Window;
class SplitArea;

namespace System{

	namespace Dialog{
		class QuestionSessionDialog : public QDialog, public Ui::QSDialog{
			Q_OBJECT
		public:
			QuestionSessionDialog(QWidget* parent=0) : QDialog(parent){ this->setupUi(this);}
		};


		class InfoDialog : public QDialog, public Ui::InfoDialog{
			Q_OBJECT
		public:
			InfoDialog(QWidget* parent=0) : QDialog(parent){ this->setupUi(this);}

			bool setContent(QUrl urlHTMLFile);
		};
	}

	class Error{
	public:
		virtual ~Error() =0;

		typedef enum{SUCCESS, NO_SCENE, SCENE_UNREFERENCED, SCENE_EXISTS, NO_DOCK, NO_PLUGIN_PATH_FILE,
					ERROR_OPEN_PLUGIN_FILE ,BAD_PLUGIN_PATH_FILE, BAD_PLUGIN_PATH_IN_FILE, NO_PLUGIN_IN_DIR,
					PLUGIN_EXISTS, ERROR_PLUGIN_LOAD, BAD_LINK_PLUGIN_WINDOW, UNSATSIFIED_PLUGIN_DEPENDENCIE,
					BAD_ACTION_MENU_PATH, MAP_UNREFERENCED, MAP_EXISTS, SHADER_EXISTS, SHADER_NOT_FOUND,
					VBO_EXISTS, VBO_NOT_FOUND, TOPO3RENDER_EXISTS, TOPO3RENDER_NOT_FOUND, OPEN_FILE_ERROR,
					BAD_XML_FILE} ERROR_CODE;


		static QString parameter;

		static ERROR_CODE code;



		static ERROR_CODE BAD_PLUGIN_PATH_IN_FILE_f(QString file);
		static ERROR_CODE NO_PLUGIN_IN_DIR_f(QString dir);
		static ERROR_CODE PLUGIN_EXISTS_f(QString plugin);
		static ERROR_CODE ERROR_PLUGIN_LOAD_f(QString plugin);
		static ERROR_CODE BAD_LINK_PLUGIN_WINDOW_f(QString plugin);
		static ERROR_CODE UNSATSIFIED_PLUGIN_DEPENDENCIE_f(QString dependencie, QString from=QString());
		static ERROR_CODE BAD_ACTION_MENU_PATH_f(QString actionPath);
		static ERROR_CODE MAP_UNREFERENCED_f(QString map_name);
		static ERROR_CODE MAP_EXISTS_f(QString map_name);
		static ERROR_CODE SHADER_EXISTS_f(QString shader_name, QString glv_name=QString());
		static ERROR_CODE SHADER_NOT_FOUND_f(QString shader_name, QString glv_name=QString());
		static ERROR_CODE VBO_EXISTS_f(QString vbo_name, QString glv_name=QString());
		static ERROR_CODE VBO_NOT_FOUND_f(QString vbo_name, QString glv_name=QString());
		static ERROR_CODE TOPO3RENDER_EXISTS_f(QString name, QString glv_name=QString());
		static ERROR_CODE TOPO3RENDER_NOT_FOUND_f(QString name, QString glv_name=QString());
		static ERROR_CODE OPEN_FILE_ERROR_f(QString filepath);
		static ERROR_CODE BAD_XML_FILE_f(QString filepath);


		static void showError(QWidget* parent=0);
	};

	class Events{
	public:
		virtual ~Events() =0;

		static bool movieDialogOpened;
	};

	class Info{
	public:
		virtual ~Info() =0;

		static void showPluginInfo(QString pluginAbsolutePath, QWidget* parent=0);
		static void showPluginInfo(QString locationPath, QString pluginName, QWidget* parent=0);
	};

	class StateHandler{
	public:
		virtual ~StateHandler() =0;

		static bool saveState(Window* window, PluginHash* pluginHash, SceneHash* sceneHash, SplitArea* splitArea);
		static bool loadState(Window* window, PluginHash* pluginHash, SceneHash* sceneHash, SplitArea* splitArea);
		static void unsaveState();
		static bool savePluginsInfo(Window* window, PluginHash* pluginHash, QStringList paths);
	};



	typedef enum{GLVIEWER_IN_DOCK, SIMPLE_GLVIEWER} QGLV_CREATE_SETTINGS;

	extern QString app_path;

	extern QSplashScreen* splash;

}

#endif
