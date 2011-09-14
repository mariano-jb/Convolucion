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
 * \file   equalizer.cpp
 *         Implements the equalizer H(w) computation
 * \author Pablo Alvarado
 * \date   2010.12.12
 *
 * $Id: equalizer.cpp $
 */

#include "equalizer.h"
#include <cmath>
#include <iostream>
#include <cstring>

#undef _DSP_DEBUG
// #define _DSP_DEBUG

#ifdef _DSP_DEBUG
#define _debug(x) std::cerr << x
#include <iostream>
#include <fstream>
#else
#define _debug(x)
#endif

equalizer::equalizer(int bands,
                     const int hnSize,
                     const int HwSize)
  : size_(bands),hnSize_(hnSize),HwSize_(HwSize),verbose_(false),wnd_(0) {

  bands_ = new float[size_];
  freqs_ = new float[size_];
  for (int i=0;i<size_;++i) {
    bands_[i]=1.0;
    freqs_[i]=0.0;
  }

  // initialize FFT transformers
  // Buffer that holds the frequency domain data
  Hw_ = reinterpret_cast<fftwf_complex*>
        (fftwf_malloc(sizeof(fftwf_complex)*HwSize_));

  memset(Hw_,0,HwSize_*sizeof(fftwf_complex));

  // Even if the size of h(n) is hnSize_, we use HwSize because zero
  // padding is to be performed
  hn_ = reinterpret_cast<float*>(fftwf_malloc(sizeof(float)*HwSize_));
  memset(hn_,0,sizeof(float)*HwSize_);

  ifft_ = fftwf_plan_dft_c2r_1d(HwSize_,Hw_,hn_,FFTW_MEASURE);
  fft_  = fftwf_plan_dft_r2c_1d(HwSize_,hn_,Hw_,FFTW_MEASURE);

  hanning();
  //rectangular();

}

equalizer::~equalizer() {
  delete[] bands_;
  bands_=0;
  delete[] freqs_;
  freqs_=0;

  size_=0;

  // delete FFT plans
  fftwf_destroy_plan(fft_);
  fftwf_destroy_plan(ifft_);

  fftwf_free(hn_);
  fftwf_free(Hw_);

  delete[] wnd_;
}

int equalizer::bands() const {
  return size_;
}

void equalizer::setBand(const int idx,const float value) {
  if ((idx>=0) && (idx<size_)) {
    bands_[idx]=value;
    if (verbose_) {
      std::cout << "Band[" << idx << "] <- " << value << std::endl;
    }
  }
}

float equalizer::getBand(const int idx) const {
  if ((idx>=0) && (idx<size_)) {
    return bands_[idx];
  }
  return 0.0f;
}

void equalizer::setVerbose(bool v) {
  verbose_=v;
}

/*
 * Create the filter frequency response for the previously set bands.
 *
 * The steps taken are:
 * 1. Linear interpolation of the magnitude response
 * 2. Assumption of a linear phase
 * 3. Inverse FFT to determine h(n)
 * 4. Analysis of the impulse response to reduce its size, according to the
 *    epsilon relevance coefficient.
 * 5. Conversion of the truncated filter to the frequency domain.
 *
 * The resulting filter size is returned.
 */
int equalizer::createFilter() {
  _debug("equalizer::createFilter" << std::endl);

  float modHw[HwSize_+1]; // the real data (we use one extra element for
                          // convenience while ensuring parity
  interpolate(modHw);

  computeImpulseResponse(modHw); // result in hn_

  // now we have to obtain the DFT of the reduced impulse response
#ifdef _DSP_DEBUG

  for (int i=0;i<size_;++i) {
    _debug("band[" << i << "] = " << bands_[i] << std::endl);
  }

  std::ofstream out("test.dat");
  for (int i=0;i<HwSize_;++i) {
    const float re=Hw_[i][0];
    const float im=Hw_[i][1];
    const float aHw = sqrt(re*re+im*im);
    out << i << " \t" << modHw[i] << " \t"
        << re << " \t" << im << " \t"
        << aHw << " \t" << hn_[i] << std::endl;
  }
  out.close();
#endif

  return hnSize_;
}

/**
 * Return the last set frequency responce
 */
fftwf_complex* equalizer::getFrequencyResponse() {
  return Hw_;
}

