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

#ifndef CNC_BASICS_LINE_STREAM_H
#define CNC_BASICS_LINE_STREAM_H

#include <iostream>
#include <sstream>
#include <fstream>

#include "cnc_sparse_matrix.h"


//---------------------------------------------------------------------------//
// LineInputStream facilitates parsing ASCII files line by line.			 //
//---------------------------------------------------------------------------//

class LineInputStream {
public:
    LineInputStream(std::istream& in) : in_(in), line_in_(NULL) {   }
    ~LineInputStream() { delete line_in_ ; line_in_ = NULL ;  }
    bool eof() const { return in_.eof() ; }
    bool eol() const { return line_in_ == NULL || line_in_->eof() ; }
    bool ok() const { return in_ != NULL ; }
    void get_line() {
		in_.getline(buffer_, 65536) ;
		delete line_in_ ;
		line_in_ = new std::istringstream(buffer_) ;
	}
    std::istream& line() {  return *line_in_ ;    }
    const char *current_line() const { return buffer_;  }
    template <class T> LineInputStream& operator>>(T& param) { 
		*line_in_ >> param; return *this;
	}
private:
    std::istream& in_ ;
    std::istringstream* line_in_ ;
    char buffer_[65536] ;
} ;


//---------------------------------------------------------------------------//
// ExampleLoader helps in loading sparse matrix files saved in ASCII format	 //
//---------------------------------------------------------------------------//

class ExampleLoader {
public:

	static bool load_example ( std::string file, CNCSparseMatrix & sm, floatArray & b ) {

		std::ifstream input ( file.c_str() ) ;
		if ( !input.is_open() ) {
			printf ( "Wrong file or path: %s\n", file.c_str() ) ;
			return false ;
		}
		LineInputStream in(input) ;

		in.get_line() ;
		unsigned int vec_size ;
		in >> vec_size ;

		in.get_line() ;
		unsigned int mat_size ;
		in >> mat_size ;

		sm.allocate ( vec_size, vec_size, CNCSparseMatrix::ROWS, false ) ;
		b.allocate ( vec_size ) ;

		printf ( "\n+------------------------+\n" ) ;
		printf ( "|..Loading example file..|\n|0 ------------------ 100|\n|" ) ;

		unsigned int i, j ;
		float value ;

		unsigned int line_number = 0 ;
		unsigned int last_point_index = 0 ;
		unsigned int temp = 0 ;

		while(!in.eof()) {
            in.get_line() ;
            std::string keyword ;

			in >> keyword ;

            if ( keyword == "b" ) { // right hand side
				in >> i ;
				in >> value ;
				b[i] = value ;
			}
			else if ( keyword == "a" ) { // matrix coefficient
				in >> i ;
				in >> j ;
				in >> value ;
				sm.add ( i, j, value ) ;
			}
			line_number++ ;

			temp = (int)(24.0f*(float)line_number/(float)mat_size) ;
			if ( temp > last_point_index ) {
				printf ( "#" ) ;
				last_point_index = temp ;
			}
		}
		printf ( "|\n|   .. File loaded ..    |\n" ) ;
		printf ( "+------------------------+\n\n" ) ;
		return true ;
	}


private:
	ExampleLoader () ;
} ;

//---------------------------------------------------------------------------//


#endif
