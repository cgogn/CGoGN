#ifndef __SPARSE_MATRIX__
#define __SPARSE_MATRIX__

#include "OpenNL/full_vector.h"
#include <vector>
#include <cstdlib>
#include <cassert>

/**
* An allocator to create dynamically
* resizable structures. Note: if needed,
* a more efficient method may be used.
*/
template <class T>
class Allocator {
public:
	static inline T* allocate(size_t number) {
		return new T[number] ;
	}

	static inline void deallocate(T*& addr) {
		delete[] addr ;
		addr = NULL ; // makes it possible to track
		// access to deallocated memory.
	}

	static inline void reallocate(T*& addr, size_t old_number, size_t new_number) {
		T* new_addr = new T[new_number] ;
		for(unsigned int i = 0; i < old_number; i++) {
			new_addr[i] = addr[i] ;
		}
		delete[] addr ;
		addr = new_addr ;
	}
} ;

//_________________________________________________________

/**
* A coefficient of a SparseMatrix.
*/
template <class T>
class Coeff {
public:
	T a ;
	unsigned int index ;
} ;

//__________________________________________________

/**
* A row or a column of a SparseMatrix. SparseRowColumn is
* compressed, and stored in the form of a list of
* (value,index) couples.
*/
template <class CoeffType>
class SparseRowColumn {
public:
	SparseRowColumn() {
		coeff_ = Allocator<Coeff<CoeffType> >::allocate(2) ;
		nb_coeffs_ = 0 ;
		capacity_ = 2 ;
	}

	~SparseRowColumn() {
		Allocator<Coeff<CoeffType> >::deallocate(coeff_) ;
	}

	size_t nb_coeffs() const { return nb_coeffs_ ; }

	Coeff<CoeffType>& coeff(unsigned int ii) {
		assert(ii < nb_coeffs_) ;
		return coeff_[ii] ;
	}

	const Coeff<CoeffType>& coeff(unsigned int ii) const {
		assert(ii < nb_coeffs_) ;
		return coeff_[ii] ;
	}

	/** a_{index} <- a_{index} + val */
	void add(unsigned int index, CoeffType val) ;

	/** sorts the coefficients by increasing index */
	void sort() ;

	/**
	* removes all the coefficients and frees the allocated
	* space.
	*/
	void clear() {
		Allocator<Coeff<CoeffType> >::deallocate(coeff_) ;
		coeff_ = Allocator<Coeff<CoeffType> >::allocate(2) ;
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
	Coeff<CoeffType>* coeff_ ;
	size_t nb_coeffs_ ;
	size_t capacity_ ;
} ;

//________________________________________________________________

template <class CoeffType>
class SparseMatrix {
public:
	typedef SparseRowColumn<CoeffType> Row ;
	typedef SparseRowColumn<CoeffType> Column ;

	enum Storage { NONE, ROWS, COLUMNS, ROWS_AND_COLUMNS} ;

	//__________ constructors / destructor _____

	/**
	* Constructs a m*n sparse matrix.
	* @param Storage can be one of ROWS, COLUMNS, ROWS_AND_COLUMNS
	*/
	SparseMatrix(size_t m, size_t n, Storage storage = ROWS) ;

	/**
	* Constructs a square n*n sparse matrix.
	* @param Storage can be one of ROWS, COLUMNS, ROWS_AND_COLUMNS
	* @param symmetric_storage if set, only entries a_ij such
	*   that j <= i are stored.
	*/
	SparseMatrix(size_t n, Storage storage = ROWS, bool symmetric_storage = true) ;

	SparseMatrix() ;

	~SparseMatrix() ;

	//___________ access ________________________

	size_t m() const { return m_ ; }

	size_t n() const { return n_ ; }

	size_t diag_size() const { return diag_size_ ; }

	/** number of non-zero coefficients */
	size_t nnz() const ;

	bool rows_are_stored() const { return rows_are_stored_ ; }

	bool columns_are_stored() const { return columns_are_stored_ ; }

	Storage storage() const { return storage_ ; }

	bool has_symmetric_storage() const { return symmetric_storage_ ; }

	bool is_square() const { return (m_ == n_) ; }

	bool is_symmetric() const { return (symmetric_storage_ || symmetric_tag_) ; }

	/**
	* For symmetric matrices that are not stored in symmetric mode,
	* one may want to give a hint that the matrix is symmetric.
	*/
	void set_symmetric_tag(bool x) { symmetric_tag_ = x ; }

	/**
	* storage should be one of ROWS, ROWS_AND_COLUMNS
	* @param i index of the row, in the range [0, m-1]
	*/
	Row& row(unsigned int i) {
		assert(i < m_) ;
		assert(rows_are_stored_) ;
		return row_[i] ;
	}

	/**
	* storage should be one of ROWS, ROWS_AND_COLUMNS
	* @param i index of the row, in the range [0, m-1]
	*/
	const Row& row(unsigned int i) const {
		assert(i < m_) ;
		assert(rows_are_stored_) ;
		return row_[i] ;
	}

	/**
	* storage should be one of COLUMN, ROWS_AND_COLUMNS
	* @param i index of the column, in the range [0, n-1]
	*/
	Column& column(unsigned int j) {
		assert(j < n_) ;
		assert(columns_are_stored_) ;
		return column_[j] ;
	}

	/**
	* storage should be one of COLUMNS, ROWS_AND_COLUMNS
	* @param i index of the column, in the range [0, n-1]
	*/
	const Column& column(unsigned int j) const {
		assert(j < n_) ;
		assert(columns_are_stored_) ;
		return column_[j] ;
	}

	/**
	* returns aii.
	*/
	double diag(unsigned int i) const {
		assert(i < diag_size_) ;
		return diag_[i] ;
	}

	/**
	* aij <- aij + val
	*/
	void add(unsigned int i, unsigned int j, CoeffType val) {
		assert(i < m_) ;
		assert(j < n_) ;
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

	void allocate(size_t m, size_t n, Storage storage, bool symmetric = false) ;

	void deallocate() ;

private:
	size_t m_ ;
	size_t n_ ;
	size_t diag_size_ ;

	Row* row_ ;
	Column* column_ ;
	CoeffType* diag_ ;

	Storage storage_ ;
	bool rows_are_stored_ ;
	bool columns_are_stored_ ;
	bool symmetric_storage_ ;
	bool symmetric_tag_ ;

	// SparseMatrix cannot be copied.
	SparseMatrix(const SparseMatrix& rhs) ;
	SparseMatrix& operator=(const SparseMatrix& rhs) ;
} ;


/**
* Matrix * vector product: y = A.x
*/
template <class CoeffType>
void mult(const SparseMatrix<CoeffType>& A, const CoeffType* x, CoeffType* y) ;

template <class CoeffType>
inline void mult(const SparseMatrix<CoeffType>& A, const FullVector<CoeffType>& x, FullVector<CoeffType>& y) {
	assert(x.dimension() == A.n()) ;
	assert(y.dimension() == A.m()) ;
	mult(A, x.data(), y.data()) ;
}

/**
* Matrix * vector product: y = At.x
*/
template <class CoeffType>
void mult_transpose(const SparseMatrix<CoeffType>& A, const CoeffType* x, CoeffType* y) ;

template <class CoeffType>
inline void mult_transpose(const SparseMatrix<CoeffType>& A, const FullVector<CoeffType>& x, FullVector<CoeffType>& y) {
	assert(x.dimension() == A.m()) ;
	assert(y.dimension() == A.n()) ;
	mult_transpose(A, x.data(), y.data()) ;
}

#include "OpenNL/sparse_matrix.hpp"

#endif

