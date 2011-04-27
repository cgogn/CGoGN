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

#include "Utils/cgognStream.h"

namespace CGoGN
{

//CGoGNStream::Out CGoGNout;
//CGoGNStream::Err CGoGNerr;
//CGoGNStream::Out CGoGNdbg;
//
//CGoGNStream::Special CGoGNendl;


CGoGNStream::Out<0> CGoGNout;
CGoGNStream::Err<0> CGoGNerr;
CGoGNStream::Dbg<1> CGoGNdbg;
CGoGNStream::Dbg<2> CGoGNdbg2;
CGoGNStream::Dbg<3> CGoGNdbg3;
CGoGNStream::Special CGoGNendl;

namespace CGoGNStream
{

void allToStd(bool yes)
{
	CGoGNout.toStd(yes);
	CGoGNerr.toStd(yes);
	CGoGNdbg.toStd(yes);
	CGoGNdbg2.toStd(yes);
	CGoGNdbg3.toStd(yes);
}

void allToFile(const std::string& filename )
{
	CGoGNout.toFile(filename);
	CGoGNerr.toFile(filename);
	CGoGNdbg.toFile(filename);
	CGoGNdbg2.toFile(filename);
	CGoGNdbg3.toFile(filename);
}

void allToStatusBar(Utils::QT::SimpleQT* sqt)
{
	CGoGNout.toStatusBar(sqt);
	CGoGNerr.toStatusBar(sqt);
	CGoGNdbg.toStatusBar(sqt);
	CGoGNdbg2.toStatusBar(sqt);
	CGoGNdbg3.toStatusBar(sqt);
}

void allToConsole(Utils::QT::SimpleQT* sqt)
{
	CGoGNout.toConsole(sqt);
	CGoGNerr.toConsole(sqt);
	CGoGNdbg.toConsole(sqt);
	CGoGNdbg2.toConsole(sqt);
	CGoGNdbg3.toConsole(sqt);
}

void allToBuffer(std::stringstream* ss)
{
	CGoGNout.toBuffer(ss);
	CGoGNerr.toBuffer(ss);
	CGoGNdbg.toBuffer(ss);
	CGoGNdbg2.toBuffer(ss);
	CGoGNdbg3.toBuffer(ss);
}

} // namespace CGoGNStream

} // namespace CGoGN
