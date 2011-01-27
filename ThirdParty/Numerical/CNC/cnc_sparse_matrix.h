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

#ifndef CNC_SPARSE_MATRIX_H
#define CNC_SPARSE_MATRIX_H

#include "cnc_sparse_matrix_crs.h"
#include "cnc_sparse_matrix_bcrs.h"
#include "cnc_arrays.h"

#include <set>
#include <vector>


enum { CNC_alignment_for_SSE2 = 16} ;

// -------------------------------------------------------------------------- //
// Dynamic Sparse matrix data structures									  //
// -------------------------------------------------------------------------- //

    /**
     * A coefficient of a SparseMatrix. (currently FLOAT data ONLY)
     */
    class CNCCoeff {
    public:
        float a ;
        long index ;
    } ;

//---------------------------------------------------------------------------//

    /**
     * A row or a column of a SparseMatrix. SparseRowColumn is
     * compressed, and stored in the form of a list of
     * (value,index) couples.
     */
    class CNCSparseRowColumn {
    public:
        CNCSparseRowColumn() {
            coeff_ = CNCallocate<CNCCoeff>(2) ;
            nb_coeffs_ = 0 ;
            capacity_ = 2 ;
        }

        ~CNCSparseRowColumn() {
            CNCdeallocate<CNCCoeff>(coeff_) ;
        }

        long nb_coeffs() const { return nb_coeffs_ ; }

        CNCCoeff& coeff(long ii) { 
            return coeff_[ii] ;
        }
            
        const CNCCoeff& coeff(long ii) const {
            return coeff_[ii] ;
        }        
            
        /** a_{index} <- a_{index} + val */
        void add(long index, float val) ;
            
        /** sorts the coefficients by increasing index */
        void sort() ;


        /** 
         * removes all the coefficients and frees the allocated
         * space.
         */
        void clear() { 
			CNCdeallocate<CNCCoeff>(coeff_) ;
            coeff_ = CNCallocate<CNCCoeff>(2) ;
            nb_coeffs_ = 0 ;
            capacity_ = 2 ;
        }

        /** 
         * removes all the coefficients, but keeps the
         * allocated space, that will be used by subsequent
         * calls to add().
         */
        void zero() { nb_coeffs_ = 0 ; }

    protected:
        void grow() ;
            
    private:
        CNCCoeff* coeff_ ;
        long nb_coeffs_ ;
        long capacity_ ;
    } ;

