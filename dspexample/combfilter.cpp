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
 * \file   combfilter.cpp
 *         Implements a comb filter in the time domain
 * \author Pablo Alvarado
 * \date   2010.12.12
 *
 * $Id: equalizer.cpp $
 */

#include "combfilter.h"
#include <cmath>
#include <cstring>

combFilter::combFilter()
  : ringBuffer_(0),ringBufferSize_(0),k_(0),alpha_(0.0f),beta_(0.0f),idx_(0) {
}

/*
 * Destructor
 */
combFilter::~combFilter() {
  delete[] ringBuffer_;
  ringBufferSize_=0;
}

/*
 * Init the filter operation
 *
 * @param sampleRate used sample rate
 * @param cutFrequency base frequency of the comb filter.  Multiples of this
 *                     frequency are filtered.
 * @param bandwidth bandwidth of the filter, in Hz.
 */
void combFilter::init(int sampleRate,
                      float cutFrequency,
                      float bandwidth) {

  k_ = sampleRate/cutFrequency;

  // find the next base-2 number that can hold the required sample number
  ringBufferSize_ = 1 << static_cast<int>(ceil(log(k_+1)/log(2.0f)));


  delete[] ringBuffer_;
  ringBuffer_ = new float[ringBufferSize_];

  memset(ringBuffer_,0,sizeof(float)*ringBufferSize_);/* initial conditions 0 */

  float gamma = cos(bandwidth*3.14159265358979323f*k_/sampleRate);

  alpha_ = (1.0f-sqrt(1.0f-gamma*gamma))/gamma;
  beta_  = (1.0f+alpha_)/2.0f;

  idx_=0;
}

/**
 * Filter the in buffer and leave the result in out
 */
void combFilter::filter(int blockSize,
                        float* in,
                        float* out) {

  // the diference equation in Direct Form II is
  // v(n)=alpha*v(n-k)+beta*x(n)
  // y(n)=v(n)-v(n-k)

  const int mask = ringBufferSize_-1;
  float vnmk,vn;

  for (int n=0;n<blockSize;++n) {
    int nmk=(idx_-k_) & mask; // performing modulo with bitwise and
    vnmk = ringBuffer_[nmk];
    ringBuffer_[idx_]=vn=alpha_*vnmk+beta_*in[n];
    out[n]=vn-vnmk;
    idx_ = (idx_+1) & mask;
  }
}
