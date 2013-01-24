/*
 *  OpenNL<T>: Generic Numerical Library
 *  Copyright (C) 2004 Bruno Levy
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  In addition, as a special exception, the INRIA gives permission to link the 
 *  code of this program with the CGAL library, and distribute linked combinations 
 *  including the two. You must obey the GNU General Public License in all respects 
 *  for all of the code used other than CGAL. 
 *
 *  If you modify this software, you should include a notice giving the
 *  name of the person performing the modification, the date of modification,
 *  and the reason for such modification.
 *
 *  Contact: Bruno Levy
 *
 *     levy@loria.fr
 *
 *     ISA-ALICE Project
 *     LORIA, INRIA Lorraine, 
 *     Campus Scientifique, BP 239
 *     54506 VANDOEUVRE LES NANCY CEDEX 
 *     FRANCE
 *
 *  Note that the GNU General Public License does not permit incorporating
 *  the Software into proprietary programs. 
 */


#ifndef __FULL_VECTOR__
#define __FULL_VECTOR__

#include <cstdlib>
#include <cassert>


template <class T>
class FullVector {
public:
    typedef T CoeffType ;
    FullVector(unsigned int dim) { 
        dimension_ = dim ; coeff_ = new T[dim] ; 
    }
    ~FullVector() { 
        delete[] coeff_ ;
        coeff_ = NULL ; 
    }
    unsigned int dimension() const {
        return dimension_ ; 
    }
    const T& operator[](unsigned int i) const {
        assert(i < dimension_) ;
        return coeff_[i] ;
    }
    T& operator[](unsigned int i) {
        assert(i < dimension_) ;
        return coeff_[i] ;
    }
    const T* data() const { return coeff_ ; }
    T* data() { return coeff_ ; }

private:
    unsigned int dimension_ ;
    T* coeff_ ;
} ;


#endif
