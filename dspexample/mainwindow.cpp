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
 * \file   mainwindow.cpp
 *         Implements the equalizer H(w) computation
 * \author Pablo Alvarado
 * \date   2010.12.12
 *
 * $Id: equalizer.cpp $
 */


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "jack.h"

#include <string>

#undef _DSP_DEBUG
#define _DSP_DEBUG

#ifdef _DSP_DEBUG
#define _debug(x) std::cerr << x
#include <iostream>
#else
#define _debug(x)
#endif

/**
 * Precision used by trimming
 */
const float MainWindow::Epsilon = 0.001;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    verbose_(false),
    eqChanged_(true){

  ui->setupUi(this);

  /*
   * Set up a timer 4 times in a second to check if the user
   * changed the equalizer values, and if so, then create a new
   * filter response
   */
  timer_ = new QTimer(this);
  connect(timer_, SIGNAL(timeout()), this, SLOT(update()));
  timer_->start(250);

  dsp_ = new dspSystem;
  fd_ = new fileManager;
  jack::init(fd_,dsp_);

  updateEqualizer();

  // parse some command line arguments
  QStringList argv(QCoreApplication::arguments());

  QStringList::const_iterator it(argv.begin());
  while(it!=argv.end())
  {
    if ((*it)=="-v" || (*it)=="--verbose")
    {
      verbose_=true;
    }
    else if ((*it).indexOf(".wav",0,Qt::CaseInsensitive)>0)
    {
      ui->fileEdit->setText(*it);
      std::string tmp(qPrintable(*it));
      jack::playAlso(tmp.c_str());
    }
    ++it;
  }

}

MainWindow::~MainWindow() {
  jack::close();
  delete timer_;
  delete ui;
  delete dsp_;
}

void MainWindow::update()
{
  if (eqChanged_)
  {
    _debug("Updating equalizer" << std::endl);

    dsp_->updateEqualizer();

    eqChanged_=false;
  }
}


/**
 * Convert a band value to the slider range
 */
int MainWindow::bandToSlider(const float band) const {
  return static_cast<int>(band*50.0f + 0.5f);
}

/**
 * Convert a slider value to the band range
 */
float MainWindow::sliderToBand(const int val) const {
  return float(val)/50.0;
}

/*
 * Reads the values on the real equalizer class an set the slider
 * values accordingly.
 */
void MainWindow::updateEqualizer() {
  if (dsp_!=0) {
    equalizer& eq=*(dsp_->getEqualizer());
    int idx=0;
    ui->freq01Slider->setValue(bandToSlider(eq.getBand(idx++)));
    ui->freq02Slider->setValue(bandToSlider(eq.getBand(idx++)));
    ui->freq03Slider->setValue(bandToSlider(eq.getBand(idx++)));
    ui->freq04Slider->setValue(bandToSlider(eq.getBand(idx++)));
    ui->freq05Slider->setValue(bandToSlider(eq.getBand(idx++)));
    ui->freq06Slider->setValue(bandToSlider(eq.getBand(idx++)));
    ui->freq07Slider->setValue(bandToSlider(eq.getBand(idx++)));
    ui->freq08Slider->setValue(bandToSlider(eq.getBand(idx++)));
    ui->freq09Slider->setValue(bandToSlider(eq.getBand(idx++)));
    ui->freq10Slider->setValue(bandToSlider(eq.getBand(idx++)));
    ui->freq11Slider->setValue(bandToSlider(eq.getBand(idx++)));
    ui->freq12Slider->setValue(bandToSlider(eq.getBand(idx++)));
    ui->freq13Slider->setValue(bandToSlider(eq.getBand(idx++)));
    ui->freq14Slider->setValue(bandToSlider(eq.getBand(idx++)));
    ui->freq15Slider->setValue(bandToSlider(eq.getBand(idx++)));
    ui->freq16Slider->setValue(bandToSlider(eq.getBand(idx++)));
  }
}

void MainWindow::updateReverb() {
  if (dsp_!=0) {
    reverb& rv = *(dsp_->getReverberator());

    ui->alphaSpinBox->setValue(rv.getAlpha());
    ui->delaySlider->setValue(static_cast<int>(rv.getDelay()+0.5f));
  }
}


void MainWindow::setFrequencySlider(const int idx, const int value) {

  if (!eqChanged_) {
    eqChanged_=true;
    ui->presetsBox->setCurrentIndex(0);
  }
  float band = sliderToBand(value);
  dsp_->getEqualizer()->setBand(idx,band);
}


void MainWindow::on_freq01Slider_valueChanged(int value) {
  setFrequencySlider(0,value);
}

