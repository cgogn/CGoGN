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

//namespace CGoGNStream
//{
//
//
//
//
//Out::Out():
//	m_out_mode(STDOUT),
//	m_sqt(NULL),
//	m_qte(NULL),
//	m_ofs(NULL),
//	m_oss(NULL)
//{}
//
//Out::~Out()
//{
//	if (m_ofs!=NULL)
//	{
//		m_ofs->close();
//		delete m_ofs;
//	}
//
//}
//
//void Out::out2std(bool yes)
//{
//	if (yes)
//		m_out_mode |= STDOUT;
//	else
//		m_out_mode &= ~STDOUT;
//}
//
//void Err::out2std(bool yes)
//{
//	if (yes)
//		m_out_mode |= STDERR;
//	else
//		m_out_mode &= ~STDERR;
//}
//
//void Out::out2File(const std::string& filename )
//{
//	if (!filename.empty())
//		m_out_mode |= FILEOUT;
//	else
//		m_out_mode &= ~FILEOUT;
//
//	if (m_ofs!=NULL)
//	{
//		m_ofs->close();
//		delete m_ofs;
//	}
//	m_ofs = new std::ofstream(filename.c_str());
//}
//
//void Out::out2StatuBar(Utils::QT::SimpleQT* sqt)
//{
//	if (sqt != NULL)
//		m_out_mode |= QTSTATUSBAR;
//	else
//		m_out_mode &= ~QTSTATUSBAR;
//	m_sqt = sqt;
//}
//
//void Out::out2TextEdit(QTextEdit* qte)
//{
//	if (qte)
//		m_out_mode |= QTTEXTEDIT;
//	else
//		m_out_mode &= ~QTTEXTEDIT;
//	m_qte=qte;
//}
//
//void Out::out2Buffer(std::stringstream* ss)
//{
//	if (ss != NULL)
//		m_out_mode |= SSBUFFER;
//	else
//		m_out_mode &= ~SSBUFFER;
//
//	m_oss = ss;
//}
//
//Out&  Out::operator<< (Out& os)
//{
//	return *this;
//}
//
//void Out::close()
//{
//	if (m_ofs!=NULL)
//	{
//		m_ofs->close();
//	}
//
//}
//
//
//Out&  Out::operator<< (Special& os  )
//{
//	if (&os == &CGoGNendl)
//	{
//		m_buffer << std::endl;
//		char bufc[512];
//		m_buffer.getline(bufc,512);
//
//		if (m_out_mode & STDOUT)
//			std::cout << std::endl;
//
//		if (m_out_mode & STDERR)
//			std::cerr << std::endl;
//
////		if (m_out_mode & STDOUT)
////			std::cout << bufc << std::endl;
////
////		if (m_out_mode & STDERR)
////			std::cerr << bufc << std::endl;
//
//		if (m_out_mode & FILEOUT)
//			*m_ofs << bufc << std::endl;
//
//		if (m_out_mode & QTSTATUSBAR)
//			m_sqt->statusMsg(bufc);
//
//		if (m_out_mode & QTTEXTEDIT)
//		{
//			m_qte->insertPlainText(QString(bufc));
//			m_qte->insertPlainText(QString("\n"));
//		}
//
//		if (m_out_mode & SSBUFFER)
//			*m_oss  << bufc << std::endl;
//	}
//
//	return *this;
//}
//
//}

}


