#include "system.h"

#include <QErrorMessage>
#include <QMessageBox>
#include <QFileInfo>
#include <QUrl>
#include <QVBoxLayout>
#include <QTextBrowser>

using namespace System;

Error::ERROR_CODE Error::code= SUCCESS;

QString Error::parameter= QString("???");

QString app_path= QString();

bool Events::movieDialogOpened= false;


void Error::showError(QWidget* parent){
	QString message= QString();

	switch (Error::code){
	case SUCCESS:
	{
		message= QString::fromUtf8("ERREUR - SUCESS: Aucun erreur n'est survenue.");

		break;
	}
	case NO_SCENE:
	{
		message= QString::fromUtf8("ERREUR - NO_SCENE: aucune scène n'a été référencée.\n"
				"Essayez de charger et/ou d'activer d'autres plugins au préalable");

		break;
	}
	case SCENE_UNREFERENCED:
	{
		message= QString::fromUtf8("ERREUR - SCENE_UNREFERENCED: la scène désignée n'existe pas ou n'a pas été référencé.\n"
				"Essayez de charger et/ou d'activer d'autres plugins au préalable");

		break;
	}
	case SCENE_EXISTS:
	{
		message= QString::fromUtf8("ERREUR - SCENE_EXISTS: une Scene de même nom existe déjà.\n"
				"Un même plugin a peut-être été chargé 2 fois sinon 2 plugins sont incompatibles");

		break;
	}
	case NO_DOCK:
	{
		message= QString::fromUtf8("ERREUR - NO_DOCK: aucun dock n'a été préalablement définit\n"
				"Un plugin en charge de la création du dock est peut-être manquant");
		break;
	}
	case NO_PLUGIN_PATH_FILE:
	{
		message= QString::fromUtf8("ERREUR - NO_PLUGIN_PATH_FILE: fichier de localisation des plugins manquant\n"
				"Le fichier \"state_save.xml\" n'a pas put être trouvé, ce qui empêche la le chargement des plugins");
		break;
	}
	case ERROR_OPEN_PLUGIN_FILE:
	{
		message= QString::fromUtf8("Erreur - ERROR_OPEN_PLUGIN_FILE: erreur lors de l'ouverture du fichier de plugin\n"
				"Le fichier \"state_save.xml\" n'a pas put être ouvert. Avez-vous les droits?");
		break;
	}
	case BAD_PLUGIN_PATH_FILE:
	{
		message= QString::fromUtf8("ERREUR - BAD_PLUGIN_PATH_FILE: erreur dans lefichier de localisation des plugins\n"
				"Il y a eu une erreur dans le fichier \"state_save.xml\", celui-ci est peut être mal"
				"construit. L'avez-vous édité manuellement?");
		break;
	}
	case BAD_PLUGIN_PATH_IN_FILE:
	{
		message= QString::fromUtf8("ERREUR - BAD_PLUGIN_PATH_IN_FILE: mauvais chemin dans \"state_save.xml\""
				"Un ou plusieurs chemin indiqué dans le fichier \"state_save.xml\" est erroné."
				"Un des plugin, ou un des répertoires contenant, a peut-être été déplacé, renommé"
				"ou non-compilé. Le fichier est peut-être corrompu.\n"
				"Ficher(s) incriminé(s): ")+ parameter;
		break;
	}
	case NO_PLUGIN_IN_DIR:
	{
		message= QString::fromUtf8("ERREUR - NO_PLUGIN_IN_DIR: aucun plugin trouvé dans le répertoire.\n"
				"Un ou plusieurs répertoire censé contenir des fichiers plugin n'en contient pas."
				"Les plugins ont peut-être été supprimés, déplacés ou renommés."
				"Le répertoire indiqué est peut-être mauvais.\n"
				"Répertoire incriminé: ") + parameter;

		break;
	}
	case PLUGIN_EXISTS:
	{
		message= QString::fromUtf8("ERREUR - PLUGIN_EXISTS: le plugin existe déjà.\n"
				"Le plugin que vous essayé de référencer existe déjà. Soit un plugin"
				"de même nom à déjà été chargé, soit le plugin a été chargé 2 fois.\n"
				"Plugin concerné: ") + parameter;
		break;
	}
	case ERROR_PLUGIN_LOAD:
	{
		message= QString::fromUtf8("ERREUR - PLUGIN_LOAD: erreur lors du chargement d'un plugin.\n"
				"Un plugin n'a pas put être chargé. Peut-être que celui-ci n'est pas compatible,"
				"ou un mauvais fichier a été chargé.\n"
				"Plguin concerné:") + parameter;
		break;
	}
	case BAD_LINK_PLUGIN_WINDOW:
	{
		message= QString::fromUtf8("ERREUR - BAD_LINK_PLUGIN_WINDOW: liens du plugin avec l'application inexistant.\n"
				"Le plugin ne peut établir le liens avec l'application. Peut-être que celui-ci effectue"
				"certaines opérations impossible avant l'initialisation complète de ce-dernier.\n"
				"Plugin concerné: ") + parameter;

		break;
	}
	case UNSATSIFIED_PLUGIN_DEPENDENCIE:
	{
		message= QString::fromUtf8("ERREUR - UNSATSIFIED_PLUGIN_DEPENDENCIE: dépendance de plugin instatisfaite.\n"
				"Une dépendance entre plugin n'a put être satisfaite. Certains plugins ne sont peut-être pas chargés.\n"
				"Plugin concernés: " ) +  parameter;

		break;
	}
	case BAD_ACTION_MENU_PATH:
	{
		message= QString::fromUtf8("ERREUR - BAD_ACTION_MENU_PATH: mauvais chemin de menu pour l'ajout d'action.\n"
				"Le chemin indiqué pour la création d'une action dans le menu est mauvais. Peut-être est-il "
				"vide? Une action doit être encapsulée dans un menu.\n"
				"Chemin erroné: ") + parameter;

		break;
	}
	case MAP_UNREFERENCED:
	{
		message= QString::fromUtf8("ERREUR - MAP_UNREFERENCED: la map recherchée n'existe pas.\n"
				"Peut-être que certains plugins n'ont pas été chargés, ou le nom de map spécifié"
				"est mauvais.\n"
				"Map inexistante: ") + parameter;

		break;
	}
	case MAP_EXISTS:
	{
		message= QString::fromUtf8("ERREUR - MAP_EXISTS: la map existe déjà.\n"
				"Vous avez essayé de creéer et référencer une map qui l'est déjà.\n"
				"Map déjà existante: ") + parameter;

		break;
	}
	case SHADER_EXISTS:
	{
		message= QString::fromUtf8("ERREUR - SHADER_EXISTS: le shader existe déjà.\n"
				"Un shader a essayer d'être référencé sous un nom déjà utilisé dans la vue"
				"concernée.\n"
				"Objets concernés: ") + parameter;

		break;
	}
	case SHADER_NOT_FOUND:
	{
		message= QString::fromUtf8("ERREUR - SHADER_NOT_FOUND: le shader n'existe pas.\n"
				"Le programme a essayé d'accéder à un shader non référencé. Le nom de référencement"
				"était peut-être mauvais\n"
				"Objets concernés: ") + parameter;

		break;
	}
	case VBO_EXISTS:
	{
		message= QString::fromUtf8("ERREUR - VBO_EXISTS: le vbo existe déjà.\n"
				"Un vbo a essayer d'être référencé sous un nom déjà utilisé dans la vue"
				"concernée.\n"
				"Objets concernés: ") + parameter;

		break;
	}
	case VBO_NOT_FOUND:
	{
		message= QString::fromUtf8("ERREUR - VBO_NOT_FOUND: le vbo n'existe pas.\n"
				"Le programme a essayé d'accéder à un shader non référencé. Le nom de référencement"
				"était peut-être mauvais\n"
				"Objets concernés: ") + parameter;

		break;
	}
	case TOPO3RENDER_EXISTS:
	{
		message= QString::fromUtf8("ERREUR - TOPO3RENDER_EXISTS: le topo3Render existe déjà.\n"
				"Un topo3Render a essayer d'être référencé sous un nom déjà utilisé dans la vue"
				"concernée.\n"
				"Objets concernés: ") + parameter;

		break;
	}
	case TOPO3RENDER_NOT_FOUND:
	{
		message= QString::fromUtf8("ERREUR - TOPO3RENDER_NOT_FOUND: le topo3Render n'existe pas.\n"
				"Le programme a essayé d'accéder à un topo3Render non référencé. Le nom de référencement"
				"était peut-être mauvais\n"
				"Objets concernés: ") + parameter;

		break;
	}
	case OPEN_FILE_ERROR:
	{
		message= QString::fromUtf8("ERREUR - OPEN_FILE_ERROR: le fichier n'a pas put être ouvert.\n"
				"Le chemin est peut-être mauvaise, ou vous ne pouvez pas lire ce fichier...\n"
				"Fichiers concernés: ") + parameter;
		break;
	}
	case BAD_XML_FILE:
	{
		message= QString::fromUtf8("ERREUR - BAD_XML_FILE: le fichier xml est mal construit.\n"
				"Il y'a eu une erreur lors de la lecture du fichier XML, celui-ci est peut-être mal construit.\n"
				"Fichiers concernés: ") + parameter;
		break;
	}
	default :{
		message= QString::fromUtf8("ERREUR - UNKNOWN ERROR: erreur inconnue");

		break;
	}
	}

	if(!parameter.isEmpty()) parameter= QString("???");

	QMessageBox err;
	err.setText(message);
	err.exec();

	code= SUCCESS;
}

