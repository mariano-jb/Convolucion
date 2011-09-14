/*
 * fileManager.cpp
 *
 *  Created on: 12/09/2011
 *      Author: mariano
 */

#include"fileManager.h"

#undef _DSP_DEBUG
#define _DSP_DEBUG

#ifdef _DSP_DEBUG
#define _debug(x) std::cerr << x
#include <iostream>
#else
#define _debug(x)
#endif

fileManager::fileManager()
{

}

fileManager::~fileManager()
{
	fclose(archivo_);
	_debug("Cerrando el archivo de valores procesados por el filtro\n");
}

void fileManager::initFile(char* nombre)
{
	archivo_ = fopen(nombre,"a+");
	_debug("Archivo de valores procesados por el filtro creado\n");
}

void fileManager::writeln(int blockSize, float* in)
{
	for(int n=0;n<blockSize;++n)
	fprintf(archivo_,"%2.15f\n",in[n]);
}

void fileManager::writeFile(int blockSize, float* in, float* out)
{
	_debug("Escribiendo en el archivo de valores procesados por el filtro\n");
	for (int n=0;n<blockSize;++n)
	{
		fprintf(archivo_,"%d		%f		%f\n",n, in[n], out[n]);
	}
}

void fileManager::closeFile()
{

}
