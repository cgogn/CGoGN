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
#include "Utils/qtSimple.h"
#include <QtGui/QTextEdit>

namespace CGoGN
{

CGoGNStream::Out CGoGNout;
CGoGNStream::Err CGoGNerr;
CGoGNStream::Dbg CGoGNdbg;
CGoGNStream::Special CGoGNendl;
CGoGNStream::Special CGoGNflush;

namespace CGoGNStream
{

void allToStd(bool yes)
{
	CGoGNout.toStd(yes);
	CGoGNerr.toStd(yes);
	CGoGNdbg.toStd(yes);

}

void allToFile(const std::string& filename )
{
	CGoGNout.toFile(filename);
	CGoGNerr.toFile(filename);
	CGoGNdbg.toFile(filename);

}

void allToStatusBar(Utils::QT::SimpleQT* sqt)
{
	CGoGNout.toStatusBar(sqt);
	CGoGNerr.toStatusBar(sqt);
	CGoGNdbg.toStatusBar(sqt);

}

void allToConsole(Utils::QT::SimpleQT* sqt)
{
	CGoGNout.toConsole(sqt);
	CGoGNerr.toConsole(sqt);
	CGoGNdbg.toConsole(sqt);

}

void allToBuffer(std::stringstream* ss)
{
	CGoGNout.toBuffer(ss);
	CGoGNerr.toBuffer(ss);
	CGoGNdbg.toBuffer(ss);

}





Out::Out():
	m_out_mode(STDOUT),
	m_sqt_bar(NULL),
	m_sqt_console(NULL),
	m_qte(NULL),
	m_ofs(NULL),
	m_oss(NULL),
	m_code(0)
{}


Out::~Out()
{
	if (m_ofs != NULL)
	{
		m_ofs->close();
		delete m_ofs;
	}
}


void Out::toStd(bool yes)
{
	if (m_code)
	{
		if (yes)
			this->m_out_mode |= STDERR;
		else
			this->m_out_mode &= ~STDERR;
	}
	else
	{
		if (yes)
			m_out_mode |= STDOUT;
		else
			m_out_mode &= ~STDOUT;
	}
}




void Out::toFile(const std::string& filename )
{
	if (!filename.empty())
		m_out_mode |= FILEOUT;
	else
		m_out_mode &= ~FILEOUT;

	if (m_ofs != NULL)
	{
		m_ofs->close();
		delete m_ofs;
	}
	m_ofs = new std::ofstream(filename.c_str());
}

void Out::toStatusBar(Utils::QT::SimpleQT* sqt)
{
	if (sqt != NULL)
		m_out_mode |= QTSTATUSBAR;
	else
		m_out_mode &= ~QTSTATUSBAR;
	m_sqt_bar = sqt;
}


void Out::toConsole(Utils::QT::SimpleQT* sqt)
{
	if (sqt)
		m_out_mode |= QTCONSOLE;
	else
		m_out_mode &= ~QTCONSOLE;
	m_sqt_console = sqt;
}


void Out::toBuffer(std::stringstream* ss)
{
	if (ss != NULL)
		m_out_mode |= SSBUFFER;
	else
		m_out_mode &= ~SSBUFFER;

	m_oss = ss;
}

Out&  Out::operator<< (Out& os)
{
	return *this;
}


void Out::close()
{
	if (m_ofs != NULL)
	{
		m_ofs->close();
	}
}



Out&  Out::operator<< (Special& os  )
{
	if (&os == &CGoGNendl)
	{
		char bufc[512];

		// for cout & cerr just do the endl
		if (m_out_mode & STDOUT)
			std::cout << std::endl;;
		if (m_out_mode & STDERR)
			std::cerr << std::endl;

		if (m_out_mode & FILEOUT)
		{
			while (! m_buffer.eof())
			{
				m_buffer.getline(bufc,512);
				*m_ofs << bufc << std::endl;
			}
		}

		if (m_out_mode & QTSTATUSBAR)
		{
			while (! m_buffer.eof())
			{
				m_buffer.getline(bufc,512);
//				m_sqt_bar->statusMsg(bufc);
			}
		}

		if (m_out_mode & QTCONSOLE)
		{
			while (! m_buffer.eof())
			{
				m_buffer.getline(bufc,512);

				if (m_code >= 100)
					m_sqt_console->console()->setTextColor(QColor(0, 150 - (m_code-100) * 20, 50 + (m_code-100) * 20));
				else
				{
					if (m_code > 0)
						m_sqt_console->console()->setTextColor(QColor(150, 0, 0));
					else
						m_sqt_console->console()->setTextColor(QColor(0, 0, 150));
				}

//					m_sqt_console->console()->insertPlainText(QString(bufc));
//					m_sqt_console->console()->insertPlainText(QString("\n"));
				m_sqt_console->console()->append(QString(bufc));
			}
		}

		if (m_out_mode & SSBUFFER)
		{
			while (! m_buffer.eof())
			{
				m_buffer.getline(bufc, 512);
				*m_oss  << bufc << std::endl;
			}
		}
	}

	if (&os == &CGoGNflush)
			{
				char bufc[512];

				// for cout & cerr just do the endl
				if (m_out_mode & STDOUT)
					std::cout << std::flush;
				if (m_out_mode & STDERR)
					std::cerr << std::flush;

				if (m_out_mode & FILEOUT)
				{
					while (! m_buffer.eof())
					{
						m_buffer.getline(bufc,512);
						*m_ofs << bufc << std::flush;
					}
				}

				if (m_out_mode & QTSTATUSBAR)
				{
					while (! m_buffer.eof())
					{
						m_buffer.getline(bufc,512);
//						m_sqt_bar->statusMsg(bufc);
					}
				}

				if (m_out_mode & QTCONSOLE)
				{
					while (! m_buffer.eof())
					{
						m_buffer.getline(bufc,512);

						if (m_code >= 100)
							m_sqt_console->console()->setTextColor(QColor(0, 150 - (m_code-100) * 20, 50 + (m_code-100) * 20));
						else
						{
							if (m_code > 0)
								m_sqt_console->console()->setTextColor(QColor(150, 0, 0));
							else
								m_sqt_console->console()->setTextColor(QColor(0, 0, 150));
						}

						m_sqt_console->console()->moveCursor(QTextCursor::End);
						m_sqt_console->console()->insertPlainText(QString(bufc));
					}
				}

				if (m_out_mode & SSBUFFER)
				{
					while (! m_buffer.eof())
					{
						m_buffer.getline(bufc, 512);
						*m_oss  << bufc << std::flush;
					}
				}
			}
	m_buffer.clear();
	return *this;
}




} // namespace CGoGNStream

} // namespace CGoGN
