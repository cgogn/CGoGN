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

#ifndef _CGOGNSTREAM_H_
#define _CGOGNSTREAM_H_


#ifndef DBG_MAX_LEVEL
#define DBG_MAX_LEVEL 5
#endif



#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

//forward defs
namespace CGoGN { namespace Utils { namespace QT {class SimpleQT; } } }
class QTextEdit;




namespace CGoGN
{
namespace  CGoGNStream
{

enum drawingType {	STDOUT = 1,
					STDERR = 2,
					FILEOUT = 4,
					QTSTATUSBAR = 8,
					QTCONSOLE = 16,
					SSBUFFER = 32};

class Special
{};

template<int LEVEL>
class Out
{
protected:
	int m_out_mode;

	std::stringstream m_buffer;

	Utils::QT::SimpleQT* m_sqt_bar;

	Utils::QT::SimpleQT* m_sqt_console;

	QTextEdit* m_qte;

	std::ofstream* m_ofs;

	std::stringstream* m_oss;

	int m_code;

public:
	/**
	 * constructor
	 */
	Out();

	/**
	 * destructor
	 */
	~Out();

	/**
	 * set output to standard
	 */
	void out2std(bool yes = true);

	/**
	 * set output to file
	 */
	void out2File(const std::string& filename );

	/**
	 * set output to status bar of Qt interface
	 */
	void out2StatuBar(Utils::QT::SimpleQT* sqt);

	/**
	 * set output to console of Qt interface
	 */
	void out2Console(Utils::QT::SimpleQT* sqt);

	/**
	 * set output to string stream buffer
	 */
	void out2Buffer(std::stringstream* ss);

	/**
	 * recursive stream operator
	 */
	Out<LEVEL>& operator<< (Out& cgstr);

	/**
	 * classic stream operator
	 */
	template <typename T>
	Out<LEVEL>& operator<< (const T& val);

	/**
	 * special cases (endl) stream operator
	 */
	Out<LEVEL>&  operator<< (Special& os  );

	/**
	 * for file closing
	 */
	void close();
};

/**
 * output stream class for error output (replace cout by cerr)
 */
template<int LEVEL>
class Err: public Out<LEVEL>
{
public:
	Err() {this->m_code=1;}
};

template<int LEVEL>
class Dbg: public Out<LEVEL>
{
public:
	Dbg() {this->m_code=100+LEVEL;}
};


}

// glocal stream definitions
extern CGoGNStream::Out<0> CGoGNout;
extern CGoGNStream::Err<0> CGoGNerr;
extern CGoGNStream::Dbg<1> CGoGNdbg;
extern CGoGNStream::Dbg<2> CGoGNdbg2;
extern CGoGNStream::Dbg<3> CGoGNdbg3;
extern CGoGNStream::Special CGoGNendl;

}

#include "Utils/cgognStream.hpp"

#endif /* CGOGNSTREAM_H_ */
