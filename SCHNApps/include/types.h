#ifndef _TYPES_H_
#define _TYPES_H_

#include <QMap>
#include <QString>

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"
#include "Topology/generic/attributeHandler.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"

namespace CGoGN
{

namespace Utils
{
class VBO;
class GLSLShader;
}

struct PFP_SCHNAPPS
{
	typedef float REAL;
	typedef Geom::Vector<3,REAL> VEC3;
	typedef Geom::Vector<4,REAL> VEC4;
	typedef Geom::Vector<6,REAL> VEC6;
	typedef Geom::Matrix<3,3,REAL> MATRIX33;
	typedef Geom::Matrix<4,4,REAL> MATRIX44;
	typedef Geom::Matrix<3,6,REAL> MATRIX36;

	static inline Geom::Vec3f toVec3f(const VEC3& P)
	{
		return P;
	}
};

struct PFP2 : public PFP_SCHNAPPS
{
	typedef EmbeddedMap2 MAP;
};

struct PFP3 : public PFP_SCHNAPPS
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
class CellSelectorGen;
struct Texture;

typedef QMap<QString, Plugin*> PluginSet;
typedef QMap<QString, View*> ViewSet;
typedef QMap<QString, Camera*> CameraSet;
typedef QMap<QString, MapHandlerGen*> MapSet;
typedef QMap<QString, Utils::VBO*> VBOSet;
typedef QMap<QString, QString> AttributeSet;
typedef QMap<QString, CellSelectorGen*> CellSelectorSet;
typedef QMap<QString, Utils::GLSLShader*> ShaderSet;
typedef QMap<QString, Texture*> TextureSet;

} // namespace SCHNApps

} // namespace CGoGN

#endif
