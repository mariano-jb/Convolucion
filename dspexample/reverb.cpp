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
 * \file   reverb.cpp
 *         Implements a comb filter in the time domain
 * \author Pablo Alvarado
 * \date   2010.12.12
 *
 * $Id: equalizer.cpp $
 */

#include "reverb.h"
#include <cmath>
#include <cstring>

// 4000ms is the maximal allowed delay, to avoid the ring-buffer being too
// large (this is indeed too large for an efficient DSP (line TI C67x
// implementation)

const float reverb::MaxDelay = 4000.0f;

reverb::reverb()
  : ringBuffer_(0),ringBufferSize_(0),k_(0),alpha_(0.0f),idx_(0),sampleRate_(0){
}

/*
 * Destructor
 */
reverb::~reverb() {
  delete[] ringBuffer_;
  ringBufferSize_=0;
}

/*
 * Init the filter operation
 */
void reverb::init(int sampleRate,
                  float delay,
                  float alpha) {


  if (sampleRate!=sampleRate_) {
    sampleRate_=sampleRate;

    int k = MaxDelay*sampleRate_/1000.0f;

    // find the next base-2 number that can hold the required sample number
    ringBufferSize_ = 1 << static_cast<int>(ceil(log(k+1)/log(2.0f)));
    delete[] ringBuffer_;
    ringBuffer_ = new float[ringBufferSize_];
    memset(ringBuffer_,0,sizeof(float)*ringBufferSize_);/* initial conds. 0 */
  }

 setDelay(delay);
 setAlpha(alpha);

  idx_=0;
}

void reverb::setAlpha(float alpha) {
  alpha_ = (alpha<-1.0f) ? -1.0f : (alpha>1.0f) ? 1.0: alpha;
}

void reverb::setDelay(float delay) {
  // ensure the set delay is valid
  if (delay>MaxDelay) {
    delay=MaxDelay;
  }

  k_ = delay*sampleRate_/1000.0f;

  if (k_ < 1) {
    k_=1;
  }

}

/**
 * Filter the in buffer and leave the result in out
 */
void reverb::filter(int blockSize,
                        float* in,
                        float* out) {

  // the following works because the ringBuffer was set with a size
  // equal to 2^n.
  const int mask = ringBufferSize_-1;
  const float nalpha=1.0f-alpha_;

  for (int n=0;n<blockSize;++n) {
    int nmk=(idx_-k_) & mask; // performing modulo with bitwise and

    // y(n) = a*y(n-k) + (1-a)*x(n)
    ringBuffer_[idx_] = out[n] = alpha_*ringBuffer_[nmk]+nalpha*in[n];
    idx_ = (idx_+1) & mask;
  }
}

/*
 * Reset
 */
void reverb::reset() {
  memset(ringBuffer_,0,sizeof(float)*ringBufferSize_);/* initial conds. 0 */
}

/*
 * Return alpha value in use.
 */
float reverb::getAlpha() const {
  return alpha_;
}

/*
 * Return delay value in use, in miliseconds.
 */
float reverb::getDelay() const {
  return 1000.0f*float(k_)/float(sampleRate_);
}

