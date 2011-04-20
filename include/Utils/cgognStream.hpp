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
#include "Utils/qtSimple.h"
#include <QtGui/QTextEdit>


namespace CGoGN
{

namespace CGoGNStream
{

template<int LEVEL>
Out<LEVEL>::Out():
	m_out_mode(STDOUT),
	m_sqt_bar(NULL),
	m_sqt_console(NULL),
	m_qte(NULL),
	m_ofs(NULL),
	m_oss(NULL),
	m_code(0)
{}


template<int LEVEL>
Out<LEVEL>::~Out()
{
	if (LEVEL<=DBG_MAX_LEVEL)
	{
		if (m_ofs!=NULL)
		{
			m_ofs->close();
			delete m_ofs;
		}
	}
}

template<int LEVEL>
void Out<LEVEL>::out2std(bool yes)
{
	if (LEVEL<=DBG_MAX_LEVEL)
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
}

//template<int LEVEL>
//void Err<LEVEL>::out2std(bool yes)
//{
//	if (LEVEL<=DBG_MAX_LEVEL)
//	{
//		if (yes)
//			this->m_out_mode |= STDERR;
//		else
//			this->m_out_mode &= ~STDERR;
//	}
//}

template<int LEVEL>
void Out<LEVEL>::out2File(const std::string& filename )
{
	if (LEVEL<=DBG_MAX_LEVEL)
	{
		if (!filename.empty())
			m_out_mode |= FILEOUT;
		else
			m_out_mode &= ~FILEOUT;

		if (m_ofs!=NULL)
		{
			m_ofs->close();
			delete m_ofs;
		}
		m_ofs = new std::ofstream(filename.c_str());
	}
}

template<int LEVEL>
void Out<LEVEL>::out2StatuBar(Utils::QT::SimpleQT* sqt)
{
	if (LEVEL<=DBG_MAX_LEVEL)
	{
		if (sqt != NULL)
			m_out_mode |= QTSTATUSBAR;
		else
			m_out_mode &= ~QTSTATUSBAR;
		m_sqt_bar = sqt;
	}
}

template<int LEVEL>
void Out<LEVEL>::out2Console(Utils::QT::SimpleQT* sqt)
{
	if (LEVEL<=DBG_MAX_LEVEL)
	{
		if (sqt)
			m_out_mode |= QTCONSOLE;
		else
			m_out_mode &= ~QTCONSOLE;
		m_sqt_console=sqt;
	}
}

template<int LEVEL>
void Out<LEVEL>::out2Buffer(std::stringstream* ss)
{
	if (LEVEL<=DBG_MAX_LEVEL)
	{
		if (ss != NULL)
			m_out_mode |= SSBUFFER;
		else
			m_out_mode &= ~SSBUFFER;

		m_oss = ss;
	}
}

template<int LEVEL>
Out<LEVEL>&  Out<LEVEL>::operator<< (Out& os)
{
	return *this;
}

template<int LEVEL>
void Out<LEVEL>::close()
{
	if (LEVEL<=DBG_MAX_LEVEL)
	{
		if (m_ofs!=NULL)
		{
			m_ofs->close();
		}
	}

}


template<int LEVEL>
template <typename T>
Out<LEVEL>&  Out<LEVEL>::operator<< (const T& val)
{
	if (m_out_mode & STDOUT)
		std::cout << val;
	if (m_out_mode & STDERR)
		std::cerr << val;

	if (m_out_mode && (FILEOUT|QTSTATUSBAR|QTCONSOLE|SSBUFFER))
		m_buffer << val;

	return *this;
}


template<int LEVEL>
Out<LEVEL>&  Out<LEVEL>::operator<< (Special& os  )
{
	if (LEVEL<=DBG_MAX_LEVEL)
	{
		if (&os == &CGoGNendl)
		{
			char bufc[512];

			// for cout & cerr just do the endl
			if (m_out_mode & STDOUT)
				std::cout << std::endl;
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
					m_sqt_bar->statusMsg(bufc);
				}
			}

			if (m_out_mode & QTCONSOLE)
			{
				while (! m_buffer.eof())
				{
					m_buffer.getline(bufc,512);

					if (m_code>=100)
						m_sqt_console->console()->setTextColor( QColor(0, 150 - (m_code-100)*20, 50+(m_code-100)*20) );
					else
					{
						if (m_code>0)
							m_sqt_console->console()->setTextColor( QColor(150, 0, 0) );
						else
							m_sqt_console->console()->setTextColor( QColor(0, 0, 150) );
					}

					m_sqt_console->console()->insertPlainText(QString(bufc));
					m_sqt_console->console()->insertPlainText(QString("\n"));
				}
			}

			if (m_out_mode & SSBUFFER)
			{
				while (! m_buffer.eof())
				{
					m_buffer.getline(bufc,512);
					*m_oss  << bufc << std::endl;
				}
			}
		}
	}

	m_buffer.clear();
	return *this;
}


}
}

