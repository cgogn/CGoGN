#include "slot_debug.h"


namespace CGoGN
{

namespace SCHNApps
{

unsigned int PrettyPrintShiftMessage::s_nb = 0;

PrettyPrintShiftMessage::PrettyPrintShiftMessage(const char *fname):
	m_msg(fname)
{
	for (unsigned int i = 0; i < s_nb; ++i)
	{
		std::cout << "    ";
	}
	std::cout << "Entering slot "<< m_msg << std::endl;

	s_nb++;
}


PrettyPrintShiftMessage::~PrettyPrintShiftMessage()
{
	for (unsigned int i = 0; i < s_nb; ++i)
	{
		std::cout << "    ";
	}
	std::cout << "Exiting slot "<< m_msg  << std::endl;
	s_nb--;
}

void PrettyPrintShiftMessage::shift()
{
	for (unsigned int i = 0; i < s_nb; ++i)
	{
		std::cout << "    ";
	}
}

} // namespace SCHNApps

} // namespace CGoGN
