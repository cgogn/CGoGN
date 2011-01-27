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

#ifndef CNC_UTILS_H
#define CNC_UTILS_H

#ifdef OS_LINUX
#include <stdlib.h>
#include "cnc_tchar.h"
#endif

#ifdef OS_WIN
#include <tchar.h>
#endif

//---------------------------------------------------------------------------//


#define CNC_ASSERT(isOK,message) \
  if ( !(isOK) ) { \
    (void)printf("ERROR!! Assert '%s' failed\n%s\n", \
				 #isOK, message); \
	return false ;\
  }

#define CNC_SHORT_ASSERT(isOK,message) \
  if ( !(isOK) ) { \
    (void)printf("%s\n", message); \
	return false ;\
  }

#define CNC_SIMPLE_ASSERT(isOK,message) \
  if ( !(isOK) ) { \
    (void)printf("ERROR!! Assert '%s' failed\n%s\n", \
				 #isOK, message); \
  }


//---------------------------------------------------------------------------//

	template <class T, class S> inline S * CNCallocate_and_copy ( const T * in, int size ) ;
	template <class T> T* CNCallocate ( long number ) ;
	template <class T> void CNCdeallocate ( T*& addr ) ;
	template <class T> void CNCreallocate (  T*& addr, long old_number, long new_number ) ;

//---------------------------------------------------------------------------//

	template<class T, class S> inline S * CNCallocate_and_copy ( const T * in, int size ) {
		
		S * out = new S[size] ;
		for ( int i=0; i<size; i++ ) {
			out[i] = (S)in[i] ;
		}
		return out ;
	}

//---------------------------------------------------------------------------//

    template <class T> T* CNCallocate ( long number ) {
            return new T[number] ;
    }
        
//---------------------------------------------------------------------------//

	template <class T> void CNCdeallocate ( T*& addr ) {
            delete[] addr ;
            addr = NULL ; // makes it possible to track
                          // access to deallocated memory.
	}
        
//---------------------------------------------------------------------------//

	template <class T> void CNCreallocate (
            T*& addr, long old_number, long new_number
        ) {
            T* new_addr = new T[new_number] ;
            for(int i=0; i<old_number; i++) {
                new_addr[i] = addr[i] ;
            }
            delete[] addr ;
            addr = new_addr ;
	}

//---------------------------------------------------------------------------//


#endif
