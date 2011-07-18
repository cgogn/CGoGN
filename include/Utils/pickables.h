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


#ifndef __PICKABLES_H_
#define __PICKABLE_H_

#include "Utils/vbo.h"
#include "glm/glm.hpp"
#include "Utils/Shaders/shaderColorPerVertex.h"
#include "Utils/Shaders/shaderSimpleColor.h"

namespace CGoGN
{

namespace Utils
{


class LineDrawable
{
protected:
	/**
	 * id of indices VBO
	 */
	GLuint m_ind;

	/**
	 * VBO for position
	 */
	Utils::VBO* m_vboPos;

	/**
	 * Shader
	 */
	Utils::ShaderSimpleColor* m_shader;

	/**
	 * number of indices in vbo
	 */
	unsigned int m_nb;


public:
	/**
	 * constructor
	 */
	LineDrawable();

	/**
	 * destructor
	 */
	virtual ~LineDrawable();

	/**
	 * set the color of drawing
	 */
	void setColor(const Geom::Vec4f& col);

	/**
	 * draw the Drawable at origin with size=1
	 */
	virtual void draw();

	virtual std::string shape() { return "unknown shape";}

	/**
	 * picking
	 * @param P camera point
	 * @param V vector ray direction
	 * @param epsilon distance epsilon for picking
	 * @return code picking (0: nothing picked / != 0 something picked)
	 */
	virtual unsigned int pick(const Geom::Vec3f& P, const Geom::Vec3f& V, float epsilon=0.0f) = 0;

	/**
	 * update the precision of drawing
	 * @param sub number of subdivisions
	 * @param sub2 number of subdivisions (if two needed), default value is same as first
	 */
	virtual void updatePrecisionDrawing(unsigned int sub, unsigned int sub2=0)=0;

};


class Pickable
{
protected:
	/// type of drawable
	LineDrawable* m_drawable;

	/// transformation matrix
	glm::mat4 m_transfo;

	/// id of pickable
	unsigned int m_id;

	/**
	 * comparison operator for depth ordered picking
	 */
	static bool distOrder(const std::pair<float, Pickable*>& e1, const std::pair<float, Pickable*>& e2);

public:
	/**
	 * constructor
	 * @param ld LineDrawable to use for drawing & picking
	 */
	Pickable(LineDrawable* ld, unsigned int id);

	/**
	 * picking
	 * @param P camera point
	 * @param V vector ray direction
	 * @param epsilon distance epsilon for picking
//	 * @return picked ?
	 */
	bool pick(const Geom::Vec3f& P, const Geom::Vec3f& V, float epsilon=0.0f);

	/**
	 * apply inverse transfo on picking ray
	 */
	static void invertPV(const Geom::Vec3f& P, const Geom::Vec3f& V, const glm::mat4& transfo, Geom::Vec3f& PP, Geom::Vec3f& VV);

	/**
	 * draw
	 */
	void draw();

	/**
	 * return a ref on the transformation matrix
	 */
	glm::mat4&  transfo();

	/**
	 * get the id
	 */
	unsigned int id() { return m_id;}

	/**
	 * return the shape of pickable
	 */
	std::string shape() { return m_drawable->shape();}

	/**
	 * rotate
	 */
	void rotate(float angle, const Geom::Vec3f& Axis);

	/**
	 * translate
	 */
	void translate(const Geom::Vec3f& P);

	/**
	 * scale
	 */
	void scale(const Geom::Vec3f& S);

	/**
	 * set a random orientation
	 */
	void randomOrientation();

	/**
	 * set a random scale
	 * @param min minimum value
	 * @param max maximum value
	 */
	void randomScale(float min, float max);

	/**
	 * set an uniform random scale
	 * @param min minimum value
	 * @param max maximum value
	 */
	void randomUniformScale(float min, float max);

	/**
	 * distance from center
	 * @param P point from which compute distance
	 */
	float distancefrom(const Geom::Vec3f& P);


	/**
	 * pick a vector of pickable and return the closest
	 */
	static Pickable* pick(const std::vector<Pickable*>& picks,const Geom::Vec3f& P, const Geom::Vec3f& V);

	/**
	 * pick a vector of pickable and return the picked object ordered from closed to farthest
	 */
	static std::vector<Pickable*> sortedPick(std::vector<Pickable*>& picks, const Geom::Vec3f& P, const Geom::Vec3f& V);

};


/**
 * Grid (-1,-1,0 ; 1,1,0)
 */
class Grid: public LineDrawable
{
public:
	/**
	 * constructor
	 * @param sub number of subdivision of grig
	 */
	Grid(unsigned int sub=5);

	/**
	 * change topo subdivision
	 */
	void changeTopo(unsigned int sub);

	/**
	 * picking
	 */
	unsigned int pick(const Geom::Vec3f& P, const Geom::Vec3f& V, float epsilon=0.0f);

	/**
	 * return a string with shape of object
	 */
	std::string shape() { return "grid";}

