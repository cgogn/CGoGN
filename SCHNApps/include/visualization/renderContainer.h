#ifndef _RENDERCONTAINER_H_
#define _RENDERCONTAINER_H_


#include "Utils/GLSLShader.h"
#include "Algo/Render/GL2/topo3Render.h"
#include "Utils/vbo.h"

#include "types.h"
#include "system.h"

#include <QList>
#include <QHash>


//forward declaration
class Scene;

using namespace CGoGN;


class RenderContainer{
public:
	RenderContainer(Scene* scene=0);
	~RenderContainer();

	void associateGLViewer(Scene* scene);
	void removeGLViewer(Scene* scene);
	bool isLinked();

	bool addNewVBO(QString name, Utils::VBO* &vbo);
	bool addVBO(QString name, Utils::VBO* vbo);
	Utils::VBO* getVBO(QString name);
	void eraseVBO(QString name);

	bool addAndRegisterShader(QString name, Utils::GLSLShader* shader);

	template<typename T>
	T* getShader(QString name){
		if(h_shader){
			ShaderHash::iterator it;
			if((it=h_shader->find(name))!=h_shader->end()){
				return (T*)(*it);
			}
			else{
				System::Error::code= System::Error::SHADER_NOT_FOUND_f(name);

				return NULL;
			}
		}
		else{
			System::Error::code= System::Error::SHADER_NOT_FOUND_f(name);
			return NULL;
		}
	}

	QList<Utils::GLSLShader*> registeredShaders();

	void eraseAndUnregisterShader(QString name);


	bool addNewTopo3Render(QString name, Algo::Render::GL2::Topo3Render* &topo3Render);
	Algo::Render::GL2::Topo3Render* getTopo3Render(QString name);
	void eraseTopo3Render(QString name);

protected:
	QList<Scene*> l_scene;

	VBOHash* h_vbo;
	ShaderHash* h_shader;
	Topo3RenderHash* h_topo3render;
};

#endif
