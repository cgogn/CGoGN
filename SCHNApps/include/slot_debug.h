#ifndef _SLOT_DBG_H_
#define _SLOT_DBG_H_

#include <iostream>

namespace CGoGN
{

namespace SCHNApps
{


// TO USE DEBUGGING INFO ON SLOT
// USE cmake .. -DSLOT_DEBUG_MODE=1


class PrettyPrintShiftMessage
{
protected:
	static unsigned int s_nb;
	std::string m_msg;

public:
	PrettyPrintShiftMessage(const char *fname);

	~PrettyPrintShiftMessage();

	static void shift();
};


} // namespace SCHNApps

} // namespace CGoGN

//#define SLOT_DBG_MODE 1

#ifdef SLOT_DEBUG_MODE
#define DEBUG_SLOT() PrettyPrintShiftMessage ppslots(__PRETTY_FUNCTION__);
#define DEBUG_EMIT(SIGNALS) PrettyPrintShiftMessage::shift(); std::cout << "Emitting signal " << SIGNALS << " from " << __PRETTY_FUNCTION__ << std::endl;
#else
#define DEBUG_SLOT()
#define DEBUG_EMIT(SIGNALS)
#endif


#endif
