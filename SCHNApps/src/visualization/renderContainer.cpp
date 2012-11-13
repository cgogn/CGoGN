#include "renderContainer.h"

#include "scene.h"

RenderContainer::RenderContainer(Scene* scene) :
	h_vbo(NULL),
	h_shader(NULL),
	h_topo3render(NULL)
{
	l_scene.push_back(scene);
}

RenderContainer::~RenderContainer(){
	if(h_vbo){
		VBOHash::iterator it= h_vbo->begin();
		while(it!=h_vbo->end()){
			delete (*it);
			it= h_vbo->erase(it);
		}

		delete h_vbo;
		h_vbo=NULL;
	}

	if(h_shader){
		ShaderHash::iterator it= h_shader->begin();
		while(it!=h_shader->end()){
			delete (*it);
			it= h_shader->erase(it);
		}

		delete h_shader;
		h_shader=NULL;
	}

	if(h_topo3render){
		Topo3RenderHash::iterator it= h_topo3render->begin();
		while(it!=h_topo3render->end()){
			delete(*it);
			it= h_topo3render->erase(it);
		}

		delete h_topo3render;
		h_topo3render=NULL;
	}
}

void RenderContainer::associateGLViewer(Scene* scene){
	if(!l_scene.contains(scene)){
		l_scene.push_back(scene);
	}
}

void RenderContainer::removeGLViewer(Scene* scene){
	l_scene.removeAll(scene);
}

bool RenderContainer::isLinked(){
	return !l_scene.isEmpty();
}

bool RenderContainer::addNewVBO(QString name, Utils::VBO* &vbo){
	if(!h_vbo){
		h_vbo= new VBOHash();
	}
	VBOHash::iterator it;
	if((it=h_vbo->find(name))==h_vbo->end()){
		vbo= new Utils::VBO();
		h_vbo->insert(name, vbo);
		return true;
	}
	else{
		System::Error::code= System::Error::VBO_EXISTS_f(name);
		return false;
	}
}

bool RenderContainer::addVBO(QString name, Utils::VBO* vbo){
	if(!h_vbo){
		h_vbo= new VBOHash();
	}
	VBOHash::iterator it;
	if((it=h_vbo->find(name))==h_vbo->end()){
		h_vbo->insert(name, vbo);
		return true;
	}
	else{
		System::Error::code= System::Error::VBO_EXISTS_f(name);
		return false;
	}
}

Utils::VBO* RenderContainer::getVBO(QString name){
	if(h_vbo){
		VBOHash::iterator it;
		if((it=h_vbo->find(name))!=h_vbo->end()){
			return (*it);
		}
		else{
			System::Error::code= System::Error::VBO_NOT_FOUND_f(name);
			return NULL;
		}
	}
	else{
		System::Error::code= System::Error::VBO_NOT_FOUND_f(name);
		return NULL;
	}
}

void RenderContainer::eraseVBO(QString name){
	if(h_vbo){
		VBOHash::iterator it;
		if((it=h_vbo->find(name))!=h_vbo->end()){
			delete (*it);
			h_vbo->erase(it);
		}
		if(h_vbo->isEmpty()){
			delete h_vbo;
			h_vbo=NULL;
		}
	}
}

bool RenderContainer::addAndRegisterShader(QString name, Utils::GLSLShader* shader){
	if(!h_shader){
		h_shader= new ShaderHash();
	}
	ShaderHash::iterator it;
	if((it=h_shader->find(name))==h_shader->end()){
		h_shader->insert(name,shader);
		return true;
	}
	else{
		System::Error::code= System::Error::SHADER_EXISTS_f(name);

		return false;
	}
}

QList<Utils::GLSLShader*> RenderContainer::registeredShaders(){
//	return h_shader->values();
	QList<Utils::GLSLShader*> l = h_shader->values();
	if(h_topo3render){
		for(Topo3RenderHash::iterator it= h_topo3render->begin(); it!=h_topo3render->end(); ++it){
			l.push_back((Utils::GLSLShader*)((*it)->shader1()));
			l.push_back((Utils::GLSLShader*)((*it)->shader2()));
		}
	}

	return l;
}

void RenderContainer::eraseAndUnregisterShader(QString name){
	if(h_shader){
		ShaderHash::iterator it;
		if((it=h_shader->find(name))!=h_shader->end()){
			delete (*it);
			h_shader->erase(it);
		}
		if(h_shader->isEmpty()){
			delete h_shader;
			h_shader= NULL;
		}
	}
}

bool RenderContainer::addNewTopo3Render(QString name, Algo::Render::GL2::Topo3Render* &topo3Render){
	if(!h_topo3render){
		h_topo3render= new Topo3RenderHash();
	}
	Topo3RenderHash::iterator it;
	if((it=h_topo3render->find(name))==h_topo3render->end()){
		topo3Render= new Algo::Render::GL2::Topo3Render();
		h_topo3render->insert(name, topo3Render);
		return true;
	}
	else{
		System::Error::code= System::Error::TOPO3RENDER_EXISTS_f(name);

		return false;
	}
}

Algo::Render::GL2::Topo3Render* RenderContainer::getTopo3Render(QString name){
	if(h_topo3render){
		Topo3RenderHash::iterator it;
		if((it=h_topo3render->find(name))!=h_topo3render->end()){
			return (*it);
		}
		else{
			System::Error::code= System::Error::TOPO3RENDER_NOT_FOUND_f(name);
			return NULL;
		}
	}
	else{
		System::Error::code= System::Error::TOPO3RENDER_NOT_FOUND_f(name);
		return NULL;
	}
}

void RenderContainer::eraseTopo3Render(QString name){
	if(h_topo3render){
		Topo3RenderHash::iterator it;
		if((it=h_topo3render->find(name))!=h_topo3render->end()){
			delete (*it);
			h_topo3render->erase(it);
		}
		if(h_topo3render->isEmpty()){
			delete h_topo3render;
			h_topo3render=NULL;
		}
	}
}
