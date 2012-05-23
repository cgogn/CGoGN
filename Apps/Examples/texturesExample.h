/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/
#ifndef _TEXTURE_EXAMPLE_
#define _TEXTURE_EXAMPLE_

#include <iostream>


#include "Utils/Qt/qtSimple.h"
#include "Utils/textures.h"
#include "Utils/Shaders/shaderSimpleTexture.h"
#include "Utils/Shaders/shaderTextureMask.h"
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Algo/Render/GL2/mapRender.h"


// forward definitions (minimize includes)
namespace CGoGN { namespace Algo { namespace Render { namespace GL2 { class MapRender; }}}}
namespace CGoGN { namespace Utils { class VBO; } }


using namespace CGoGN ;

struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap2 MAP ;
};

typedef PFP::MAP MAP ;
typedef PFP::VEC3 VEC3 ;
/**
 * A class for a little interface and rendering
 */

class TexView: public Utils::QT::SimpleQT
{
	Q_OBJECT
protected:
	void createMask(unsigned int nb);
	void computeImage();
public:

	MAP myMap ;

	// render
	Algo::Render::GL2::MapRender* m_render;

	// VBO
	Utils::VBO* m_positionVBO;
	Utils::VBO* m_texcoordVBO;

	Utils::Texture<2,Geom::Vec3uc>* m_texture;
	Utils::Texture<2,float>* m_mask;

	//2 shaders
	Utils::ShaderSimpleTexture* m_shader;
	Utils::ShaderTextureMask* m_shader2;

	//with mask or not
	bool m_modeMask;

	/// filename of last loaded texture
	std::string m_fileName;

	TexView();

	~TexView();

	// callbacks of simpleQT to overdefine:
	void cb_redraw();

	void cb_initGL();

	void cb_keyPress(int code);

	void cb_Open();
};

#endif
