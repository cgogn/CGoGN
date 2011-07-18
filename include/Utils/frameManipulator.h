/*******************************************************************************
 * CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
 * version 0.1                                                                  *
 * Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
 *                                                                              *
 * This library is free software; you can redistribute it and/or modify it      *
 * under the terms of the GNU Lesser General Public License as published by the *
 * Free Software Foundation; either version 2.1 of the License, or (at your     *
 * option) any later version.                                                   *
 *                                                                              *
 * This library is distributed in the hope that it will be useful, but WITHOUT  *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
 * for more details.                                                            *
 *                                                                              *
 * You should have received a copy of the GNU Lesser General Public License     *
 * along with this library; if not, write to the Free Software Foundation,      *
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
 *                                                                              *
 * Web site: http://cgogn.u-strasbg.fr/                                         *
 * Contact information: cgogn@unistra.fr                                        *
 *                                                                              *
 *******************************************************************************/


#ifndef __FRAMEMANIPULATOR_H_
#define __FRAMEMANIPULATOR_H_

#include "Utils/vbo.h"
#include "glm/glm.hpp"
//#include "Utils/Shaders/shaderColorPerVertex.h"
#include "Utils/Shaders/shaderSimpleColor.h"
#include "Utils/pickables.h"

namespace CGoGN
{

namespace Utils
{


//
//
//class LineDrawable
//{
//protected:
//
//	/**
//	 * VBO for position
//	 */
//	Utils::VBO* m_vboPos;
//
//	/**
//	 * Shader
//	 */
//	Utils::ShaderSimpleColor* m_shader;
//
//	/**
//	 * number of indices in vbo
//	 */
//	unsigned int m_nb;
//
//public:
//	/**
//	 * constructor
//	 */
//	LineDrawable();
//
//	/**
//	 * destructor
//	 */
//	virtual ~LineDrawable();
//
//	/**
//	 * set the color of drawing
//	 */
//	void setColor(const Geom::Vec4f& col);
//
//	/**
//	 * draw the Drawable at origin with size=1
//	 */
//	virtual void draw();
//
//	/**
//	 * picking
//	 * @param P camera point
//	 * @param V vector ray direction
//	 * @param epsilon distance epsilon for picking
//	 * @return code picking (0: nothing picked / != 0 something picked)
//	 */
//	virtual unsigned int pick(const Geom::Vec3f& P, const Geom::Vec3f& V, float epsilon=0.0f) = 0;
//
//};
//
//
//class Pickable
//{
//protected:
//	/**
//	 * type of drawable
//	 */
//	LineDrawable* m_drawable;
//
//	/**
//	 * transformation matrix
//	 */
//	glm::mat4 m_transfo;
//
//	/**
//	 * id of pickable
//	 */
//	unsigned int m_id;
//
//public:
//	/**
//	 * constructor
//	 * @param ld LineDrawable to use for drawing & picking
//	 */
//	Pickable(LineDrawable* ld, unsigned int id):
//		m_drawable(ld),m_transfo(1.0f), m_id(id)
//		{}
//
//	/**
//	 * picking
//	 * @param P camera point
//	 * @param V vector ray direction
//	 * @param epsilon distance epsilon for picking
//	 * @return code picking (0: nothing picked / != 0 something picked)
//	 */
//	unsigned int pick(const Geom::Vec3f& P, const Geom::Vec3f& V, float epsilon=0.0f);
//
//	/**
//	 * apply inverse transfo on picking ray
//	 */
//	static void invertPV(const Geom::Vec3f& P, const Geom::Vec3f& V, const glm::mat4& transfo, Geom::Vec3f& PP, Geom::Vec3f& VV);
//
//	/**
//	 * draw
//	 */
//	void draw() { m_drawable->draw();}
//
//	/**
//	 * return a ref on the transformation matrix
//	 */
//	glm::mat4&  transfo();
//
//	void rotate(float angle, const Geom::Vec3f& Axis);
//
//	void translate(const Geom::Vec3f& P);
//
//	void scale(const Geom::Vec3f& S);
//
//	/**
//	 * pick a vector of pickable and return the closest
//	 */
//	static unsigned int pick(const std::vector<Pickable*>& picks,const Geom::Vec3f& P, const Geom::Vec3f& V, Pickable*& res);
//
//	float distancefrom(const Geom::Vec3f& P);
//
//
//};
//
//
///**
// * Grid (-1,-1,0 ; 1,1,0)
// */
//class Grid: public LineDrawable
//{
//public:
//	/**
//	 * constructor
//	 * @param sub number of subdivision of grig
//	 */
//	Grid(unsigned int sub=5);
//
//	/**
//	 * change topo subdivision
//	 */
//	void changeTopo(unsigned int sub);
//
//	/**
//	 * picking
//	 */
//	unsigned int pick(const Geom::Vec3f& P, const Geom::Vec3f& V, float epsilon=0.0f);
//};
//
//
///**
// * Sphere of radius 1 drawon with lines
// */
//class Sphere: public LineDrawable
//{
//protected:
//	GLuint m_ind;
//public:
//	/**
//	 * constructor
//	 * @param par number of parallels
//	 * @param mer number of meridians
//	 */
//	Sphere(unsigned int par=5, unsigned int mer=5);
//
//	/**
//	 * change topo subdivision
//	 */
//	void changeTopo(unsigned int par, unsigned int mer);
//
//	/**
//	 * specific drawing function for sphere (with indexed vbos)
//	 */
//	void draw();
//
//	/**
//	 * picking
//	 */
//	unsigned int pick(const Geom::Vec3f& P, const Geom::Vec3f& V, float epsilon=0.0f);
//};
//
//
///**
// * Cone of radius 1 drawon with lines
// */
//class Cone: public Sphere
//{
//public:
//	/**
//	 * constructor
//	 * @param par number of parallels
//	 * @param mer number of meridians
//	 */
//	Cone(unsigned int par=5, unsigned int mer=5);
//
//	/**
//	 * change topo subdivision
//	 */
//	void changeTopo(unsigned int par, unsigned int mer);
//
//	/**
//	 * picking
//	 */
//	unsigned int pick(const Geom::Vec3f& P, const Geom::Vec3f& V, float epsilon=0.0f);
//};
//
///**
// * Cone of radius 1 drawon with lines
// */
//class Cylinder: public Sphere
//{
//public:
//	/**
//	 * constructor
//	 * @param par number of parallels
//	 * @param mer number of meridians
//	 */
//	Cylinder(unsigned int par=5, unsigned int mer=5);
//
//	/**
//	 * change topo subdivision
//	 */
//	void changeTopo(unsigned int par, unsigned int mer);
//
//	/**
//	 * picking
//	 */
//	unsigned int pick(const Geom::Vec3f& P, const Geom::Vec3f& V, float epsilon=0.0f);
//};
//
//
///**
// * Sphere of radius 1 drawon with lines
// */
//class Cube: public LineDrawable
//{
//protected:
//	GLuint m_ind;
//public:
//	/**
//	 * constructor
//	 * @param par number of parallels
//	 * @param mer number of meridians
//	 */
//	Cube(unsigned int sub=0);
//
//	/**
//	 * destructor
//	 */
//	~Cube();
//
//	/**
//	 * change topo subdivision
//	 */
//	void changeTopo(unsigned int sub);
//
//	/**
//	 * specific drawing function for sphere (with indexed vbos)
//	 */
//	void draw();
//
//	/**
//	 * picking
//	 */
//	unsigned int pick(const Geom::Vec3f& P, const Geom::Vec3f& V, float epsilon=0.0f);
//};
//
//


class FrameManipulator//: public Pickable
{
public:
	enum AXIS {NONE=0, CENTER, Xt, Yt, Zt, Xr, Yr, Zr, Xs, Ys, Zs};

protected:

	/**
	 * number of segment for circle drawing
	 */
	static const unsigned int nb_segments = 64;

	static const float ring_half_width = 0.08f;


	/**
	 * locking table
	 */
	bool m_locked_axis[11];

	/**
	 * VBO for position
	 */
	Utils::VBO* m_vboPos;

	/**
	 * VBO for color
	 */
	Utils::VBO* m_vboCol;

	/**
	 * Shader
	 */
	Utils::ShaderSimpleColor* m_shader;

	/**
	 * the axis to be highlighted
	 */
	unsigned int m_highlighted;

	/**
	 * Rotation matrices
	 */
	glm::mat4 m_rotations;

	/**
	 * scale rendering factor
	 */
	float m_scaleRendering;

	/**
	 * translation
	 */
	glm::vec3 m_trans;

	/**
	 * scale
	 */
	glm::vec3 m_scale;

	/**
	 * epsilon distance for picking
	 */
	float m_epsilon;

	/**
	 * compute and set length axes / scale factors
	 */
	void setLengthAxes();

	/**
	 * length of axes
	 */
	Geom::Vec3f m_lengthAxes;

	Geom::Vec3f m_projectedSelectedAxis;

	Geom::Vec3f m_projectedOrigin;

	/**
	 * get the matrix transformation with the scale factor for rendering
	 */
	glm::mat4 transfoRenderFrame();

public:
	FrameManipulator();