Error::ERROR_CODE Error::BAD_PLUGIN_PATH_IN_FILE_f(QString file){
	if(code!=BAD_PLUGIN_PATH_IN_FILE){
		parameter.clear();
	}

	parameter+= " "+file;

	return BAD_PLUGIN_PATH_IN_FILE;
}

Error::ERROR_CODE Error::NO_PLUGIN_IN_DIR_f(QString dir){
	if(code!=NO_PLUGIN_IN_DIR){
			parameter.clear();
	}

	parameter+= " "+dir;

	return NO_PLUGIN_IN_DIR;
}

Error::ERROR_CODE Error::PLUGIN_EXISTS_f(QString plugin){
	if(code!=PLUGIN_EXISTS){
		parameter.clear();
	}

	parameter+= " "+plugin;

	return PLUGIN_EXISTS;
}

Error::ERROR_CODE Error::ERROR_PLUGIN_LOAD_f(QString plugin){
	if(code!=ERROR_PLUGIN_LOAD){
		parameter.clear();
	}

	parameter+= " "+plugin;

	return ERROR_PLUGIN_LOAD;
}

Error::ERROR_CODE Error::BAD_LINK_PLUGIN_WINDOW_f(QString plugin){
	if(code!=BAD_LINK_PLUGIN_WINDOW){
		parameter.clear();
	}

	parameter+= " "+plugin;

	return BAD_LINK_PLUGIN_WINDOW;
}