void MainWindow::on_freq02Slider_valueChanged(int value) {
  setFrequencySlider(1,value);
}

void MainWindow::on_freq03Slider_valueChanged(int value) {
  setFrequencySlider(2,value);
}

void MainWindow::on_freq04Slider_valueChanged(int value) {
  setFrequencySlider(3,value);
}

void MainWindow::on_freq05Slider_valueChanged(int value) {
  setFrequencySlider(4,value);
}

void MainWindow::on_freq06Slider_valueChanged(int value) {
  setFrequencySlider(5,value);
}

void MainWindow::on_freq07Slider_valueChanged(int value) {
  setFrequencySlider(6,value);
}

void MainWindow::on_freq08Slider_valueChanged(int value) {
  setFrequencySlider(7,value);
}

void MainWindow::on_freq09Slider_valueChanged(int value) {
  setFrequencySlider(8,value);
}

void MainWindow::on_freq10Slider_valueChanged(int value) {
  setFrequencySlider(9,value);
}

void MainWindow::on_freq11Slider_valueChanged(int value) {
  setFrequencySlider(10,value);
}

void MainWindow::on_freq12Slider_valueChanged(int value) {
  setFrequencySlider(11,value);
}

void MainWindow::on_freq13Slider_valueChanged(int value) {
  setFrequencySlider(12,value);
}

void MainWindow::on_freq14Slider_valueChanged(int value) {
  setFrequencySlider(13,value);
}

void MainWindow::on_freq15Slider_valueChanged(int value) {
  setFrequencySlider(14,value);
}

void MainWindow::on_freq16Slider_valueChanged(int value) {
  setFrequencySlider(15,value);
}


void MainWindow::on_alphaSpinBox_valueChanged(double value) {
  ui->dialAlpha->setValue(static_cast<int>(value*1000.0+0.5));
  dsp_->getReverberator()->setAlpha(static_cast<float>(value));
}

void MainWindow::on_dialAlpha_dialMoved(int value) {
  ui->alphaSpinBox->setValue(double(value)/1000.0);
}

void MainWindow::on_fileButton_clicked() {
  selectedFiles_ =
      QFileDialog::getOpenFileNames(this,
                                   "Select one or more audio files to open",
                                   ui->fileEdit->text(),
                                   "WAV Files (*.wav)");

  if (!selectedFiles_.empty()) {
    ui->fileEdit->setText(*selectedFiles_.begin());

    jack::stopFiles();
    QStringList::iterator it;
    for (it=selectedFiles_.begin();it!=selectedFiles_.end();++it) {
      std::string tmp(qPrintable(*it));
      jack::playAlso(tmp.c_str());
    }
  }
}

void MainWindow::on_fileEdit_returnPressed() {
  jack::stopFiles();
  std::string tmp(qPrintable(ui->fileEdit->text()));
  if (!tmp.empty()) {
    jack::playAlso(tmp.c_str());
  }
}

void MainWindow::on_reverb_toggled(bool checked) {
  _debug("Reverberator " << (checked?"activated":"deactivated") << std::endl);
  dsp_->setReverb(checked);
  dsp_->setFFilter(checked);
  dsp_->setFileManager(checked);
}

void MainWindow::on_equalizer_toggled(bool checked) {
  _debug("Equalizer " << (checked?"activated":"deactivated") << std::endl);
  dsp_->setEqualizer(checked);
}

void MainWindow::on_filter60_toggled(bool checked) {
  _debug("60 Hz filter " << (checked?"activated":"deactivated") << std::endl);
  dsp_->setFilter60(checked);
}


void MainWindow::on_presetsBox_currentIndexChanged(int index) {
  _debug("MainWindow::on_presetsBox_currentIndexChanged(" << index << ")\n");

  static const float presets[5][16] =
  { {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
     0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, 0.7f, 0.5f, 0.3f, 0.1f, 0.0f,
     0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
     0.0f, 0.1f, 0.3f, 0.5f, 0.7f, 1.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.1f, 0.3f, 0.5f, 0.7f, 1.0f, 1.0f,
     1.0f, 1.0f, 0.7f, 0.5f, 0.3f, 0.1f, 0.0f, 0.0f} };

  if ((index>0) || !eqChanged_) {
    for (int n=0;n<16;++n) {
      dsp_->getEqualizer()->setBand(n,presets[index][n]);
    }
    eqChanged_=true;
    updateEqualizer();
  }

}

void MainWindow::on_resetButton_clicked(){
  dsp_->getReverberator()->reset();
}

void MainWindow::on_delaySlider_valueChanged(int value){
  dsp_->getReverberator()->setDelay(float(value));
}
