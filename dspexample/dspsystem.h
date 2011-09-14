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

#ifndef DSPSYSTEM_H
#define DSPSYSTEM_H

#include "processor.h"
#include "equalizer.h"
#include "freqFilter.h"
#include "combfilter.h"
#include "reverb.h"
#include "fir.h"
#include "fileManager.h"

class dspSystem : public processor {
public:
  /**
   * Constructor
   */
  dspSystem();

  /**
   * Destructor
   */
  ~dspSystem();

  /**
   * Initialization function for the current filter plan
   */
  virtual bool init(const int frameRate,const int bufferSize);

  /**
   * Processing function inside dspsystem.h
   */
  virtual bool process(float* in,float* out);

  /**
   * Shutdown the processor
   */
  virtual bool shutdown();

  /**
   * Set buffer size
   */
  virtual int setBufferSize(const int bufferSize);

  /**
   * Set frame rate
   */
  virtual int setSampleRate(const int sampleRate);

  /**
   * Get equalizer object
   */
  equalizer* getEqualizer();

  /**
   * Update the equalizer getting the values set to the equalizer and
   * passing them to the frequency filter.
   */
  void updateEqualizer();

  /**
   * (De)activate equalizer
   */
  void setEqualizer(bool on=true);

  /**
   * (De)activate 60Hz filter
   */
  void setFilter60(bool on=true);

  /**
   * (De)activate reverberator
   */
  void setReverb(bool on=true);

  void setFFilter(bool on=true);

  void setFileManager(bool on=true);

  /**
   * Get reverberator object
   */
  reverb* getReverberator();

protected:
  /**
   * Equalizer object.  Computes the frequency response of the
   * desired equalizer filter.
   */
  equalizer* eq_;

  /**
   * Frequency domain filter (for the equalizer)
   */
  freqFilter* ff_;

  /**
   * Comb filter
   */
  combFilter* cf_;

  /**
   * Reverberator
   */
  reverb* rv_;

  fir* fFilt_;

  fileManager* fm_;

  /**
   * Equalizer impuse response size
   */
  int eqhnSize_;

  /**
   * Equalizer frequency response complete size
   */
  int eqHwSize_;

  /**
   * Sample rate
   */
  int sampleRate_;

  /**
   * Buffer size
   */
  int bufferSize_;

  /**
   * Equalizer on or off
   */
  bool equalizerOn_;

  /**
   * Filter of 60Hz on or off
   */
  bool filter60On_;

  /**
   * Reberberator on or off
   */
  bool reverbOn_;

  bool firOn_;

  bool wfOn_;
};

#endif // DSPSYSTEM_H
