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
 * \file   equalizer.h
 *         Implements the equalizer H(w) computation
 * \author Pablo Alvarado
 * \date   2010.12.12
 *
 * $Id: equalizer.cpp $
 */


#ifndef EQUALIZER_H
#define EQUALIZER_H

#include <fftw3.h>

/**
 * Equalizer class
 *
 * Simple class to hold band amplification values and create the
 * filters frequency responses.
 */
class equalizer {
public:
    /**
     * Creates equilizer with the given number of bands
     * @param bands number of bands used in the equalizer
     * @param hnSize size of impulse response used.
     * @param HwSize size of the frequency response
     */
    equalizer(const int bands,
              const int hnSize,
              const int HwSize);

    /**
     * Destroys equalizer
     */
    ~equalizer();

    /**
     * Returns how many bands have been configured
     */
    int bands() const;

    /**
     * set band amplification factor.  Should be between 0 and 2;
     */
    void setBand(const int idx,const float value);

    /**
     * Get the value set for the given band.
     */
    float getBand(const int idx) const;

    /**
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
    int createFilter();

    /**
     * Return the last set frequency responce
     */
    fftwf_complex* getFrequencyResponse();

    /**
     * Set verbose mode
     */
    void setVerbose(bool v=true);

protected:
    /**
     * Record the amplification factors for each band
     */
    float* bands_;

    /**
     * Central frequencies of each band in the equalizer
     */
    float* freqs_;

    /**
     * Size given at construction time
     */
    int size_;

    /**
     * Size of the impulse response h(n)
     */
    int hnSize_;

    /**
     * Size of the frequency response H(k)
     */
    int HwSize_;

    /**
     * Verbose flag
     */
    bool verbose_;

    /**
     * fftw3 library plan for direct transform
     */
    fftwf_plan fft_;

    /**
     * fftw3 library plan for inverse transform
     */
    fftwf_plan ifft_;

    /**
     * Buffer used for frequency domain
     */
    fftwf_complex* Hw_;

    /**
     * Buffer used for impulse response
     */
    float* hn_;


    /**
     * Window initialized with hnSize_
     */
    float* wnd_;

    /**
     * Create a Hanning window in wnd_
     */
    void hanning();

    /**
     * Create a rectangular window in wnd_
     */
    void rectangular();

    /**
     * Interpolate filter from the given points, linearly
     * @param data array of floats where the final data will be stored
     * @param size of the array
     */
    void interpolate(float* data);

    /**
     * Impulse response will be computed out of the given magnitude response
     * of H(w)
     */
    void computeImpulseResponse(float* modHw);
};

#endif // EQUALIZER_H