void equalizer::hanning() {
  delete[] wnd_;
  wnd_ = new float[HwSize_];

  // the window should be centered on the middle
  memset(wnd_,0,HwSize_*sizeof(float));

  int offset=(HwSize_-hnSize_)/2;

  for (int n=0;n<hnSize_;++n) {
    wnd_[n+offset] = 0.5f*(1.0f-cos(6.28318530718f*n/hnSize_))/HwSize_;
  }
}

void equalizer::rectangular() {
  delete[] wnd_;
  wnd_ = new float[HwSize_];

  // the window should be centered on the middle
  memset(wnd_,0,HwSize_*sizeof(float));

  int offset=(HwSize_-hnSize_)/2;

  for (int n=0;n<hnSize_;++n) {
    wnd_[n+offset] = 1.0f;
  }
}

void equalizer::interpolate(float* data) {

  /*
   * In principle, data should have the fftSize, but, since h(n) is assumed to
   * be real then its transform is hermitian which is used by fftw3 and
   * therefore just the first half of the spectrum is interpolated
   */

  /*
   * We will be using an octave based frequency separation of the bands, but if
   * there are too many bands, then maybe the fftSize is not enough, in which
   * case we would need to use another constant.
   */
  float divider=2.0;
  if (float(HwSize_)/(1<<(size_))<1.0) { //ups, not large enough for octaves
    divider=pow(HwSize_/2,1.0/size_); // this is a larger factor
  }


  // compute the central frequencies for each band (given by variable f)
  freqs_[size_-1]=0.5-1.0/HwSize_;
  int p=HwSize_/2;// why /2? because the fft range covers the whole frequency
                    // range, and the band description has to fit on the half of
                    // that range, since the highest representable frequency
                    // is 1/2
  float pf(p);

  // positions of each band in the array, which are rounded to the next
  // integer position
  int pos[size_];
  pos[size_-1]=p-1;
  data[0]=0.0f;

  for(int i=1;i<size_;++i) {
    pf/=divider;
    p=static_cast<int>(pf + 0.5); // round the position
    pos[size_-1-i]=p-1;
    freqs_[size_-1-i] = float(p)/float(HwSize_);
  }

  /*
   * do linear interpolation, i.e. the following loops paint in a logarithmic
   * scale the frequency response for the first half of the spectrum, while
   * the second part needs to be mirrored from the first.
   */
  int from(1),to(pos[0]);

  data[0]=bands_[0];

  for (int j=from;j<to;++j) {
    data[HwSize_-j]=data[j]=bands_[0];
  }

  for (int i=1;i<size_;++i) {
    from = pos[i-1];
    to   = pos[i];

    const float m = (bands_[i]-bands_[i-1])/(to-from);
    const float b = bands_[i]-m*to;

    for(int j=from;j<to;++j) {
      data[HwSize_-j]=data[j]=m*j+b;
    }
  }

  const int h2=HwSize_/2;

  for (int j=to;j<=h2;++j) {
    data[HwSize_-j]=data[j]=bands_[size_-1];
  }
}

void equalizer::computeImpulseResponse(float* modHw) {

  const int halfSize = HwSize_/2 + 1;
  float s=1.0;

  /*
   * It is easy to prove that the linear phase of this system to achieve the
   * minimal phase and keep a real and causal system response has to be N/2,
   * which will force an alternating real part.
   */
  for (int i=0;i<=halfSize;++i) {
    Hw_[i][0]=modHw[i]*s;
    Hw_[i][1]=0.0;
    s*=-1.0;
  }

  // compute the inverse transform
  fftwf_execute(ifft_);

  // the previous line leaves h(n) in hn_

  // Now, we use a window to limit the size of the impulse response to
  // the desired one, and use zero padding to get the final frequency response
  int offset=(HwSize_-hnSize_)/2;

  // A trick to avoid adding the offset everytime.
  float* hno=hn_+offset;
  float* wndo=wnd_+offset;
  int i;
  for (i=0;i<hnSize_;++i) {
    hn_[i]=wndo[i]*hno[i]/HwSize_;
  }
  for (;i<HwSize_;++i) {
    hn_[i]=0.0f;
  }

  // Recompute the frequency response of the now trunctated frequency response
  fftwf_execute(fft_);
}

