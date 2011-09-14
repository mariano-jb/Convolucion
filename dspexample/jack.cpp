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
 * \file   jack.h
 *         Implements the sound subsystem
 * \author Pablo Alvarado
 * \date   2010.12.12
 *
 * $Id: equalizer.cpp $
 */

#include "jack.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <iostream>

#include <unistd.h>

#include <QMessageBox>

#undef _DSP_DEBUG
#define _DSP_DEBUG

#ifdef _DSP_DEBUG
#define _debug(x) std::cerr << x
#else
#define _debug(x)
#endif


/**
 * Constructor
 */
jack::fileThread::fileThread() : playing_(false), exitRq_(false)
{

}

/**
 * Destructor
 */
jack::fileThread::~fileThread()
{
  exitRq_=true;
}

void jack::fileThread::suspend()
{
  playing_ = false;
}

void jack::fileThread::resume()
{
  playing_ = true;
}

void jack::fileThread::exitRequest()
{
  exitRq_ = true;
}


/**
 * Run method
 *
 * Is executed in a second thread
 */
void jack::fileThread::run()
{

  _debug("fileThread::run() called\n");

  while(!exitRq_)
  {

    if (playing_)
    {
      if ((jack::fileWindow_-jack::playWindow_+1)<jack::MaxWindows)
      {
        //_debug(".");
        playing_ = (jack::getNextBlock() > 0);
      }
    }
    usleep(jack::sleepTime_);
    jack::cleanGarbage();
  }

}

/*
 * Garbage
 *
 * This map holds all pointers to memory that needs removal.
 * The memory is not removed inmediately since the highly multithreading
 * architecture makes it difficult to safetly remove it, without implementing
 * a mutex/semaphore control, which is not advisable because it is too slow
 * for the process() method of jack.  We just ensure that for several block
 * reads and processes the memory is still there, but later on it is auto-
 * matically removed by the secondary thread.
 */
jack::garbage_type jack::garbage_;


/*
 * Size of the window being played
 */
int jack::windowSize_ = 0;

/*
 * Sample rate used
 */
int jack::sampleRate_=0;

/*
 * Size of the input and output buffers in the process
 */
int jack::bufferSize_=0;

/*
 * Input port
 */
jack_port_t* jack::inputPort_=0;

/*
 * Output port
 */
jack_port_t* jack::outputPort_=0;

/*
 * Jack client
 */
jack_client_t* jack::client_=0;

/*
 * Pointer to the current used processor
 */
processor* jack::dsp_=0;

fileManager* jack::fd_=0;
/*
 * Handler to file being played
 */
SNDFILE* jack::file_=0;

/*
 * Playing file
 */
bool jack::playingFile_=false;

/*
 * List of files to be played
 */
std::list<std::string> jack::audioFiles_;

/*
 * Mutex to protect the audioFiles_ list from multiple access
 */
QMutex jack::lock_;

/*
 * Window block
 */
int jack::fileWindow_=0;

/*
 * Window block
 */
int jack::playWindow_=0;

/*
 * Buffer with fusioned and sample-rate-fixed buffer
 */
float* jack::audioBuffer_=0;

/*
 * Buffer with fusioned and sample-rate-fixed buffer
 */
int jack::audioBufferSize_=0;

/*
 * Buffer with fusioned and sample-rate-fixed buffer
 */
float* jack::fileBuffer_ = 0;

/*
 * Buffer with fusioned and sample-rate-fixed buffer
 */
int jack::fileBufferSize_ = 0;

/*
 * Create the thread
 */
jack::fileThread jack::thread_;

/*
 * Sample rate of the file being played
 */
int jack::fileSampleRate_=0;

/*
 * Number of channels in the file
 */
int jack::fileChannels_=0;

/*
 * Time waited between windows checks
 */
int jack::sleepTime_=0;



jack::jack()
{

}

jack::~jack()
{
  close();
}