//---------------------------------------------------------------------------//

    class CNCSparseMatrix {
    public:

        enum Storage { NONE, ROWS, COLUMNS, ROWS_AND_COLUMNS} ;

        // constructors / destructor

        /**
         * Constructs a m*n sparse matrix.
         * @param Storage can be one of ROWS, COLUMNS, ROWS_AND_COLUMNS
         */
        CNCSparseMatrix(long m, long n, Storage storage = ROWS) ;

		/**
         * Constructs a n*n sparse matrix, row storage is used,
		 * Non symmetric storage is used
         */
        CNCSparseMatrix(long n ) ;

		/**
         * Constructs a square n*n sparse matrix.
         * @param Storage can be one of ROWS, COLUMNS, ROWS_AND_COLUMNS
         * @param symmetric_storage if set, only entries a_ij such
         *   that j <= i are stored.
         */
        CNCSparseMatrix(long n, Storage storage, bool symmetric_storage) ;

        CNCSparseMatrix() ;

        ~CNCSparseMatrix() ;

        // access

        long m() const {
            return m_ ;
        }

        long n() const {
            return n_ ;
        }

        long diag_size() const {
            return diag_size_ ;
        }

        /** number of non-zero coefficients */
        long nnz() const ;

        bool rows_are_stored() const {
            return rows_are_stored_ ;
        }

        bool columns_are_stored() const {
            return columns_are_stored_ ;
        }

        Storage storage() const {
            return storage_ ;
        }

        bool has_symmetric_storage() const {
            return symmetric_storage_ ;
        }

        bool is_square() const {
            return (m_ == n_) ;
        }

        bool is_symmetric() const {
            return (symmetric_storage_ || symmetric_tag_) ;
        }

        /**
         * For symmetric matrices that are not stored in symmetric mode,
         * one may want to give a hint that the matrix is symmetric.
         */
        void set_symmetric_tag(bool x) {
            symmetric_tag_ = x ;
        }

        /**
         * storage should be one of ROWS, ROWS_AND_COLUMNS
         * @param i index of the row, in the range [0, m-1]
         */
        CNCSparseRowColumn& row(long i) {
            return row_[i] ;
        }

        /**
         * storage should be one of ROWS, ROWS_AND_COLUMNS
         * @param i index of the row, in the range [0, m-1]
         */
        const CNCSparseRowColumn& row(long i) const {
            return row_[i] ;
        }

        /**
         * storage should be one of COLUMN, ROWS_AND_COLUMNS
         * @param i index of the column, in the range [0, n-1]
         */
        CNCSparseRowColumn& column(long j) {
            return column_[j] ;
        }

        /**
         * storage should be one of COLUMNS, ROWS_AND_COLUMNS
         * @param i index of the column, in the range [0, n-1]
         */
        const CNCSparseRowColumn& column(long j) const {
            return column_[j] ;
        }
        
        /**
         * returns aii.
         */
        float diag(long i) const {
            return diag_[i] ;
        }

        /**
         * aij <- aij + val
         */
        void add(long i, long j, float val) {
            if(symmetric_storage_ && j > i) {
                return ;
            }
            if(i == j) {
                diag_[i] += val ;
            } 
            if(rows_are_stored_) {
                row(i).add(j, val) ;
            }
            if(columns_are_stored_) {
                column(j).add(i, val) ;
            }
        }
            

        /** sorts rows and columns by increasing coefficients */
        void sort() ;
            

        /**
         * removes all the coefficients and frees the allocated
         * space.
         */
        void clear() ;

        /**
         * removes all the coefficients, but keeps the allocated
         * storage, that will be used by subsequent calls to add().
         */
        void zero() ;

        void allocate(
            long m, long n, Storage storage, bool symmetric = false
        ) ;

        void deallocate() ;

    private:
        long m_ ;
        long n_ ;
        long diag_size_ ;

        CNCSparseRowColumn* row_ ;
        CNCSparseRowColumn* column_ ;
        float* diag_ ;

        Storage storage_ ;
        bool rows_are_stored_ ;
        bool columns_are_stored_ ;
        bool symmetric_storage_ ;
        bool symmetric_tag_ ;

        // SparseMatrix cannot be copied.
        CNCSparseMatrix(const CNCSparseMatrix& rhs) ;
        CNCSparseMatrix& operator=(const CNCSparseMatrix& rhs) ;
    } ;


//---------------------------------------------------------------------------//

	template <class T> inline void convert_matrix(
        const CNCSparseMatrix& rhs, CNCSparseMatrixCRS<T>& A, bool separate_diag = true ) ;

    template <class T, int BM, int BN> inline void convert_matrix(
        const CNCSparseMatrix& rhs, CNCSparseMatrixBCRS<T, BM, BN>& A ) ;


