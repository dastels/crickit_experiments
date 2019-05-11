// Copyright (c) 2016, Cristiano Urban (http://crish4cks.net)
//
// A simple C++ class created to provide an easily interaction with
// the geared stepper motor 28BYJ48 through the ULN2003APG driver.
//

#ifndef STEPPER_MOTOR_HPP
#define STEPPER_MOTOR_HPP

#include <vector>
#include "Adafruit_Crickit.h"

using namespace std;

enum  StepDirection {FORWARD, REVERSE};
enum StepStyle {SINGLE, DOUBLE, INTERLEAVE, MICROSTEP};

class StepperMotor {
 public:
  StepperMotor(Adafruit_Crickit *crickit, unsigned in1, unsigned in2, unsigned in3, unsigned in4, unsigned microsteps=16);
  void update_coils(bool microstepping=false);
  void release();
  unsigned onestep(StepDirection direction=FORWARD, StepStyle style=SINGLE);

 private:
  Adafruit_Crickit *crickit;
  unsigned microsteps;
  unsigned current_microstep;
  int *curve;
  int coils[4];                 // stepper motor driver inputs
};

#endif
