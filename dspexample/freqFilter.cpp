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
 * \file   freqFilter.cpp
 *         Implements a filter in the frequency domain
 * \author Pablo Alvarado
 * \date   2010.12.12
 *
 * $Id: equalizer.cpp $
 */

#include "freqFilter.h"
#include <cstring>

#undef _DSP_DEBUG
#define _DSP_DEBUG

#ifdef _DSP_DEBUG
#define _debug(x) std::cerr << x
#include <iostream>
#else
#define _debug(x)
#endif

/**
 * Get the minimum of two numbers
 */
inline int freqFilter::min(const int a,const int b) const {
  return (a<b) ? a : b;
}

/**
 * Get the maximum of two numbers
 */
int freqFilter::max(const int a,const int b) const {
  return (a>b) ? a : b;
}

inline void freqFilter::mul(fftwf_complex& X,
                            const fftwf_complex& H) const {
  float re,im;
  re=X[0]*H[0]-X[1]*H[1];
  im=X[0]*H[1]+X[1]*H[0];

  X[0]=re;
  X[1]=im;
}


/*
 * Constructor
 *
 * @param blockSize size of the data blocks to be filtered
 */
freqFilter::freqFilter(int blockSize)
  : blockSize_(blockSize),HwSize_(0),hnSize_(0),
    Hw_(0),Xw_(0),xn_(0),yn_(0) {
}

/*
 * Destructor
 */
freqFilter::~freqFilter() {

  blockSize_=0;
  HwSize_=0;
  hnSize_=0;

  fftwf_destroy_plan(fft_);
  fftwf_destroy_plan(ifft_);

  fftwf_free(Hw_);
  Hw_=0;

  fftwf_free(Xw_);
  Xw_=0;

  fftwf_free(xn_);
  xn_=0;

  fftwf_free(yn_);
  yn_=0;


}
  ;

/*
 * Set the frequency response of the filter
 */
void freqFilter::setFilter(fftwf_complex* Hw,
                           int HwSize,
                           int hnSize) {

  if (HwSize != HwSize_) {

    if (HwSize_>0) {
      fftwf_destroy_plan(fft_);
      fftwf_destroy_plan(ifft_);
    }

    if (Hw_!=0) {
      fftwf_free(Hw_);
      Hw_=0;
    }

    if (Xw_!=0) {
      fftwf_free(Xw_);
      Xw_=0;
    }

    if (xn_!=0) {
      fftwf_free(xn_);
      xn_=0;
    }

    if (yn_!=0) {
      fftwf_free(yn_);
      yn_=0;
    }

    HwSize_=HwSize;
    hnSize_=hnSize;

    Hw_ = reinterpret_cast<fftwf_complex*>
          (fftwf_malloc(sizeof(fftwf_complex)*HwSize_));

    Xw_ = reinterpret_cast<fftwf_complex*>
          (fftwf_malloc(sizeof(fftwf_complex)*HwSize_));
    memset(Xw_,0,sizeof(float)*HwSize_);

    // Even if the size of h(n) is hnSize_, we use HwSize because zero
    // padding is to be performed
    xn_ = reinterpret_cast<float*>(fftwf_malloc(sizeof(float)*HwSize_));
    memset(xn_,0,sizeof(float)*HwSize_);

    yn_ = reinterpret_cast<float*>(fftwf_malloc(sizeof(float)*HwSize_));
    memset(yn_,0,sizeof(float)*HwSize_);

    fft_  = fftwf_plan_dft_r2c_1d(HwSize_,xn_,Xw_,FFTW_MEASURE);
    ifft_ = fftwf_plan_dft_c2r_1d(HwSize_,Xw_,yn_,FFTW_MEASURE);
  }

  // The FFTW does not automatically normalize the inverse transform.
  // We force the normalization inserting the normalization factor into the
  // filter itself

#if 0 // set to zero to avoid dividing by HwSize_

  const fftwf_complex* src = Hw;
  const fftwf_complex *const srcEnd = src+HwSize_;
  fftwf_complex* dest = Hw_;

  while (src!=srcEnd) {
    (*dest)[0]=(*src)[0]/HwSize_;
    (*dest)[1]=(*src)[1]/HwSize_;

    ++src;
    ++dest;
  }

#else

  // debug line avoiding normalization
  memcpy(Hw_,Hw,sizeof(float)*HwSize_);

#endif

}

/*
 * Set the frequency response of the filter
 */
