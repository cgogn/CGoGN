#ifndef __SCENE_GRAPH_
#define __SCENE_GRAPH_

#include <string>
#include "Geometry/matrix.h"
#include "Algo/Render/vbo_MapRender.h"


namespace CGoGN
{
namespace Utils
{
namespace SceneGraph
{


class Node
{
protected:
	std::string m_label;
public:
	Node():m_label("SceneGraph::Node") {}

	virtual ~Node() {}

	Node(const std::string& lab): m_label(lab) {}

	const std::string& getLabel() const { return m_label;}

	void setLabel(const std::string& lab) { m_label = lab ;}

	virtual void apply() = 0;


};

void render(Node* node)
{
	node->apply();
}

void eraseGraph(Node* node)
{
	node->apply();
}



class Group_Node: public Node
{
protected:
	std::list<Node*> m_children;
public:
	Group_Node(): Node("SceneGraph::Group_Node") {}

	Group_Node(const std::string& label): Node(label) {}

	virtual ~Group_Node()
	{
		std::cout<< "delete group"<<std::endl;
		for (std::list<Node*>::const_iterator it = m_children.begin(); it !=m_children.end(); ++it)
			delete *it;
	}

	const std::list<Node*>& getChildren() const { return m_children;}

	void addChild(Node* child) { m_children.push_back(child);}

	void removeChild(Node* child) {m_children.remove( child);}

	virtual void apply()
	{
		for (std::list<Node*>::const_iterator it = m_children.begin(); it !=m_children.end(); ++it)
			render(*it);
	}

};

class Transfo_Node: public Group_Node
{
protected:
		Geom::Matrix44f* m_matrix;
public:
		Transfo_Node():Group_Node("SceneGraph::Transfo_Node"),m_matrix(NULL) {}

		virtual ~Transfo_Node() {std::cout<< "delete group"<<std::endl; delete m_matrix; }

		void setMatrix(const Geom::Matrix44f& mat)
		{
			if (m_matrix == NULL)
				m_matrix = new Geom::Matrix44f(mat);
			*m_matrix = mat.transposed();
		}

		Geom::Matrix44f getMatrix() const { return m_matrix->transposed(); }

		virtual void apply()
		{
			if (m_matrix != NULL)
				glMultMatrixf(&((*m_matrix)(0,0)));

			Group_Node::apply();
		} // transpose ??
};


class Material_Node: public Group_Node
{
protected:
	Geom::Vec4f m_diffuse;
	Geom::Vec4f m_specular;
	Geom::Vec4f m_ambient;
	float m_shininess;
	Geom::Vec4f m_color;
	bool m_has_diffuse;
	bool m_has_specular;
	bool m_has_ambient;
	bool m_has_shininess;
	bool m_has_color;

public:
	Material_Node():Group_Node("SceneGraph::Material_Node"), m_has_diffuse(false), m_has_specular(false),m_has_ambient(false),m_has_shininess(false),m_has_color(false){}

	void setDiffuse(const Geom::Vec4f& v) { m_diffuse = v; m_has_diffuse=true;}

	void setSpecular(const Geom::Vec4f& v) { m_specular = v; m_has_specular=true;}

	void setAmbient(const Geom::Vec4f& v) { m_ambient = v; m_has_ambient=true;}

	void setShininess(float v) { m_shininess = v; m_has_shininess=true; }

	void setColor(const Geom::Vec4f& v) { m_color = v; m_has_color=true;}

	virtual void apply()
	{
		if (m_has_diffuse)
			glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,m_diffuse.data());
		if (m_has_specular)
			glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,m_specular.data());
		if (m_has_ambient)
			glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,m_ambient.data());
		if (m_has_shininess)
			glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,&m_shininess);
		if (m_has_color)
			glColor3fv(m_color.data());

		Group_Node::apply();
	}
};





class VBO_Node: public Node
{
protected:
	Algo::Render::VBO::MapRender_VBO* m_vbo;
	unsigned int m_primitives;
public:
	VBO_Node() : Node("SceneGraph::VBO_Node"), m_vbo(NULL), m_primitives(0) {}

	VBO_Node(Algo::Render::VBO::MapRender_VBO* vbo) : m_vbo(vbo) {}

	void setVBO(Algo::Render::VBO::MapRender_VBO* vbo) { m_vbo = vbo;}

	void setPrimitives(unsigned int p) { m_primitives |= p ;}
	void unsetPrimitives(unsigned int p) { m_primitives &= ~p ;}

	virtual void apply()
	{
		if (m_vbo != NULL)
		{
			if (m_primitives & Algo::Render::VBO::TRIANGLES)
			{
				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(1.0f, 1.0f);
				glEnable(GL_LIGHTING);
				m_vbo->draw(Algo::Render::VBO::TRIANGLES);
			}

			if (m_primitives & Algo::Render::VBO::LINES)
			{
				glDisable(GL_POLYGON_OFFSET_FILL);
				glDisable(GL_LIGHTING);
				m_vbo->draw(Algo::Render::VBO::LINES);
			}

			if (m_primitives & Algo::Render::VBO::POINTS)
			{
				glDisable(GL_POLYGON_OFFSET_FILL);
				glDisable(GL_LIGHTING);
				m_vbo->draw(Algo::Render::VBO::POINTS);
			}
			if (m_primitives & Algo::Render::VBO::FLAT_TRIANGLES)
			{
				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(1.0f, 1.0f);
				glEnable(GL_LIGHTING);
				m_vbo->draw(Algo::Render::VBO::FLAT_TRIANGLES);
			}
		}
	}
};



}
}
}

#endif
