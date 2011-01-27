/*
 * sizeblock.h
 *
 *  Created on: Jun 10, 2010
 *      Author: thery
 */

#ifndef SIZEBLOCK_H_
#define SIZEBLOCK_H_

#include "gzstream.h"

#define _BLOCKSIZE_ 4096
typedef unsigned int uint;

//typedef std::ifstream CGoGNistream;
//typedef std::ofstream CGoGNostream;


/// use everywhere in save/load compressed stream
typedef igzstream CGoGNistream;
typedef ogzstream CGoGNostream;

#endif /* SIZEBLOCK_H_ */
