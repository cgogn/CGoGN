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

/*
 * This Linear Solver class extends the one found in Graphite,
 * developped by Bruno Levy, ALICE Project, LORIA, INRIA Lorraine
 */


#ifndef __LINEAR_SOLVER__
#define __LINEAR_SOLVER__

#include <vector>
#include <iostream>
#include <cassert>
#include <cstdlib>

#include <Eigen/Sparse>


template <class CoeffType>
class Variable {
public:
    Variable() : x_(0), index_(-1), locked_(false) { }
    double value() const { return x_; }
    void set_value(double x_in) { x_ = x_in ; }
    void lock()   { locked_ = true ; }
    void unlock() { locked_ = false ; }
    bool is_locked() const { return locked_ ; }
    unsigned int index() const {
        assert(index_ != -1) ;
        return (unsigned int)(index_) ;
    }
    void set_index(unsigned int index_in) {
        index_ = index_in ;
    }
private:
    CoeffType x_ ;
    int index_ ;
    bool locked_ ;
}  ;


/**
 * Solves a linear system or minimizes a quadratic form.
 */
template <typename CoeffType>
class LinearSolver {
protected:
    enum State {
        INITIAL, IN_SYSTEM, IN_ROW, CONSTRUCTED, SOLVED
    } ;

public:
	LinearSolver(unsigned int nb_variables) ;
	LinearSolver() ;
	~LinearSolver() {
		delete[] variable_ ;
		delete A_ ;
		delete x_ ;
		delete b_ ;
//		delete direct_solver_ ;
	}

	// __________________ Parameters ________________________

	void set_direct(bool x) { direct_ = x ; }
	void set_least_squares(bool x) { least_squares_ = x ; }

	// __________________ Access ____________________________

	unsigned int nb_variables() const { return nb_variables_ ; }

	Variable<CoeffType>& variable(unsigned int idx) {
		assert(idx < nb_variables_) ;
		return variable_[idx] ;
	}

	const Variable<CoeffType>& variable(unsigned int idx) const {
		assert(idx < nb_variables_) ;
		return variable_[idx] ;
	}

	// _________________ Construction _______________________

	void begin_system() ;
	inline void begin_row() ;
	inline void set_right_hand_side(CoeffType b) ;
	inline void add_coefficient(unsigned int iv, CoeffType a) ;
	inline void normalize_row(CoeffType weight = 1) ;
	inline void scale_row(CoeffType s) ;
	inline void end_row() ;
	void end_system() ;

	// ----------------------------- Solver -------------------------------

	void solve() ;

	void reset(bool keep_matrix) ;

protected:

	// ----------- Converting between user representation and the internal representation -----

	void vector_to_variables() ;
	void variables_to_vector() ;

	// ----------- Finite state automaton (checks that calling sequence is respected) ---------

	std::string state_to_string(State s) {
		switch(s) {
			case INITIAL:
				return "initial" ;
			case IN_SYSTEM:
				return "in system" ;
			case IN_ROW:
				return "in row" ;
			case CONSTRUCTED:
				return "constructed" ;
			case SOLVED:
				return "solved" ;
		}
		// Should not go there.
		assert(false) ;
		return "undefined" ;
	}

	void check_state(State s) {
		if(state_ != s) {
			std::cerr << "Wrong state, expected: "
					<< state_to_string(s)
					<< " got:"
					<< state_to_string(state_)
					<< std::endl ;
			std::cerr << "exiting ..." << std::endl ;
		}
		assert(state_ == s) ;
	}

	void transition(State from, State to) {
		check_state(from) ;
		state_ = to ;
	}

private:

	// --------------- parameters --------------------------
	bool least_squares_ ;
	bool direct_ ;
	bool matrix_already_set_ ;

	// --------------- user representation --------------
	unsigned int nb_variables_ ;
	Variable<CoeffType>* variable_ ;

	// --------------- construction -----------------------
	State state_ ;
	unsigned int current_row_ ;
	std::vector<CoeffType> af_ ;
	std::vector<unsigned int> if_ ;
	std::vector<CoeffType> al_ ;
	std::vector<CoeffType> xl_ ;
	CoeffType bk_ ;

	// --------------- internal representation ---------
	Eigen::SparseMatrix<CoeffType>* A_ ;
	Eigen::Matrix<CoeffType, Eigen::Dynamic, 1>* x_ ;
	Eigen::Matrix<CoeffType, Eigen::Dynamic, 1>* b_ ;

	Eigen::ConjugateGradient<Eigen::SparseMatrix<CoeffType> >* symmetric_solver_;
	Eigen::BiCGSTAB<Eigen::SparseMatrix<CoeffType> >* nonsymmetric_solver_;
	Eigen::SimplicialLDLT<Eigen::SparseMatrix<CoeffType> >* direct_solver_;
} ;

#include "OpenNL/linear_solver.hpp"

#endif
