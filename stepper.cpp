#include <stdint.h>
#include <stdio.h>
#include <wiringPi.h>
#include "StepperMotor.h"

Adafruit_Crickit crickit;

main ()
{
  crickit.begin();  // if not... fail gracefully
  printf("Crickit started\n");

  StepperMotor stepper = StepperMotor(&crickit, CRICKIT_MOTOR_A1, CRICKIT_MOTOR_A2, CRICKIT_MOTOR_B1, CRICKIT_MOTOR_B2);

  for (int i = 0; i < 200; i++) {
    printf("Step %d\n", i);
    stepper.onestep(FORWARD);
    delay(10);
  }
}
