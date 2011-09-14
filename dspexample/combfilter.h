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
 * \file   combFilter.h
 *         Implements a comb filter
 * \author Pablo Alvarado
 * \date   2010.12.12
 *
 * $Id: equalizer.cpp $
 */

#ifndef COMBFILTER_H
#define COMBFILTER_H

/**
 * Comb filter class
 *
 * This filter eliminate all multiples of a given frequency.
 *
 * The filter follows the difference equation
 * \f[
 * y(n)=\beta(x(n)-x(n-k)) + \alpha y(n-k)
 * \f]
 */
class combFilter {
public:
  /**
   * Constructor
   */
  combFilter();

  /**
   * Destructor
   */
  ~combFilter();

  /**
   * Init the filter operation
   *
   * @param sampleRate used sample rate
   * @param cutFrequency base frequency of the comb filter.  Multiples of this
   *                     frequency are filtered.
   * @param bandwidth bandwidth of the filter, in Hz.
   */
  void init(int sampleRate,
            float cutFrequency,
            float bandwidth);

  /**
   * Filter the in buffer and leave the result in out
   */
  void filter(int blockSize,
              float* in,
              float* out);

protected:
  /**
   * Ring buffer
   *
   * This will have a 2^n size to facilitate the modulo computation
   */
  float* ringBuffer_;

  /**
   * Ring buffer size
   */
  int ringBufferSize_;

  /**
   * Delay
   */
  int k_;

  /**
   * Alpha coefficient
   */
  float alpha_;

  /**
   * Beta coefficient
   */
  float beta_;

  /**
   * Index of the last processed data in the ring buffer
   */
  int idx_;
};

#endif // COMBFILTER_H
