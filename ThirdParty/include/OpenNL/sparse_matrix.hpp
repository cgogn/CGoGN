#include <algorithm>

template <class CoeffType>
void SparseRowColumn<CoeffType>::add(unsigned int index, CoeffType val) {
	Coeff<CoeffType>* coeff = NULL ;
	// Search for a_{index}
	for(unsigned int ii=0; ii < nb_coeffs_; ii++) {
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
		coeff->a = val ;
		coeff->index = index ;
	}
}

template <class CoeffType>
void SparseRowColumn<CoeffType>::grow() {
	size_t old_capacity = capacity_ ;
	capacity_ = capacity_ * 2 ;
	Allocator<Coeff<CoeffType> >::reallocate(coeff_, old_capacity, capacity_) ;
}

template <class CoeffType>
class CoeffIndexCompare {
public:
	bool operator()(const Coeff<CoeffType>& c1, const Coeff<CoeffType>& c2) {
		return c1.index < c2.index ;
	}
} ;

template <class CoeffType>
void SparseRowColumn<CoeffType>::sort() {
	Coeff<CoeffType>* begin = coeff_ ;
	Coeff<CoeffType>* end   = coeff_ + nb_coeffs_ ;
	std::sort(begin, end, CoeffIndexCompare<CoeffType>()) ;
}

//_________________________________________________________

template <class CoeffType>
SparseMatrix<CoeffType>::SparseMatrix(size_t m, size_t n, Storage storage) {
	storage_ = NONE ;
	allocate(m,n,storage,false) ;
}

template <class CoeffType>
SparseMatrix<CoeffType>::SparseMatrix(size_t n, Storage storage, bool symmetric_storage) {
	storage_ = NONE ;
	allocate(n, n, storage, symmetric_storage) ;
}

template <class CoeffType>
SparseMatrix<CoeffType>::~SparseMatrix() {
	deallocate() ;
}

