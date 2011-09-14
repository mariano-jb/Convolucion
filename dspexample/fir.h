/*
 * fir.h
 *
 *  Created on: 08/09/2011
 *      Author: Gabriel Hernández Montero.
 *      		Mariano Jiménez Brenes.
 */

#ifndef FIR_H_
#define FIR_H_


class fir
{
	public:
	  /**
	   * Constructor
	   */
	  fir();

	  /**
	   * Destructor
	   */
	  ~fir();

	  //Métodos de la clase

	  void initFir();

	  void filterFir(int blockSize, float* in, float* out);

	protected:
};

#endif /* FIR_H_ */

