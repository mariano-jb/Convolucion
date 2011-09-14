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
 * \file   reverb.h
 *         Implements a simple reverberator
 * \author Pablo Alvarado
 * \date   2010.12.12
 *
 * $Id: equalizer.cpp $
 */

#ifndef REVERB_H
#define REVERB_H

/**
 * Reverberation class
 *
 * This IIR filter simulate a simple form of reverberation
 *
 * The filter follows the difference equation
 * \f[
 * y(n)=(1-\alpha)x(n) + \alpha y(n-k)
 * \f]
 */
class reverb {
public:
  /**
   * Constructor
   */
  reverb();

  /**
   * Destructor
   */
  ~reverb();

  /**
   * Init the filter operation
   *
   * @param sampleRate used sample rate
   * @param delay delay of the replicas in ms (related to k in the difference
   *              equation).  This value must be less than 4000ms and
   *              larger or equal than 1/sampleRate.
   * @param alpha attenuation factor.  Must be between 0 and 1.
   */
  void init(int sampleRate,
            float delay,
            float alpha);

  /**
   * Filter the in buffer and leave the result in out
   */
  void filter(int blockSize,
              float* in,
              float* out);

  /**
   * Return alpha value in use.
   */
  float getAlpha() const;

  /**
   * Return delay value in use, in miliseconds.
   */
  float getDelay() const;

  /**
   * Set alpha value in use.
   */
  void setAlpha(float alpha);

  /**
   * Set delay value
   */
  void setDelay(float delay);

  /**
   * Reset reverberator
   */
  void reset();

  /**
   * Constant that defines what is the maximum delay allowed in ms
   */
  static const float MaxDelay;

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
   * Index of the last processed data in the ring buffer
   */
  int idx_;

  /**
   * Sample rate
   */
  int sampleRate_;


};

#endif // COMBFILTER_H
