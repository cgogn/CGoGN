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

#include "cnc_sparse_matrix.h"
#include <algorithm>


// -------------------------------------------------------------------------- //
// Dynamic Sparse matrix data structures									  //
// -------------------------------------------------------------------------- //

    void CNCSparseRowColumn::add(long index, float val) {
        CNCCoeff* coeff = NULL ;
        // Search for a_{index}
        for(long ii=0; ii < nb_coeffs_; ii++) {
            if(coeff_[ii].index == index) {
                coeff = &(coeff_[ii]) ;
                break ;
            }
        }
        if(coeff != NULL) {
            coeff->a += val ;
        } else {
            nb_coeffs_++ ;
            if(nb_coeffs_ > capacity_) {
                grow() ;
            }
            coeff = &(coeff_[nb_coeffs_ - 1]) ;
            coeff->a     = val ;
            coeff->index = index ;
        }
    }
    
//---------------------------------------------------------------------------//

    void CNCSparseRowColumn::grow() {
        long old_capacity = capacity_ ;
        capacity_ = capacity_ * 2 ;
        CNCreallocate<CNCCoeff>(
            coeff_, old_capacity, capacity_
        ) ;
    }

//---------------------------------------------------------------------------//

    class CNCCoeffIndexCompare {
    public:
        bool operator()(const CNCCoeff& c1, const CNCCoeff& c2) {
            return c1.index < c2.index ;
        }
    } ;

//---------------------------------------------------------------------------//

    void CNCSparseRowColumn::sort() {
        CNCCoeff* begin = coeff_ ;
        CNCCoeff* end   = coeff_ + nb_coeffs_ ;
        std::sort(begin, end, CNCCoeffIndexCompare()) ;
    }

//---------------------------------------------------------------------------//

    CNCSparseMatrix::CNCSparseMatrix(long m, long n, Storage storage) {
        storage_ = NONE ;
        allocate(m,n,storage,false) ;
    }

//---------------------------------------------------------------------------//

    CNCSparseMatrix::CNCSparseMatrix(
        long n, Storage storage, bool symmetric_storage
    ) {
        storage_ = NONE ;
        allocate(n,n,storage,symmetric_storage) ;
    }

//---------------------------------------------------------------------------//

   CNCSparseMatrix::CNCSparseMatrix( long n ) {
        m_ = 0 ;
        n_ = 0 ;
        diag_size_ = 0 ;

        row_ = NULL ;
        column_ = NULL ;
        diag_ = NULL ;

		storage_ = ROWS ;
        allocate(n,n,storage_,false) ;
    }

//---------------------------------------------------------------------------//

    CNCSparseMatrix::~CNCSparseMatrix() {
        deallocate() ;
    }

//---------------------------------------------------------------------------//

    CNCSparseMatrix::CNCSparseMatrix() {
        m_ = 0 ;
        n_ = 0 ;
        diag_size_ = 0 ;

        row_ = NULL ;
        column_ = NULL ;
        diag_ = NULL ;

        storage_ = NONE ;
        rows_are_stored_ = false ;
        columns_are_stored_ = false ;
        symmetric_storage_ = false ;
        symmetric_tag_ = false ;
    }

//---------------------------------------------------------------------------//

    long CNCSparseMatrix::nnz() const {
        long result = 0 ;
        if(rows_are_stored()) {
            for(long i=0; i<m(); i++) {
                result += row(i).nb_coeffs() ;
            }
        } else if(columns_are_stored()) {
            for(long j=0; j<n(); j++) {
                result += column(j).nb_coeffs() ;
            }
        } else {
        }
        return result ;
    }

//---------------------------------------------------------------------------//
        
    void CNCSparseMatrix::deallocate() {
        m_ = 0 ;
        n_ = 0 ;
        diag_size_ = 0 ;

		if ( row_ != NULL ) delete[] row_ ;
        if ( column_ != NULL ) delete[] column_ ;
        if ( diag_ != NULL ) delete[] diag_ ;
        row_ = NULL ;
        column_ = NULL ;
        diag_ = NULL ;

        storage_ = NONE ;
        rows_are_stored_    = false ;
        columns_are_stored_ = false ;
        symmetric_storage_  = false ;
    }

//---------------------------------------------------------------------------//

    void CNCSparseMatrix::allocate(
        long m, long n, Storage storage, bool symmetric_storage
    ) {
        m_ = m ;
        n_ = n ;
		diag_size_ = (m<n)?(m):(n) ;
        symmetric_storage_ = symmetric_storage ;
        symmetric_tag_ = false ;
        storage_ = storage ;
        switch(storage) {
        case NONE:
            break ;
        case ROWS:
            rows_are_stored_    = true ;
            columns_are_stored_ = false ;
            break ;
        case COLUMNS:
            rows_are_stored_    = false ;
            columns_are_stored_ = true ;
            break ;
        case ROWS_AND_COLUMNS:
            rows_are_stored_    = true ;
            columns_are_stored_ = true ;
            break ;
        }
        diag_ = new float[diag_size_] ;
        for(long i=0; i<diag_size_; i++) {
            diag_[i] = 0.0 ;
        }

        if(rows_are_stored_) {
            row_ = new CNCSparseRowColumn[m] ;
        } else {
            row_ = NULL ;
        }

        if(columns_are_stored_) {
            column_ = new CNCSparseRowColumn[n] ;
        } else {
            column_ = NULL ;
        }
    }

//---------------------------------------------------------------------------//

    void CNCSparseMatrix::sort() {
        if(rows_are_stored_) {
            for(long i=0; i<m_; i++) {
                row(i).sort() ;
            }
        }
        if(columns_are_stored_) {
            for(long j=0; j<n_; j++) {
                column(j).sort() ;
            }
        }
    }

//---------------------------------------------------------------------------//

    void CNCSparseMatrix::zero() {
        if(rows_are_stored_) {
            for(long i=0; i<m_; i++) {
                row(i).zero() ;
            }
        }
        if(columns_are_stored_) {
            for(long j=0; j<n_; j++) {
                column(j).zero() ;
            }
        }
        for(long i=0; i<diag_size_; i++) {
            diag_[i] = 0.0 ;
        }
	}

//---------------------------------------------------------------------------//

    void CNCSparseMatrix::clear() {
        if(rows_are_stored_) {
            for(long i=0; i<m_; i++) {
                row(i).clear() ;
            }
        }
        if(columns_are_stored_) {
            for(long j=0; j<n_; j++) {
                column(j).clear() ;
            }
        }
        for(long i=0; i<diag_size_; i++) {
            diag_[i] = 0.0 ;
        }
    }


//---------------------------------------------------------------------------//

