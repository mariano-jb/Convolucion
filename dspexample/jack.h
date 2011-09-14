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

#ifndef JACK_H
#define JACK_H

#include <jack/jack.h>
#include <sndfile.h>

#include <list>
#include <utility> // for std::pair

#include <QThread>
#include <QMutex>

#include "processor.h"
#include "fileManager.h"

class jack
{
public:
  /**
   * Initialization of jack
   */
  static void init(fileManager* datoLeido, processor* proc);

  /**
   * Close jack
   */
  static void close();

  /**
   * Process the in buffer and leave the filtered info in the out buffer.
   */
  static int process(jack_default_audio_sample_t* in,
                     jack_default_audio_sample_t* out);

  /**
   * Start playing the given file, stopping everything else
   */
  static bool play(const char* filename);

  /**
   * Insert the given filename into the list of files to play
   */
  static bool playAlso(const char* filename);

  /**
   * Stop playing from files (the capture will continue from the mic
   */
  static bool stopFiles();

private:
  /**
   * Only construct privately, since this class is a singleton
   */
  jack();

  /**
   * Destructor
   */
  ~jack();

  /**
   * Process callback
   */
  static int process(jack_nframes_t nframes, void *arg);

  /**
   * Shutdown callback
   */
  static void shutdown(void *arg);

  /**
   * Callback used to update used sample rate
   */
  static int sampleRateChanged(jack_nframes_t nframes, void *arg);

  /**
   * Callback used to update used buffer size
   */
  static int bufferSizeChanged(jack_nframes_t nframes, void *arg);

  /**
   * Sample rate used by jack (reproduction and mic capture)
   */
  static int sampleRate_;

  /**
   * Size of the input and output buffers in the process method
   */
  static int bufferSize_;

  /**
   * Input port
   */
  static jack_port_t *inputPort_;

  /**
   * Output port
   */
  static jack_port_t *outputPort_;

  /**
   * Jack client
   */
  static jack_client_t *client_;

  /**
   * @name Data used to play audio files
   */
   //{

  /**
   * Thread used to read the audio file, and prepare it for processing with
   * jack
   */
   class fileThread : public QThread {
   public:
     /**
      * Constructor
      */
      fileThread();

     /**
      * Destructor
      */
     virtual ~fileThread();

     /**
      * Run method
      *
      * Is executed in a second thread
      */
     virtual void run();

     /**
      * Suspend the file reading thread
      */
     void suspend();

     /**
      * Suspend the file reading thread
      */
     void resume();

     /**
      * Request the end of this thread
      */
     void exitRequest();

   protected:
     /**
      * Flag to indicate that a file is being played
      */
     bool playing_;

     /**
      * Request finilization of main loop
      */
     bool exitRq_;
   };

  friend class fileThread;

  /**
   * Pointer to the current used processor
   */
  static processor* dsp_;

  static fileManager* fd_;
  /**
   * Handler to file being played
   */
  static SNDFILE* file_;

  /**
   * Flag to indicate that the file can be played
   */
  static bool playingFile_;

  /**
   * List of files to be played
   */
  static std::list<std::string> audioFiles_;

  /**
   * Mutex to protect the audioFiles_ list from multiple access
   */
  static QMutex lock_;

  /**
   * Window being read
   */
  static int fileWindow_;

  /**
   * Window being played
   */
  static int playWindow_;

  /**
   * Buffer with fusioned and sample-rate-fixed buffer
   */
   static float* audioBuffer_;

  /**
   * Size of buffer with fusioned and sample-rate-fixed buffer
   */
   static int audioBufferSize_;

  /**
   * Buffer with file data as-is, without any normalization
   */
   static float* fileBuffer_;

  /**
   * Size of buffer with file data as-is, without any normalization
   */
   static int fileBufferSize_;

  /**
   * The file reading thread
   */
   static fileThread thread_;

   /**
    * Sample rate given in the file
    */
   static int fileSampleRate_;

   /**
    * Number of channels in the file
    */
   static int fileChannels_;

   /**
    * File window size in frames (can be different than buffer size if
    * the sample-rate in the file is different than the one of jack)
    */
   static int windowSize_;

   /**
    * Time waited between windows checks
    */
   static int sleepTime_;

   /**
    * Type to keep record of old buffers still to be removed.
    */
   typedef std::list< std::pair<int,float*> > garbage_type;

   /**
    * Garbage
    *
    * This map holds all pointers to memory that needs removal.
    * The memory is not removed inmediately since the highly multithreading
    * architecture makes it difficult to safetly remove it, without implementing
    * a mutex/semaphore control, which is not advisable because it is too slow
    * and may block the process() method of jack.
    * We just ensure that for several block reads and processes the memory is
    * still there, but later on it is automatically removed by the secondary
    * thread.
    */
   static garbage_type garbage_;

   /**
    * Clean garbage
    */
   static void cleanGarbage();

   /**
    * Read next window in file and adapt it to the proper format.
    *
    * Returns how many frames were read
    */
   static int getNextBlock();


   /**
    * Some constants
    */
   enum
   {
     MaxWindows=4
   };
  //}
};

#endif // JACK_H
