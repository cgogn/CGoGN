#ifndef _TYPES_H_
#define _TYPES_H_

#include <QHash>
#include <QString>

class Window;
class Plugin;
class Scene;
class View;
class Camera;
class Context;

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
typedef QHash<QString, View*> ViewHash;
typedef QHash<QString, Camera*> CameraHash;

typedef QHash<QString, MapHandler*> MapHash;
typedef QHash<QString, CGoGN::Utils::VBO*> VBOHash;
typedef QHash<QString, CGoGN::Utils::GLSLShader*> ShaderHash;

namespace Qt
{
	extern int UserRoleType;
}

#endif