void freqFilter::setFilter(float* hn,int hnSize,int HwSize) {
  _debug(" freqFilter::setFilter()" << std::endl);

  if ((HwSize != HwSize_) || (hnSize != hnSize_)) {
    _debug("  set-up memory arrays" << std::endl);

    if (HwSize_>0) {
      fftwf_destroy_plan(fft_);
      fftwf_destroy_plan(ifft_);
    }
    if (Hw_!=0) {
      fftwf_free(Hw_);
      Hw_=0;
    }

    if (Xw_!=0) {
      fftwf_free(Xw_);
      Xw_=0;
    }

    if (xn_!=0) {
      fftwf_free(xn_);
      xn_=0;
    }

    if (yn_!=0) {
      fftwf_free(yn_);
      yn_=0;
    }


    HwSize_=HwSize;
    hnSize_=hnSize;

    Hw_ = reinterpret_cast<fftwf_complex*>
          (fftwf_malloc(sizeof(fftwf_complex)*HwSize_));

    Xw_ = reinterpret_cast<fftwf_complex*>
          (fftwf_malloc(sizeof(fftwf_complex)*HwSize_));
    memset(Xw_,0,sizeof(float)*HwSize_);

    // Even if the size of h(n) is hnSize_, we use HwSize because zero
    // padding is to be performed
    xn_ = reinterpret_cast<float*>(fftwf_malloc(sizeof(float)*HwSize_));
    yn_ = reinterpret_cast<float*>(fftwf_malloc(sizeof(float)*HwSize_));
    memset(yn_,0,sizeof(float)*HwSize_);

    fft_  = fftwf_plan_dft_r2c_1d(HwSize_,xn_,Xw_,FFTW_MEASURE);
    ifft_ = fftwf_plan_dft_c2r_1d(HwSize_,Xw_,yn_,FFTW_MEASURE);
  }

  _debug("  computing frequency response of given impulse response\n");

  memset(xn_,0,sizeof(float)*HwSize_); // zero padding

  // first move the impulse response to x(n)
  memcpy(xn_,hn,hnSize_);

  // Compute the frequency response
  fftwf_execute(fft_);

  // The FFTW does not automatically normalize the inverse transform.
  // We force the normalization inserting the normalization factor into the
  // filter itself

#if 1 // set to zero to avoid dividing by HwSize_

  const fftwf_complex* src = Xw_;
  const fftwf_complex *const srcEnd = src+HwSize_;
  fftwf_complex* dest = Hw_;

  while (src!=srcEnd) {
    (*dest)[0]=(*src)[0]/HwSize_;
    (*dest)[1]=(*src)[1]/HwSize_;

    ++src;
    ++dest;
  }

#else

  // debug line avoiding normalization
  memcpy(Hw_,Xw_,sizeof(float)*HwSize_);

#endif
}

/*
 * Filter the input block of the given size and produce
 * the output of the same size considering past evaluations.
 */
void freqFilter::filter(float* in,float* out) {
  // we use overlap-save method

  // the save-part first:
  const int hnSize1 = (hnSize_-1);
  // copy the last hnSize_-1 samples from the end of the last response to
  // the very beginning
  memcpy(xn_,xn_+blockSize_,hnSize1*sizeof(float));
  // now copy the input block after the saved block
  memcpy(xn_+hnSize1,in,blockSize_*sizeof(float));
  // when the filter was set, the rest was set to zero.

  fftwf_execute(fft_); // input to the frequency domain

  // multiply Xw_ and Hw_
  for (int n=0;n<HwSize_;++n) {

#if 0 // def _DSP_DEBUG
    if (n<8) {
      std::cerr << n << "\t ("
                << Xw_[n][0] << " + j" << Xw_[n][1] << ") x ("
                << Hw_[n][0] << " + j" << Hw_[n][1] << ") -> (";
    }
#endif

    mul(Xw_[n],Hw_[n]);

#if 0 // def _DSP_DEBUG
    if (n<8) {
      std::cerr << Xw_[n][0] << " + j" << Xw_[n][1] << ")" << std::endl;
    }
#endif

  }

  // return to the time domain
  fftwf_execute(ifft_);

  // and the last step: move the data to the output array
  memcpy(out,yn_+hnSize1,blockSize_*sizeof(float));
}

void freqFilter::reset() {
  if (HwSize_>0) {
    memset(xn_,0,sizeof(float)*HwSize_);
    memset(yn_,0,sizeof(float)*HwSize_);
  }
}
