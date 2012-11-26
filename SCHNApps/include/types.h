#ifndef _TYPES_H_
#define _TYPES_H_

#include <GL/glew.h>
#include <QGLViewer/qglviewer.h>

#include <QHash>
#include <QString>

class Plugin;
class Scene;
class MapHandler;
namespace CGoGN
{
	namespace Utils
	{
		class VBO;
		class GLSLShader;
	}
}

typedef QHash<QString, Plugin*> PluginHash;
typedef QHash<QString, Scene*> SceneHash;
typedef QHash<QString, MapHandler*> MapHash;
typedef QHash<QString, CGoGN::Utils::VBO*> VBOHash;
typedef QHash<QString, CGoGN::Utils::GLSLShader*> ShaderHash;

namespace Qt
{
	extern int UserRoleType;
}

#endif