	/**
	 * update the precision of drawing
	 * @param sub number of subdivisions
	 * @param sub2 number of subdivisions (if two needed)
	 */
	void updatePrecisionDrawing(unsigned int sub, unsigned int sub2);

};


/**
 * Sphere of radius 1 drawon with lines
 */
class Sphere: public LineDrawable
{
public:
	/**
	 * constructor
	 * @param par number of parallels
	 * @param mer number of meridians
	 */
	Sphere(unsigned int par=5, unsigned int mer=5);

	/**
	 * change topo subdivision
	 */
	void changeTopo(unsigned int par, unsigned int mer);

	/**
	 * specific drawing function for sphere (with indexed vbos)
	 */
	void draw();

	/**
	 * picking
	 */
	unsigned int pick(const Geom::Vec3f& P, const Geom::Vec3f& V, float epsilon=0.0f);

	/**
	 * return a string with shape of object
	 */
	std::string shape() { return "sphere";}

	/**
	 * update the precision of drawing
	 * @param sub number of subdivisions
	 * @param sub2 number of subdivisions (if two needed)
	 */
	void updatePrecisionDrawing(unsigned int sub, unsigned int sub2);
};


/**
 * Cone of radius 1 drawon with lines
 */
class Cone: public Sphere
{
public:
	/**
	 * constructor
	 * @param par number of parallels
	 * @param mer number of meridians
	 */
	Cone(unsigned int par=5, unsigned int mer=5);

	/**
	 * change topo subdivision
	 */
	void changeTopo(unsigned int par, unsigned int mer);

	/**
	 * picking
	 */
	unsigned int pick(const Geom::Vec3f& P, const Geom::Vec3f& V, float epsilon=0.0f);

	/**
	 * return a string with shape of object
	 */
	std::string shape() { return "cone";}

	/**
	 * update the precision of drawing
	 * @param sub number of subdivisions
	 * @param sub2 number of subdivisions (if two needed)
	 */
	void updatePrecisionDrawing(unsigned int sub, unsigned int sub2);

};

/**
 * Cone of radius 1 drawon with lines
 */
class Cylinder: public Sphere
{
public:
	/**
	 * constructor
	 * @param par number of parallels
	 * @param mer number of meridians
	 */
	Cylinder(unsigned int par=5, unsigned int mer=5);

	/**
	 * change topo subdivision
	 */
	void changeTopo(unsigned int par, unsigned int mer);

	/**
	 * picking
	 */
	unsigned int pick(const Geom::Vec3f& P, const Geom::Vec3f& V, float epsilon=0.0f);

	/**
	 * return a string with shape of object
	 */
	std::string shape() { return "cylinder";}

	/**
	 * update the precision of drawing
	 * @param sub number of subdivisions
	 * @param sub2 number of subdivisions (if two needed)
	 */
	void updatePrecisionDrawing(unsigned int sub, unsigned int sub2);

};


/**
 * Sphere of radius 1 drawon with lines
 */
class Cube: public LineDrawable
{
public:
	/**
	 * constructor
	 * @param par number of parallels
	 * @param mer number of meridians
	 */
	Cube(unsigned int sub=1);


	/**
	 * change topo subdivision
	 */
	void changeTopo(unsigned int sub);

	/**
	 * specific drawing function for sphere (with indexed vbos)
	 */
	void draw();

	/**
	 * picking
	 */
	unsigned int pick(const Geom::Vec3f& P, const Geom::Vec3f& V, float epsilon=0.0f);

	/**
	 * return a string with shape of object
	 */
	std::string shape() { return "cube";}

	/**
	 * update the precision of drawing
	 * @param sub number of subdivisions
	 * @param sub2 number of subdivisions (if two needed)
	 */
	void updatePrecisionDrawing(unsigned int sub, unsigned int sub2);

};

class IcoSphere: public Sphere
{
protected:

	unsigned int m_sub;

	unsigned int insertPoint(std::vector<Geom::Vec3f>& points, const Geom::Vec3f& P);

	void subdivide(std::vector<unsigned int>& triangles, std::vector<Geom::Vec3f>& points);

public:
	/**
	 * constructor
	 * @param par number of parallels
	 * @param mer number of meridians
	 */
	IcoSphere(unsigned int sub=1);


	/**
	 * change topo subdivision
	 * @param sub approximative subdivision (coherent with other primitives)
	 */
	void changeTopo(unsigned int sub);

	/**
	 * change topo subdivision
	 * @param sub number of passes of subdivisions
	 */
	void changeTopoSubdivision(unsigned int sub);

	/**
	 * return a string with shape of object
	 */
	std::string shape() { return "icosphere";}

	/**
	 * update the precision of drawing
	 * @param sub number of subdivisions
	 * @param sub2 number of subdivisions (if two needed)
	 */
	void updatePrecisionDrawing(unsigned int sub, unsigned int sub2);

};


}
}
#endif
