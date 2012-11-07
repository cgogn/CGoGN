#ifndef _TYPES_H_
#define _TYPES_H_


#include <GL/glew.h>
#include <QGLViewer/qglviewer.h>

#include <QHash>
#include <QString>

class Plugin;
class Scene;
namespace CGoGN { class GenericMap; }

#define PluginHash QHash<QString, Plugin*>
#define SceneHash QHash<QString, Scene*>

/**
 * \def MapHash
 *
 * Preprocessor macro for simplifying the type "QHash<QString, GenericMap*>" into GLViewerHash, within the code
 *
 */
//#define MapHash QHash<QString,CGoGN::GenericMap*>

/**
 * \def VBOHash
 * \brief Preprocessor macro for simplifying the type "QHash<QString, Utils::VBO*>" into GLViewerHash, within the code
 */
#define VBOHash QHash<QString, CGoGN::Utils::VBO*>
/**
 * \def ShaderHash
 * \brief Preprocessor macro for simplifying the type "QHash<QString, Utils::GLSLShader*>" into GLViewerHash, within the code
 */
#define ShaderHash QHash<QString, CGoGN::Utils::GLSLShader*>
/**
 * \def ShaderHash
 * \brief Preprocessor macro for simplifying the type "QHash<QString, Algo::Render::GL2::Topo3Render*>" into GLViewerHash, within the code
 */
#define Topo3RenderHash QHash<QString, Algo::Render::GL2::Topo3Render*>

#define MapHash QHash<QString, MapHandler*>

namespace Qt{
	extern int UserRoleType;
}


#endif
