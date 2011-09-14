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

#ifndef PROCESSOR_H
#define PROCESSOR_H

/**
 * Pure abstract class defining interface for DSP of signal blocks
 */
class processor
{
public:
  /**
   * Constructor
   */
  processor() {};

  /**
   * Destructor
   */
  virtual ~processor() {};

  /**
   * Initialization function for the current filter plan
   */
  virtual bool init(const int frameRate,
                    const int bufferSize)=0;

  /**
   * Processing function
   */
  virtual bool process(float* in,
                       float* out)=0;

  /**
   * Shutdown the processor
   */
  virtual bool shutdown()=0;

  /**
   * Set buffer size
   */
  virtual int setBufferSize(const int bufferSize)=0;

  /**
   * Set frame rate
   */
  virtual int setSampleRate(const int sampleRate)=0;

};

#endif // PROCESSOR_H