void jack::close()
{
  _debug("Calling jack::close()" << std::endl);

  _debug(" Request playing threads to stop" << std::endl);
  stopFiles();

  if (thread_.isRunning())
  {
    thread_.exitRequest();
  }

  _debug(" Waiting threads to stop...");
  usleep(2*sleepTime_);
  _debug(" done." << std::endl);

  _debug(" Clean garbage" << std::endl);
  while(!garbage_.empty()) {
    cleanGarbage();
  }

  if (client_!=NULL)
  {
    _debug(" Stop JACK client" << std::endl);
    jack_client_close(client_);
    client_=NULL;
  }
  dsp_=0;

  _debug(" Clean up remaining buffers" << std::endl);
  delete[] audioBuffer_;
  audioBuffer_=0;
  audioBufferSize_=0;

  delete[] fileBuffer_;
  fileBuffer_=0;
  fileBufferSize_=0;
}

void jack::init(fileManager* datoLeido, processor* proc)
{
  _debug("jack::init()\n");

  fd_ = datoLeido;
  fd_->initFile("Datos_Leidos.txt");

  dsp_ = proc;

  const char **ports;
  static const char *clientName = "PDS_1";
  static const char *serverName = NULL;

  static jack_options_t options = JackNullOption;
  static jack_status_t status=jack_status_t(0);

  _debug(" jack_client_open()\n");

  client_ = jack_client_open(clientName, options, &status, serverName);
  if (client_ == NULL)
  {
    std::cerr << "jack_client_open() failed, status = " << status << std::endl;
    if (status & JackServerFailed) {
      std::cerr << "Unable to connect to JACK server" << std::endl;
    }
    exit(1);
  }

  if (status & JackServerStarted)
  {
    std::cerr << "JACK server started" << std::endl;
  }

  if (status & JackNameNotUnique)
  {
    clientName = jack_get_client_name(client_);
    std::cerr << "unique name '" << clientName << "' assigned" << std::endl;
  }

  /* tell the JACK server to call `process()' whenever
   * there is work to be done.
   */
  if (jack_set_process_callback(client_,jack::process,dsp_) != 0)
  {
    std::cerr << "Unable to set process callback" << std::endl;
  };

  /* tell the JACK server to call `shutdown()' if
   * it ever shuts down, either entirely, or if it
   * just decides to stop calling us.
   */
  jack_on_shutdown(client_,jack::shutdown,dsp_);

  /*
   * Update buffer size and sample rate if necessary
   */
  if (jack_set_buffer_size_callback(client_,jack::bufferSizeChanged,dsp_)!=0)
  {
    std::cerr << "Unable to set buffer size callback" << std::endl;
  }

  if (jack_set_sample_rate_callback(client_,jack::sampleRateChanged,dsp_)!=0)
  {
    std::cerr << "Unable to set sample rate callback" << std::endl;
  }

  /*
   * Get sample rate and buffer size
   */
  sampleRate_  = jack_get_sample_rate(client_);
  bufferSize_ = jack_get_buffer_size(client_);

  sleepTime_ = static_cast<int>(double(bufferSize_)*1000000.0/sampleRate_)/2;

  dsp_->init(sampleRate_,bufferSize_);


  _debug(" create ports\n");

  /* create two ports */
  inputPort_ = jack_port_register (client_, "input",
                                   JACK_DEFAULT_AUDIO_TYPE,
                                   JackPortIsInput, 0);
  outputPort_ = jack_port_register (client_, "output",
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsOutput, 0);

  if ((inputPort_ == NULL) || (outputPort_ == NULL))
  {
    std::cerr << "no more JACK ports available" << std::endl;
    exit (1);
  }

  /* Tell the JACK server that we are ready to roll.  Our
   * process() callback will start running now.
   */
  _debug(" calling jack_activate_client.\n");
  if (jack_activate (client_))
  {
    std::cerr << "cannot activate client" << std::endl;
    exit (1);
  }

  /* Connect the ports.  You can't do this before the client is
   * activated, because we can't make connections to clients
   * that aren't running.  Note the confusing (but necessary)
   * orientation of the driver backend ports: playback ports are
   * "input" to the backend, and capture ports are "output" from
   * it.
   */
  ports=jack_get_ports(client_,NULL,NULL,JackPortIsPhysical|JackPortIsOutput);

  if (ports == NULL)
  {
    std::cerr << "no physical capture ports" << std::endl;
    exit (1);
  }

  /* connect left microphone */
  if (jack_connect(client_, ports[0], jack_port_name (inputPort_)))
  {
    std::cerr << "cannot connect input ports" << std::endl;
  }

  /* connect right microphone */
  if (jack_connect(client_, ports[1], jack_port_name (inputPort_)))
  {
    std::cerr << "cannot connect input ports" << std::endl;
  }

  free(ports);

  ports=jack_get_ports(client_, NULL,NULL,JackPortIsPhysical|JackPortIsInput);
  if (ports == NULL)
  {
    std::cerr << "no physical playback ports" << std::endl;
    exit (1);
  }

  /* connect left speaker */
  if (jack_connect(client_,jack_port_name(outputPort_), ports[0]))
  {
    std::cerr << "cannot connect output ports" << std::endl;
  }

  /* connect right speaker */
  if (jack_connect(client_,jack_port_name(outputPort_), ports[1]))
  {
    std::cerr << "cannot connect output ports" << std::endl;
  }

#define ASUS_EEE
#ifdef ASUS_EEE

  /* connect left speaker */
  if (jack_connect(client_,jack_port_name(outputPort_), ports[2]))
  {
    std::cerr << "cannot connect output ports" << std::endl;
  }

  /* connect right speaker */
  if (jack_connect(client_,jack_port_name(outputPort_), ports[3]))
  {
    std::cerr << "cannot connect output ports" << std::endl;
  }
#endif


  free(ports);
}

