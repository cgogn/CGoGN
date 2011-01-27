/*
 *  CNC: Concurrent Number Cruncher
 *  Copyright (C) 2008 GOCAD/ASGA, INRIA/ALICE
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
 *  If you modify this software, you should include a notice giving the
 *  name of the person performing the modification, the date of modification,
 *  and the reason for such modification.
 *
 *  Contact: Luc Buatois
 *
 *     buatois@gocad.org
 *
 *     ASGA-INPL Bt. G
 *     Rue du Doyen Marcel Roubault - BP 40
 *     54501 VANDOEUVRE LES NANCY
 *     FRANCE
 *
 *  Note that the GNU General Public License does not permit incorporating
 *  the Software into proprietary programs. 
 */

#ifndef CNC_BASIC_ARRAYS_H
#define CNC_BASIC_ARRAYS_H


//---------------------------------------------------------------------------//


template <class T> class CNCArray1d {
public:
    typedef CNCArray1d<T> thisclass ;
	typedef T CoeffType ;

    CNCArray1d(unsigned int size = 0, unsigned int alignment = 1) {
        data_ = NULL ;
        base_mem_ = NULL ;
        size_ = 0 ;
        allocate(size, alignment) ;
    } 

    inline ~CNCArray1d() { deallocate() ; }

//---------------------------------------------------------------------------//

    /** does not preserve previous values stored in the array */
    void allocate(unsigned int size, unsigned int alignment = 1) {
        deallocate() ;
        if(size != 0) {
            base_mem_ = (char*)malloc(size * sizeof(T) + alignment -1) ;
	    char* p = base_mem_ ;
	    // GMY 20090825 original: while(unsigned __int64(p) % alignment) {  p++ ; }
	    while (((unsigned long long) p) % alignment) { ++p; }
            data_ = (T*)p ;
            for(unsigned int i=0; i<size; i++) {
                // Direct call to the constructor, see dlist.h for more explanations.
                new(&data_[i])T() ;                    
            }
        }
        size_ = size ;
    }

//---------------------------------------------------------------------------//

    void set_all(const T& value) {
        for(unsigned int i=0; i<size_; i++) {
            data_[i] = value ;
        }
    }

//---------------------------------------------------------------------------//

    T& operator()(unsigned int i) {
        return data_[i] ;
    }

//---------------------------------------------------------------------------//

    const T& operator()(unsigned int i) const {
        return data_[i] ;
    }

//---------------------------------------------------------------------------//

    T& operator[](unsigned int index) {
        return data_[index] ;
    }

//---------------------------------------------------------------------------//

    const T& operator[](unsigned int index) const {
        return data_[index] ;
    }

//---------------------------------------------------------------------------//

    T& from_linear_index(unsigned int index) {
        return data_[index] ;
    }

//---------------------------------------------------------------------------//

    const T& from_linear_index(unsigned int index) const {
        return data_[index] ;
    }

//---------------------------------------------------------------------------//

    unsigned int size() const { return size_ ; }
    unsigned int dimension() const { return size_ ; }

//---------------------------------------------------------------------------//
	
	unsigned int alignment() const { return alignment_ ; }

//---------------------------------------------------------------------------//

    void clear() { allocate(0) ; }

//---------------------------------------------------------------------------//

    /** low-level access, for experts only. */
    const T* data() const { return data_ ; }

//---------------------------------------------------------------------------//

    /** low-level access, for experts only. */
    T* data() { return data_ ; }

//---------------------------------------------------------------------------//

    unsigned int mem_usage() const {
        return size_ * sizeof(T) + sizeof(thisclass) ;
    }

//---------------------------------------------------------------------------//

	void print () const {
		for ( unsigned int index = 0; index<size_; index++ ) {
			printf ( "array[%d]=%lg\n", index, data_[index] ) ;
		}
	}

//---------------------------------------------------------------------------//

protected:
    T* data_ ;
    unsigned int size_ ;
    char* base_mem_ ;
    unsigned int alignment_ ;

//---------------------------------------------------------------------------//

    void deallocate() {
        if(size_ != 0) {
            for(unsigned int i=0; i<size_; i++) {
                // direct call to the destructor
                data_[i].~T() ;
            }
            free(base_mem_) ;
            data_ = NULL ;
            base_mem_ = NULL ;
            size_ = 0 ;
        }
    }

//---------------------------------------------------------------------------//

private:
    CNCArray1d(const thisclass& rhs) ;
    thisclass& operator=(const thisclass& rhs) ;
} ;

typedef CNCArray1d<float>  floatArray  ;
typedef CNCArray1d<double> doubleArray ;

//---------------------------------------------------------------------------//


#endif

