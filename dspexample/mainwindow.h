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
 * \file   mainwindow.h
 *         Implements the equalizer H(w) computation
 * \author Pablo Alvarado
 * \date   2010.12.12
 *
 * $Id: mainwindow.h $
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QFileDialog>

#include "dspsystem.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:

  enum {
    NumBands=16,
    FFTSize=1024
  };

  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  /**
   * Read the values on the real equalizer class an set the slider
   * values accordingly.
   */
  void updateEqualizer();

  /**
   * Read the values on the real reverberator class an set the controls
   * accordingly.
   */
  void updateReverb();

private:
  Ui::MainWindow *ui;

  /**
   * Tolerance value
   */
  static const float Epsilon;

  /**
   * Verbose flag
   */
  bool verbose_;

  /**
   * Timer used to recompute the filter once the user changes the
   * values
   */
  QTimer *timer_;

  /**
   * List of selected files so far
   */
  QStringList selectedFiles_;

  /**
   * Equalization changed
   */
  bool eqChanged_;

  /**
   * Pointer to an inherited class of processor, which does
   * all the real work.
   */
  dspSystem* dsp_;

  fileManager* fd_;

  /**
   * Convert a band value to the slider range
   */
  int bandToSlider(const float band) const;

  /**
   * Convert a slider value to the band range
   */
  float sliderToBand(const int val) const;

  /**
   * Summarizes the task when an equalizer slider is being moved
   */
  void setFrequencySlider(const int idx, const int value);

private slots:

  void on_delaySlider_valueChanged(int value);
  void on_resetButton_clicked();
  void on_presetsBox_currentIndexChanged(int index);
  void on_filter60_toggled(bool checked);
  void on_equalizer_toggled(bool checked);
  void on_reverb_toggled(bool checked);
  void on_fileEdit_returnPressed();
  void on_fileButton_clicked();
  void on_dialAlpha_dialMoved(int value);
  void on_alphaSpinBox_valueChanged(double );

  void on_freq01Slider_valueChanged(int value);
  void on_freq02Slider_valueChanged(int value);
  void on_freq03Slider_valueChanged(int value);
  void on_freq04Slider_valueChanged(int value);
  void on_freq05Slider_valueChanged(int value);
  void on_freq06Slider_valueChanged(int value);
  void on_freq07Slider_valueChanged(int value);
  void on_freq08Slider_valueChanged(int value);
  void on_freq09Slider_valueChanged(int value);
  void on_freq10Slider_valueChanged(int value);
  void on_freq11Slider_valueChanged(int value);
  void on_freq12Slider_valueChanged(int value);
  void on_freq13Slider_valueChanged(int value);
  void on_freq14Slider_valueChanged(int value);
  void on_freq15Slider_valueChanged(int value);
  void on_freq16Slider_valueChanged(int value);

  void update();

};

#endif // MAINWINDOW_H