/**
 * Process callback
 */
int jack::process(jack_nframes_t nframes, void *arg) {

#if 0 // defined(_DSP_DEBUG)
  static const char* prog="/|\\-";
  static int progIdx=0;
  progIdx=(progIdx+1)%4;
  _debug(prog[progIdx] << "\r");
#endif

  jack_default_audio_sample_t *in, *out;

  if (playingFile_)
  {
    in = audioBuffer_+bufferSize_*(playWindow_%MaxWindows);
    ++playWindow_;
  }
  else
  {
    in  = static_cast<jack_default_audio_sample_t*>
          (jack_port_get_buffer(inputPort_, nframes));
  }
  out = static_cast<jack_default_audio_sample_t*>
        (jack_port_get_buffer(outputPort_,nframes));

  // return 0 on success, or anything else on error
  processor* dsp = reinterpret_cast<processor*>(arg);

  //Se escriben los datos leidos desde el archivo de audio
  //fd_->writeln(nframes, out);

  //A continuación se llama a process() definida dentro de dspsystem.cpp
  return (dsp->process(in,out))?0:1;
}

/*
 * Shutdown callback
 */
void jack::shutdown(void *arg) {
  processor* ptr=reinterpret_cast<processor*>(arg);
  ptr->shutdown();

  // no server => no client to close
  client_=0; // avoid trying to tell the (non-existent) server to close the
             // client
  close();
  exit(1); // without jack server, we have nothing else to do!
}

/*
 * Callback used to update used sample rate
 */
int jack::sampleRateChanged(jack_nframes_t nframes, void *arg) {
  processor* ptr=reinterpret_cast<processor*>(arg);
  return ptr->setSampleRate(nframes);
}

/*
 * Callback used to update used buffer size
 */
int jack::bufferSizeChanged(jack_nframes_t nframes, void *arg) {
  processor* ptr=reinterpret_cast<processor*>(arg);
  return ptr->setBufferSize(nframes);
}

/*
 * Stop playing from files (the capture will continue from the mic
 */
bool jack::stopFiles() {
  lock_.lock();
  thread_.suspend();
  playingFile_=false;

  audioFiles_.clear();

  // close any other previous open file
  if (file_ != 0) {
    int err;
    err=sf_close(file_);
    file_=0;
  }

  lock_.unlock();

  return true;
}

/*
 * Start playing the given file
 */
bool jack::playAlso(const char* filename)
{
  lock_.lock();
  if (!playingFile_)
  {
    play(filename);
  }
  else
  {
    audioFiles_.push_back(filename);
  }
  lock_.unlock();
  return true;
}

/*
 * Start playing the given file
 */