Error::ERROR_CODE Error::UNSATSIFIED_PLUGIN_DEPENDENCIE_f(QString dependencie, QString from){
	if(code!=UNSATSIFIED_PLUGIN_DEPENDENCIE){
		parameter.clear();
	}

	parameter+= " " + dependencie + ((from.isEmpty())?(" from " + from):("")) + ';';

	return UNSATSIFIED_PLUGIN_DEPENDENCIE;
}

Error::ERROR_CODE Error::BAD_ACTION_MENU_PATH_f(QString actionPath){
	if(code!=BAD_ACTION_MENU_PATH){
		parameter.clear();
	}

	parameter+= " " +actionPath;

	return BAD_ACTION_MENU_PATH;
}

Error::ERROR_CODE Error::MAP_UNREFERENCED_f(QString map_name){
	if(code!=MAP_UNREFERENCED){
		parameter.clear();
	}

	parameter+= " " + map_name;

	return MAP_UNREFERENCED;
}

Error::ERROR_CODE Error::MAP_EXISTS_f(QString map_name){
	if(code!=MAP_EXISTS){
		parameter.clear();
	}

	parameter+= " " + map_name;

	return MAP_EXISTS;
}

Error::ERROR_CODE Error::SHADER_EXISTS_f(QString shader_name, QString glv_name){
	if(code!=SHADER_EXISTS){
		parameter.clear();
	}

	parameter+= " " + shader_name + ((glv_name.isEmpty())?"":(" in view "+glv_name));

	return SHADER_EXISTS;
}
Error::ERROR_CODE Error::SHADER_NOT_FOUND_f(QString shader_name, QString glv_name){
	if(code!=SHADER_NOT_FOUND){
		parameter.clear();
	}

	parameter+= " " + shader_name + ((glv_name.isEmpty())?"":(" in view "+glv_name));

	return SHADER_NOT_FOUND;
}