//---------------------------------------------------------------------------//

    template <class T> inline void convert_matrix(
		const CNCSparseMatrix& rhs, CNCSparseMatrixCRS<T>& A, bool separate_diag ) {

        A.separate_diag = separate_diag ;
        A.symmetric_storage = rhs.has_symmetric_storage() ;
        A.N = rhs.n() ;
        A.rowptr.allocate(rhs.m() + 1) ;
        unsigned int nnz = rhs.nnz() ;
        if(separate_diag) {
            nnz -= rhs.diag_size() ;
        }
        A.colind.allocate(nnz) ;
        A.a.allocate(nnz, CNC_alignment_for_SSE2) ;
        A.diag.allocate(rhs.diag_size(), CNC_alignment_for_SSE2) ;
        
        unsigned int cur = 0 ;
        for(int i=0; i<rhs.m(); i++) {
            A.rowptr[i] = cur ;
            const CNCSparseRowColumn & R = rhs.row(i) ;
			for(int jj=0; jj<R.nb_coeffs(); jj++) {
				if(!separate_diag || (R.coeff(jj).index != i )) {
                    A.a[cur] = T(R.coeff(jj).a) ;
                    A.colind[cur] = R.coeff(jj).index ;
                    cur++ ;
                }
            }
        }
        A.rowptr[rhs.m()] = nnz ;
        for(int i=0; i<rhs.diag_size(); i++) {
            A.diag[i] = T(rhs.diag(i)) ;
        }
    }


//---------------------------------------------------------------------------//

    template <class T, int BM, int BN> inline void convert_matrix(
		const CNCSparseMatrix& rhs, CNCSparseMatrixBCRS<T, BM, BN>& A ) {

        unsigned int BLOC_SIZE = CNCSparseMatrixBCRS<T, BM, BN>::BLOC_SIZE ;

        // Compute number of bloc rows and bloc columns
        unsigned int M = rhs.m() / BM ;
        if((rhs.m() % BM) != 0) { M++ ; }

        unsigned int N = rhs.n() / BN ;
        if((rhs.n() % BN) != 0) { N++ ; }

        A.N_ = N ;

        // Step 1: determine blocs to use
		CNCArray1d< std::set<unsigned int> > row_blocs(M) ;
        for(int i=0; i<rhs.m(); i++) {
            unsigned int I = i / BM ;
            const CNCSparseRowColumn & Ri = rhs.row(i) ;
			for(int jj=0 ; jj < Ri.nb_coeffs(); jj++) {
                unsigned int j = Ri.coeff(jj).index ;
                unsigned int J = j / BN ;
                row_blocs[I].insert(J) ;
            }
        }

        // Step 2: initialize rowptr 
        A.rowptr.allocate(M+1) ;
        A.rowptr[0] = 0 ;
        for(unsigned int I=0; I<M; I++) {
            A.rowptr[I+1] = (long)(A.rowptr[I] + row_blocs[I].size()) ;
        }
        unsigned int NNZ = A.rowptr[M] ;

        // Step 3: initialize colind
        A.colind.allocate(NNZ) ;
        unsigned int cur = 0 ;
        for(unsigned int I=0; I<M; I++) {
            for(std::set<unsigned int>::iterator it = row_blocs[I].begin(); it != row_blocs[I].end(); it++) {
                A.colind[cur++] = (*it) ;
            }
        }           
            
        // Step 4: initialize a
        A.a.allocate(NNZ * BLOC_SIZE, CNC_alignment_for_SSE2) ;
        A.a.set_all(0.0) ;
        for(int i=0; i<rhs.m(); i++) {
            unsigned int I = i / BM ;
            unsigned int di = i % BM ;
            const CNCSparseRowColumn & Ri = rhs.row(i) ; 
			for(int jj=0; jj < Ri.nb_coeffs(); jj++) {
                unsigned int j = Ri.coeff(jj).index ;
                unsigned int J = j / BN ;
                unsigned int dj = j % BN ;
                bool found = false ;
                for(int K=A.rowptr[I]; K<A.rowptr[I+1]; K++) {
                    if(A.colind[K] == J) {
                        A.a[K * BLOC_SIZE + di * BN + dj] = Ri.coeff(jj).a ;
                        found = true ;
                        break ;
                    }
                }
            }
        }
        // Step 5: initialize diag
        A.diag.allocate(rhs.diag_size()) ;
        for(unsigned int i=0; i<A.diag.size(); i++) {
            A.diag[i] = rhs.diag(i) ;
        }
    }


//---------------------------------------------------------------------------//


#endif

