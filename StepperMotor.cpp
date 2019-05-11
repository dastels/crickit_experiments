#include <cassert>
#include <cstdlib>
#include <stdio.h>
#include <cmath>
#include <algorithm>
#include <wiringPi.h>
#include "StepperMotor.h"

using namespace std;


StepperMotor::StepperMotor(Adafruit_Crickit *crickit, unsigned in1, unsigned in2, unsigned in3, unsigned in4, unsigned ms)
{
  crickit = crickit;
  coils[0] = in1;
  coils[1] = in2;
  coils[2] = in3;
  coils[3] = in4;

  for (int i = 0; i < 4; i++) {
    crickit->setPWMFreq(coils[i], 2000);
    crickit->analogWrite(coils[i], 0);
  }

  microsteps = 0;
  if (ms < 2) {
    printf("ERROR: microsteps must be at least 2, but was %d\n", ms);
  }
  if (ms % 2 == 1) {
    printf("ERROR: microsteps must be even, but was %d\n", ms);
  }
  microsteps = ms;
  current_microstep = 0;

  curve = new int[ms];
  for (int i = 0; i < ms + 1; i++) {
    curve[i] = (int)round(0xffff * sin(3.14159 / (2 * ms * i)));
    printf("Curve[%d]: %d\n", i, curve[i]);
  }
  update_coils();
}


void StepperMotor::update_coils(bool microstepping)
{
  unsigned duty_cycles[] = {0, 0, 0, 0};
  int trailing_coil = ((unsigned)(current_microstep / microsteps)) % 4;
  int leading_coil = (trailing_coil + 1) % 4;
  int microstep = current_microstep % microsteps;
  duty_cycles[leading_coil] = curve[microstep];
  duty_cycles[trailing_coil] = curve[microsteps - microstep];

  // This ensures DOUBLE steps use full torque. Without it, we'd use partial torque from the
  // microstepping curve (0xb504).
  if (!microstepping && (duty_cycles[leading_coil] == duty_cycles[trailing_coil] && duty_cycles[leading_coil] > 0)) {
    duty_cycles[leading_coil] = 0xffff;
    duty_cycles[trailing_coil] = 0xffff;

    // Energize coils as appropriate:
    for (int i = 0; i < 4; i++) {
      printf("Writing 0x%02u to 0x%02u\n", duty_cycles[i], coils[i]);
      crickit->analogWrite(coils[i], duty_cycles[i]);
    }
  } else {
    printf("leading: %d, trailing:  %d\n", duty_cycles[leading_coil], duty_cycles[trailing_coil]);
  }
}


void StepperMotor::release()
{
  // Releases all the coils so the motor can free spin, also won't use any power
  // De-energize coils:
  for (int i = 0; i < 4; i++) {
    crickit->analogWrite(coils[i], 0);
  }
}


// Performs one step of a particular style. The actual rotation amount will vary by style.
// `SINGLE` and `DOUBLE` will normal cause a full step rotation. `INTERLEAVE` will normally
// do a half step rotation. `MICROSTEP` will perform the smallest configured step.
// When step styles are mixed, subsequent `SINGLE`, `DOUBLE` or `INTERLEAVE` steps may be
// less than normal in order to align to the desired style's pattern.
// :param int direction: Either `FORWARD` or `BACKWARD`
// :param int style: `SINGLE`, `DOUBLE`, `INTERLEAVE`"""

unsigned StepperMotor::onestep(StepDirection direction, StepStyle style)
{
  // Adjust current steps based on the direction and type of step.
  unsigned step_size = 0;
  if (style == MICROSTEP) {
    step_size = 1;
  } else {
    unsigned half_step = microsteps / 2;
    unsigned full_step = microsteps;
    // Its possible the previous steps were MICROSTEPS so first align with the interleave pattern.
    unsigned additional_microsteps = current_microstep % half_step;
    if (additional_microsteps != 0) {
      // We set current_microstep directly because our step size varies depending on the direction.
      if (direction == FORWARD) {
        current_microstep += (half_step - additional_microsteps);
      } else {
        current_microstep -= additional_microsteps;
      }
      step_size = 0;
    } else if (style == INTERLEAVE) {
      step_size = half_step;
    }

    unsigned current_interleave = current_microstep / half_step;
    if ((style == SINGLE && current_interleave % 2 == 1) || (style == DOUBLE && current_interleave % 2 == 0)) {
      step_size = half_step;
    } else if (style == SINGLE || style == DOUBLE) {
      step_size = full_step;
    }
  }


  if (direction == FORWARD) {
    current_microstep += step_size;
  } else {
    current_microstep -= step_size;
  }

  // Now that we know our target microstep we can determine how to energize the four coils.
  update_coils(style == MICROSTEP);

  return current_microstep;
}