	/**
	 * set size of frame (for rendering)
	 */
	void setSize(float radius);

	/**
	 * add a (signed) val to the size of frame
	 */
	void addSize(float val);

	/**
	 * get the size of frame
	 */
	float getSize();

	/**
	 * draw the frame
	 */
	void draw();

	/**
	 * try picking the frame
	 */
	 unsigned int pick(const Geom::Vec3f& P, const Geom::Vec3f& V, float epsilon=0.0f);


	 /**
	  * lock an axis
	  */
	 void lock(unsigned int axis);

	 /**
	  * unlock an axis
	  */
	 void unlock(unsigned int axis);

	/**
	 * higlight an axis (change width rendering).
	 * To unhighlight, just highlight NONE or highlight a already highlighted  axis
	 */
	void highlight(unsigned int axis);

	/**
	 * rotate the frame around one of its axis
	 */
	void rotate(unsigned int axis, float angle);

	/**
	 * translate the frame around one of its axis
	 * @param axis
	 * @param x ratio of frame radius
	 */
	void translate(unsigned int axis, float x);

	/**
	 * scale the frame in direction of one axis
	 * @param axis (Xs/Ys/Zs/CENTER)
	 * @param sc scale factor to apply on
	 */
	void scale(unsigned int axis, float sc);




	/**
	 * get the matrix transformation
	 */
	glm::mat4 transfo();

	/**
	 * set the position of frame
	 * @param P the position of origin
	 */
	void setTranslation(const Geom::Vec3f& P);

	Geom::Vec3f getPosition() { return Geom::Vec3f(m_trans[0],m_trans[1],m_trans[2]);}

	Geom::Vec3f getAxis(unsigned int ax);

//	void project(unsigned int ax, Geom::Vec3f& origin, Geom::Vec3f& vect);

	void storeProjection(unsigned int ax);

	/**
	 * set the scale of frame
	 * @param P the vector of scale factors
	 */
	void setScale(const Geom::Vec3f& S);

	/**
	 * set the orientation of frame (Z is deduced)
	 * @param X the vector X of frame
	 * @param Y the vector Y of frame
	 * @return return false if parameters are not unit orthogonal vectors
	 */
	bool setOrientation(const Geom::Vec3f& X, const Geom::Vec3f& Y);

	/**
	 * set transformation matrix
	 */
	void setTransformation( const glm::mat4& transfo);



	float angleFromMouse(int x, int y, int dx, int dy);

	float distanceFromMouse(int dx, int dy);

	float scaleFromMouse(int dx, int dy);

//	float factorCenterFromMouse(int dx, int dy);

	static bool rotationAxis(unsigned int axis) { return (axis>=Xr) && (axis<=Zr);}
	static bool translationAxis(unsigned int axis) { return (axis>=Xt) && (axis<=Zt);}
	static bool scaleAxis(unsigned int axis) { return ((axis>=Xs) && (axis<=Zs))|| (axis==CENTER);}

};


}
}
#endif