template <class CoeffType>
SparseMatrix<CoeffType>::SparseMatrix() {
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

template <class CoeffType>
size_t SparseMatrix<CoeffType>::nnz() const {
	size_t result = 0 ;
	if(rows_are_stored()) {
		for(unsigned int i=0; i<m(); i++) {
			result += row(i).nb_coeffs() ;
		}
	} else if(columns_are_stored()) {
		for(unsigned int j=0; j<n(); j++) {
			result += column(j).nb_coeffs() ;
		}
	} else {
		assert(false) ;
	}
	return result ;
}

template <class CoeffType>
void SparseMatrix<CoeffType>::deallocate() {
	m_ = 0 ;
	n_ = 0 ;
	diag_size_ = 0 ;

	delete[] row_ ;
	delete[] column_ ;
	delete[] diag_ ;
	row_ = NULL ;
	column_ = NULL ;
	diag_ = NULL ;

	storage_ = NONE ;
	rows_are_stored_ = false ;
	columns_are_stored_ = false ;
	symmetric_storage_ = false ;
}

template <class CoeffType>
void SparseMatrix<CoeffType>::allocate(size_t m, size_t n, Storage storage, bool symmetric_storage) {
	if(storage_ != NONE) {
		deallocate() ;
	}

	m_ = m ;
	n_ = n ;
	diag_size_ = m < n ? m : n ;
	symmetric_storage_ = symmetric_storage ;
	symmetric_tag_ = false ;
	storage_ = storage ;
	switch(storage) {
		case NONE:
			assert(false) ;
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
	diag_ = new CoeffType[diag_size_] ;
	for(unsigned int i=0; i<diag_size_; i++) {
		diag_[i] = 0 ;
	}

	if(rows_are_stored_) {
		row_ = new Row[m] ;
	} else {
		row_ = NULL ;
	}

	if(columns_are_stored_) {
		column_ = new Column[n] ;
	} else {
		column_ = NULL ;
	}
}

template <class CoeffType>
void SparseMatrix<CoeffType>::sort() {
	if(rows_are_stored_) {
		for(unsigned int i=0; i<m_; i++) {
			row(i).sort() ;
		}
	}
	if(columns_are_stored_) {
		for(unsigned int j=0; j<n_; j++) {
			column(j).sort() ;
		}
	}
}

template <class CoeffType>
void SparseMatrix<CoeffType>::zero() {
	if(rows_are_stored_) {
		for(unsigned int i=0; i<m_; i++) {
			row(i).zero() ;
		}
	}
	if(columns_are_stored_) {
		for(unsigned int j=0; j<n_; j++) {
			column(j).zero() ;
		}
	}
	for(unsigned int i=0; i<diag_size_; i++) {
		diag_[i] = 0 ;
	}
}

template <class CoeffType>
void SparseMatrix<CoeffType>::clear() {
	if(rows_are_stored_) {
		for(unsigned int i=0; i<m_; i++) {
			row(i).clear() ;
		}
	}
	if(columns_are_stored_) {
		for(unsigned int j=0; j<n_; j++) {
			column(j).clear() ;
		}
	}
	for(unsigned int i=0; i<diag_size_; i++) {
		diag_[i] = 0 ;
	}
}

//----------------------------------------------------------------------
//     Helpers for sparse matrix * vector product
//     according to storage modes:
//           rows or columns
//           symmetric
//----------------------------------------------------------------------

template <class CoeffType>
static void mult_rows_symmetric(const SparseMatrix<CoeffType>& A, const CoeffType* x, CoeffType* y) {
	size_t m = A.m() ;
	for(unsigned int i=0; i<m; i++) {
		y[i] = 0 ;
		const typename SparseMatrix<CoeffType>::Row& Ri = A.row(i) ;
		for(unsigned int ij=0; ij<Ri.nb_coeffs(); ij++) {
			const Coeff<CoeffType>& c = Ri.coeff(ij) ;
			y[i] += c.a * x[c.index] ;
			if(i != c.index) {
				y[c.index] += c.a * x[i] ;
			}
		}
	}
}

template <class CoeffType>
static void mult_rows(const SparseMatrix<CoeffType>& A, const CoeffType* x, CoeffType* y) {
	size_t m = A.m() ;
	for(unsigned int i=0; i<m; i++) {
		y[i] = 0 ;
		const typename SparseMatrix<CoeffType>::Row& Ri = A.row(i) ;
		for(unsigned int ij=0; ij<Ri.nb_coeffs(); ij++) {
			const Coeff<CoeffType>& c = Ri.coeff(ij) ;
			y[i] += c.a * x[c.index] ;
		}
	}
}

template <class CoeffType>
static void mult_cols_symmetric(const SparseMatrix<CoeffType>& A, const CoeffType* x, CoeffType* y) {
	memset(y, 0, A.m() * sizeof(CoeffType)) ;
	size_t n = A.n() ;
	for(unsigned int j=0; j<n; j++) {
		const typename SparseMatrix<CoeffType>::Column& Cj = A.column(j) ;
		for(unsigned int ii=0; ii<Cj.nb_coeffs(); ii++) {
			const Coeff<CoeffType>& c = Cj.coeff(ii) ;
			y[c.index] += c.a * x[j]       ;
			if(j != c.index) {
				y[j] += c.a * x[c.index] ;
			}
		}
	}
}

template <class CoeffType>
static void mult_cols(const SparseMatrix<CoeffType>& A, const CoeffType* x, CoeffType* y) {
	memset(y, 0, A.m() * sizeof(CoeffType)) ;
	size_t n = A.n() ;
	for(unsigned int j=0; j<n; j++) {
		const typename SparseMatrix<CoeffType>::Column& Cj = A.column(j) ;
		for(unsigned int ii=0; ii<Cj.nb_coeffs(); ii++) {
			const Coeff<CoeffType>& c = Cj.coeff(ii) ;
			y[c.index] += c.a * x[j] ;
		}
	}
}

// general driver routine for
// sparse matrix * vector product
template <class CoeffType>
void mult(const SparseMatrix<CoeffType>& A, const CoeffType* x, CoeffType* y) {
	if(A.rows_are_stored()) {
		if(A.has_symmetric_storage()) {
			mult_rows_symmetric(A, x, y) ;
		} else {
			mult_rows(A, x, y) ;
		}
	} else {
		if(A.has_symmetric_storage()) {
			mult_cols_symmetric(A, x, y) ;
		} else {
			mult_cols(A, x, y) ;
		}
	}
}

//----------------------------------------------------------------------
//     Helpers for transpose sparse matrix * vector product
//     according to storage modes:
//           rows or columns
//           symmetric
//     Note: in the symmetric case, A = A^t (and we can reuse the
//      non-transpose product)
//----------------------------------------------------------------------

template <class CoeffType>
static void mult_xpose_rows(const SparseMatrix<CoeffType>& A, const CoeffType* x, CoeffType* y) {
	memset(y, 0, A.n() * sizeof(CoeffType)) ;
	size_t m = A.m() ;
	for(unsigned int i=0; i<m; i++) {
		const typename SparseMatrix<CoeffType>::Row& Ri = A.row(i) ;
		for(unsigned int ij=0; ij<Ri.nb_coeffs(); ij++) {
			const Coeff<CoeffType>& c = Ri.coeff(ij) ;
			y[c.index] += c.a * x[i] ;
		}
	}
}

template <class CoeffType>
static void mult_xpose_cols(const SparseMatrix<CoeffType>& A, const CoeffType* x, CoeffType* y) {
	size_t n = A.n() ;
	for(unsigned int j=0; j<n; j++) {
		y[j] = 0.0 ;
		const typename SparseMatrix<CoeffType>::Column& Cj = A.column(j) ;
		for(unsigned int ii=0; ii<Cj.nb_coeffs(); ii++) {
			const Coeff<CoeffType>& c = Cj.coeff(ii) ;
			y[j] += c.a * x[c.index] ;
		}
	}
}

// general driver routine for
// transpose sparse matrix * vector product
template <class CoeffType>
void mult_transpose(const SparseMatrix<CoeffType>& A, const CoeffType* x, CoeffType* y) {
	if(A.rows_are_stored()) {
		if(A.has_symmetric_storage()) {
			mult_rows_symmetric(A, x, y) ;
		} else {
			mult_xpose_rows(A, x, y) ;
		}
	} else {
		if(A.has_symmetric_storage()) {
			mult_cols_symmetric(A, x, y) ;
		} else {
			mult_xpose_cols(A, x, y) ;
		}
	}
}