bool jack::play(const char* filename)
{
  _debug("\njack::play(" << filename << ")\n");

  if (!thread_.isRunning())
  {
    thread_.start();
  }

  thread_.suspend();
  playingFile_=false;

  // close any other previous open file
  if (file_ != 0)
  {
    int err;
    err=sf_close(file_);
    file_=0;

    if (err != 0)
    { // not zero if error
      QString msg = QString("Error closing file: ") + sf_error_number(err);
      QMessageBox::StandardButton button;
      button = QMessageBox::warning(0,"Error",msg);
      return false;
    }
  }

  // try to open the file
  SF_INFO info;
  info.format = 0; // this has to be set to zero before calling sf_open
  file_ = sf_open(filename,SFM_READ,&info);

  if (file_ == 0)
  { // not zero if error
    QString msg = QString("Error opening file: ") + filename;
    QMessageBox::StandardButton button;
    button = QMessageBox::warning(0,"Error",msg);
    return false;
  }

  fileSampleRate_=info.samplerate;
  fileChannels_=info.channels;

  _debug(" Jack sample rate: " << sampleRate_ << std::endl);
  _debug(" File sample rate: " << fileSampleRate_ << std::endl);
  _debug(" File channels   : " << fileChannels_ << std::endl);


  int newAudioBufferSize_=MaxWindows*bufferSize_;
  if (audioBufferSize_ < newAudioBufferSize_) {
    garbage_.push_back(std::make_pair(MaxWindows+1,audioBuffer_));

    audioBufferSize_=newAudioBufferSize_;
    audioBuffer_ = new float[audioBufferSize_];
    memset(audioBuffer_,0,audioBufferSize_*sizeof(float));
  }

  // do we need to change the size of the buffer?
  windowSize_ = static_cast<int>(ceil(bufferSize_*double(fileSampleRate_)/
                                      sampleRate_));
  int newFileBufferSize = MaxWindows*fileChannels_*windowSize_;

  if (fileBufferSize_ < newFileBufferSize)
  {
    garbage_.push_back(std::make_pair(MaxWindows+1,fileBuffer_));

    fileBufferSize_=newFileBufferSize;
    fileBuffer_=new float[fileBufferSize_];
    memset(fileBuffer_,0,fileBufferSize_*sizeof(float));
  }

  fileWindow_=0;
  playWindow_=0;

  thread_.resume();
  playingFile_=true;

  return true;
}

int jack::getNextBlock()
{
  sf_count_t cnt = 0;

  if (playingFile_ && (file_ != 0))
  {
    int wndIndex = fileWindow_%MaxWindows;

    float* mem = fileBuffer_+ windowSize_*fileChannels_*wndIndex;

    // this reads the buffer from the file
    cnt = sf_readf_float(file_,mem,windowSize_);
    fd_->writeln(windowSize_, mem);
    const int last = (cnt*bufferSize_)/windowSize_;

    // now let's interpolate the right samplerate
    float* wnd=audioBuffer_+wndIndex*bufferSize_;
    int i;
    for (i=0;i<last;++i)
    {
      // we also need to compute the average of all channels.
      int j=(i*fileSampleRate_/sampleRate_)*fileChannels_;
      const int ej=j+fileChannels_;
      float acc=mem[j];
      for (++j;j<ej;++j)
      {
        acc+=mem[j];
      }
      wnd[i]=acc/fileChannels_;
    }
    // fill the rest with 0s
    for (;i<bufferSize_;++i)
    {
      wnd[i]=0.0f;
    }
    ++fileWindow_;
  }

  // should we proceed with the next audio file?
  if (playingFile_ && (cnt==0))
  { // cnt==0 means the last file stopped playing
    _debug("jack.cpp(nextBlock) End of file." << std::endl);

    cnt=1; // the cnt flag has to be different than zero to keep playing!
    lock_.lock();
    std::string next;
    do
    {
      if (audioFiles_.empty())
      {
        // no more files given.  Stop playing them
        thread_.suspend();
        playingFile_=false;

        _debug("(jack.cpp nextBlock)No more files to play." << std::endl);

        if (file_ != 0)
        {
          int err;
          err=sf_close(file_);
          file_=0;
        }
        cnt=0; // do not play any further

        break;
      }

      next = audioFiles_.front();
      audioFiles_.pop_front();
    }
    while(!play(next.c_str()));

    lock_.unlock();
  }
  return static_cast<int>(cnt);
}

void jack::cleanGarbage()
{
  // clean the garbage
  garbage_type::iterator it=jack::garbage_.begin();
  while(it!=jack::garbage_.end())
  {
    if ( (--(it->first)) <= 0)
    {
      _debug("Cleaning garbage" << std::endl);

      delete[] it->second;
      it = garbage_.erase(it);

    }
    else
    {
      ++it;
    }
  }
}
