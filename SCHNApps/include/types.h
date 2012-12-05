#ifndef _TYPES_H_
#define _TYPES_H_

#include <QHash>
#include <QMap>
#include <QString>

class Window;
class Plugin;
class View;
class Camera;
class Texture;

class MapHandlerGen;

namespace CGoGN
{
	namespace Utils
	{
		class VBO;
		class GLSLShader;
	}
}

typedef QHash<QString, Plugin*> PluginHash;
typedef QHash<QString, View*> ViewHash;
typedef QHash<QString, Camera*> CameraHash;

typedef QHash<QString, MapHandlerGen*> MapHash;
typedef QMap<std::string, CGoGN::Utils::VBO*> VBOHash;
typedef QHash<QString, CGoGN::Utils::GLSLShader*> ShaderHash;

typedef QHash<QString, Texture*> TextureHash;

namespace Qt
{
	extern int UserRoleType;
}

#endif
