/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
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
* Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include <iostream>

#include "Utils/glutwin_atb.h"

#include "Topology/map/map2.h"

#include "Algo/Export/export.h"
#include "Algo/Import/import.h"

#include "Algo/Geometry/boundingbox.h"
#include "Algo/Geometry/curvature.h"
#include "Algo/Geometry/laplacian.h"

#include "Algo/LinearSolving/basic.h"

#include "Algo/Selection/raySelector.h"

#include "Topology/generic/ecell.h"

#include "Algo/Render/map_glRender.h"
#include "Algo/Render/vbo_MapRender.h"

#include "OpenNL/linear_solver.h"
#include "OpenNL/sparse_matrix.h"
#include "OpenNL/full_vector.h"
#ifdef WITH_CUDA
#include "CNC/cnc_sparse_matrix.h"
#include "CNC/cnc_arrays.h"
#endif



using namespace CGoGN ;


struct PFP {
	// definition de la carte
	typedef Map2 MAP;

	// definition du type de reel utilise
	typedef float REAL;
	typedef Geom::Vector<3,REAL> VEC3;
	typedef Geom::Vector<6,REAL> VEC6;
	typedef Geom::Matrix<3,3,REAL> MATRIX33;
	typedef Geom::Matrix<4,4,REAL> MATRIX44;
	typedef Geom::Matrix<3,6,REAL> MATRIX36;

	typedef AttributeHandler<VEC3> TVEC3;
	typedef AttributeHandler<REAL> TREAL;
	typedef AttributeHandler<MATRIX33> TFRAME;
	typedef AttributeHandler<MATRIX36> TRGBFUNCS;
};

INIT_STATICS_MAP() ;


typedef PFP::MAP MAP;


MAP myMap ;
SelectorTrue allDarts;

unsigned int skipCount = 0 ;

typedef CPULinearSolverTraits< SparseMatrix<double>, FullVector<double> > CPUSolverTraits ;

#ifdef WITH_CUDA
typedef GPULinearSolverTraits< CNCSparseMatrix, CNCArray1d<float> > GPUSolverTraits ;
#endif

class MyGlutWin : public Utils::GlutWin_ATB
{
public:
	TwBar* viewer_bar ;

	enum renderMode { FLAT, GOURAUD, PHONG, VERTICES, NORMAL, CURVATURE_DIRECTIONS } ;

	Geom::Vec4f colDif ;
	Geom::Vec4f colSpec ;
	Geom::Vec4f colClear ;
	Geom::Vec4f colNormal ;

	float shininess ;

	Geom::Vec3f gPosObj ;
	float gWidthObj ;
	float normalBaseSize ;
	float normalScaleFactor ;
	float faceShrinkage ;

	int renderStyle ;
	bool renderObject ;
	bool renderLines ;
	bool renderNormals ;

	enum curvType { MEAN, GAUSSIAN } ;

	bool showCurvature ;
	bool showCurvatureDirections ;
	int curvatureType ;
	int curvatureDir ;
	float curvMultiplicator ;
	float smoothAmount ;

	AttributeHandler<unsigned int> vIndex ;
	PFP::TVEC3 position ;
	PFP::TVEC3 normal ;
	PFP::TVEC3 diffCoord ;
	PFP::TREAL k1 ;
	PFP::TREAL k2 ;
	PFP::TVEC3 K1 ;
	PFP::TVEC3 K2 ;
	AttributeHandler<Geom::Vec4f> color ;

	Algo::Render::VBO::MapRender_VBO<PFP>* vbo_render ;
	GLuint dl_norm ;

	unsigned int nb_vertices ;

	bool def_locked, def_handle ;
	bool selecting ;
	float select_area ;
	bool deselecting ;
	bool dragging ;
	CellMarker lockingMarker, handleMarker ;
	std::vector<unsigned int> locked_vertices ;
	std::vector<unsigned int> handle_vertices ;

	enum solvType { ITERATIVE, DIRECT } ;
	int solverType ;
#ifdef WITH_CUDA
	bool gpuSolver ;
#endif
	LinearSolver<CPUSolverTraits>* solver ;

	MyGlutWin(int* argc, char **argv, int winX, int winY) ;

	void setSolverType(int t) {
		solverType = t ;
		if(solverType == DIRECT)
			solver->set_direct(true) ;
		else
			solver->set_direct(false) ;
	}
	int getSolverType() { return solverType ; }

	void init() ;
	void initGUI() ;

	void myRedraw() ;
	void myKeyboard(unsigned char keycode, int x, int y) ;
	void myMouse(int button, int state, int x, int y) ;
	void myMotion(int x, int y) ;

	void updateVBOdata(int upType, bool recompute = true) ;
	void initDLNormals() ;
	void render(int renderMode) ;

//	void selectVertices(PFP::MAP::Dart d, float dist) ;
	void selectVertex(Dart d) ;
	void deselectVertex(Dart d) ;
	void deselectAllVertices() ;

	void smooth() ;
	void smoothCurvature() ;
	void lsm() ;
	void matchDiffCoord(bool keepMatrix) ;
} ;

template<typename PFP, class SOLVER_TRAITS>
class FunctorMatrixRowDiffCoord : public FunctorMap<typename PFP::MAP>
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	const typename PFP::TVEC3& diffCoordTable ;
	AttributeHandler<unsigned int>& indexTable ;
	unsigned int coord ;

public:
	FunctorMatrixRowDiffCoord(MAP& m, LinearSolver<SOLVER_TRAITS>* s, const typename PFP::TVEC3& dc, unsigned int c, AttributeHandler<unsigned int>& idx) :
		FunctorMap<MAP>(m), solver(s), diffCoordTable(dc), indexTable(idx), coord(c)
	{}
	bool operator()(Dart d)
	{
		solver->begin_row() ;
		Dart dd = d ;
		double aii = 0.0 ;
		do
		{
			double aij = 1.0 ;
			aii += aij ;
			solver->add_coefficient(indexTable[this->m_map.phi1(dd)], aij) ;
			dd = this->m_map.alpha1(dd) ;
		} while(dd != d) ;
		solver->add_coefficient(indexTable[d], -aii) ;
		solver->normalize_row() ;
		solver->set_right_hand_side((diffCoordTable[d])[coord]) ;
		solver->end_row() ;
		return false ;
	}
} ;
