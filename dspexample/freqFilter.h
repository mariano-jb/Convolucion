/*
 * DSP Example is part of the DSP Lecture at TEC-Costa Rica
 * Copyright (C) 2010  Pablo Alvarado
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file   freqFilter.h
 *         Implements filtering in the frequency domain
 * \author Pablo Alvarado
 * \date   2010.12.12
 *
 * $Id: equalizer.cpp $
 */

#ifndef FREQFILTER_H
#define FREQFILTER_H

#include <fftw3.h>

/**
 * Filtering operation in the frequency domain.
 *
 * This small class allows to filter an incoming data stream with a kernel
 * defined in the frequency domain.
 *
 * It is assumed that the frequency response is hermetian, and therefore
 * represents a real valued impulse response filter.
 */
class freqFilter {
public:
  /**
   * Constructor
   *
   * @param blockSize size of the data blocks to be filtered
   */
  freqFilter(int blockSize);

  /**
   * Destructor
   */
  ~freqFilter();

  /**
   * Set the frequency response of the filter
   *
   * The filterSize must be at least the blockSize plus the size of the filter
   * impuse response, i.e. the size of the given filter frequency response
   * should already have considered the zero padded impulse response to
   * be able to hold the result of the convolution without aliasing.
   */
  void setFilter(fftwf_complex* Hw,int HwSize,int hnSize);

  /**
   * Set the filter impulse response
   *
   * The filterSize must be at least the blockSize plus the size of the filter
   * impuse response, i.e. the size of the given filter frequency response
   * should already have considered the zero padded impulse response to
   * be able to hold the result of the convolution without aliasing.
   */
  void setFilter(float* hn,int hnSize,int HwSize);

  /**
   * Filter the input block of the size given at construction time and produce
   * the output of the same size considering past evaluations.
   */
  void filter(float* in,float* out);

  /**
   * Reset
   *
   * Set all internal state data to zero
   */
  void reset();

protected:
  /**
   * Block size
   */
  int blockSize_;

  /**
   * Frequency response size
   */
  int HwSize_;

  /**
   * Impulse respones size
   */
  int hnSize_;

  /**
   * fftw3 library plan for direct transform
   */
  fftwf_plan fft_;

  /**
   * fftw3 library plan for inverse transform
   */
  fftwf_plan ifft_;

  /**
   * Buffer used for frequency domain filter response
   */
  fftwf_complex* Hw_;

  /**
   * Buffer used for frequency domain input
   */
  fftwf_complex* Xw_;

  /**
   * Buffer used for the input in discrete time domain
   */
  float* xn_;

  /**
   * Buffer used for the output in discrete time domain
   */
  float* yn_;

  /**
   * Get the minimum of two numbers
   */
  inline int min(const int a,const int b) const;

  /**
   * Get the maximum of two numbers
   */
  inline int max(const int a,const int b) const;

  /**
   * Multiply two complex numbers and return the result
   */
  inline void mul(fftwf_complex& X,
                  const fftwf_complex& H) const;

};

#endif // FREQFILTER_H