Error::ERROR_CODE Error::VBO_EXISTS_f(QString vbo_name, QString glv_name){
	if(code!=VBO_EXISTS){
		parameter.clear();
	}

	parameter+= " " + vbo_name + ((glv_name.isEmpty())?"":(" in view "+glv_name));

	return VBO_EXISTS;
}
Error::ERROR_CODE Error::VBO_NOT_FOUND_f(QString vbo_name, QString glv_name){
	if(code!=VBO_NOT_FOUND){
		parameter.clear();
	}

	parameter+= " " + vbo_name + ((glv_name.isEmpty())?"":(" in view "+glv_name));

	return VBO_NOT_FOUND;
}

Error::ERROR_CODE Error::TOPO3RENDER_EXISTS_f(QString name, QString glv_name){
	if(code!=TOPO3RENDER_EXISTS){
		parameter.clear();
	}

	parameter+= " " + name + ((glv_name.isEmpty())?"":(" in view "+glv_name));

	return TOPO3RENDER_EXISTS;
}

Error::ERROR_CODE Error::TOPO3RENDER_NOT_FOUND_f(QString name, QString glv_name){
	if(code!=TOPO3RENDER_NOT_FOUND){
		parameter.clear();
	}

	parameter+= " " + name + ((glv_name.isEmpty())?"":(" in view "+glv_name));

	return TOPO3RENDER_NOT_FOUND;
}

Error::ERROR_CODE Error::OPEN_FILE_ERROR_f(QString filepath){
	if(code!=OPEN_FILE_ERROR){
		parameter.clear();
	}

	parameter+= " " + filepath;

	return OPEN_FILE_ERROR;
}

Error::ERROR_CODE Error::BAD_XML_FILE_f(QString filepath){
	if(code!=BAD_XML_FILE){
		parameter.clear();
	}

	parameter+= " " + filepath;

	return BAD_XML_FILE;
}

void Info::showPluginInfo(QString pluginAbsolutePath, QWidget* parent){
	QFileInfo fileInfo(pluginAbsolutePath);
	if(fileInfo.exists()){
		QString baseName= fileInfo.baseName();
		QString path= fileInfo.absolutePath();
		int i= baseName.indexOf("lib");
		QString newName(baseName);
		if(i==0){
			newName= newName.replace(0,3,"");
		}

		showPluginInfo(path, newName);
	}
	else{
		QMessageBox msg(parent);
		msg.setText(QString::fromUtf8("Le créateur de ce plugin n'a pas jugé utile de"
				" fournir un fichier d'information pour ce plugin."));
		msg.exec();
	}

}

void Info::showPluginInfo(QString locationPath, QString pluginName, QWidget* parent){
	QString newPath= locationPath+"/"+pluginName+".html";

	QUrl url= QUrl::fromLocalFile(newPath);
	if(QFileInfo(newPath).exists()){
		Dialog::InfoDialog id(parent);
		if(!id.setContent(url)){
			QMessageBox msg(parent);
			msg.setText(QString::fromUtf8("Le créateur de ce plugin n'a pas jugé utile de"
					" fournir un fichier d'information pour ce plugin."));
			msg.exec();
		}
		else{
			id.exec();
		}
	}
	else{
		QMessageBox msg(parent);
		msg.setText(QString::fromUtf8("Le créateur de ce plugin n'a pas jugé utile de"
				" fournir un fichier d'information pour ce plugin."));
		msg.exec();
	}
}


bool Dialog::InfoDialog::setContent(QUrl urlHTMLFile){
	if(urlHTMLFile.isLocalFile()){
		textBrowser->setSource(urlHTMLFile);

		return true;
	}
	return false;
}
