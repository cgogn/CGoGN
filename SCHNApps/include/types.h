#ifndef _TYPES_H_
#define _TYPES_H_

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

class SCHNApps;
class Plugin;
class PluginInteraction;
class PluginProcessing;
class View;
class Camera;
class MapHandlerGen;
struct Texture;

typedef QMap<QString, Plugin*> PluginSet;
typedef QMap<QString, View*> ViewSet;
typedef QMap<QString, Camera*> CameraSet;
typedef QMap<QString, MapHandlerGen*> MapSet;
typedef QMap<QString, Utils::VBO*> VBOSet;
typedef QMap<QString, QString> AttributeSet;
typedef QMap<QString, Utils::GLSLShader*> ShaderSet;

typedef QMap<QString, Texture*> TextureSet;

} // namespace SCHNApps

} // namespace CGoGN

#endif
