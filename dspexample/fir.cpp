/*
 * fir.cpp
 *
 *  Created on: 08/09/2011
 *      Author: Gabriel Hernádez Montero.
 *      		Mariano Jiménez Brenes.
 */

#include "fir.h"

#undef _DSP_DEBUG
#define _DSP_DEBUG

#ifdef _DSP_DEBUG
#define _debug(x) std::cerr << x
#include <iostream>
#else
#define _debug(x)
#endif

fir::fir()
{

}

fir::~fir()
{

}

void fir::filterFir(int blockSize, float* in, float* out)
{
	_debug("Filtrando con el FIR.\n");
}

void fir::initFir()
{
	_debug("Inicializando el FIR.\n");
}
