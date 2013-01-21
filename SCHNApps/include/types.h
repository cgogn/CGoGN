#ifndef _TYPES_H_
#define _TYPES_H_

#include <QHash>
#include <QMap>
#include <QString>

#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"

namespace CGoGN
{

namespace Utils
{
class VBO;
class GLSLShader;
}

struct PFP2: public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

struct PFP3: public PFP_STANDARD
{
	typedef EmbeddedMap3 MAP;
};

namespace SCHNApps
{

class Plugin;
class View;
class Camera;
class MapHandlerGen;
struct Texture;

typedef QHash<QString, Plugin*> PluginHash;
typedef QHash<QString, View*> ViewHash;
typedef QHash<QString, Camera*> CameraHash;

typedef QHash<QString, MapHandlerGen*> MapHash;
typedef QMap<QString, Utils::VBO*> VBOHash;
typedef QHash<QString, Utils::GLSLShader*> ShaderHash;

typedef QHash<QString, Texture*> TextureHash;

} // namespace SCHNApps

} // namespace CGoGN

#endif
