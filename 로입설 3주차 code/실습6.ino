#include <Stepper.h>

const int stepsPerRevolution = 2048;
Stepper stepper(stepsPerRevolution,11,9,10,8);

void setup() {
  stepper.setSpeed(10);
}

void loop() {
  stepper.step(stepsPerRevolution);
  delay(1000);

  stepper.step(-stepsPerRevolution);
  delay(1000);
}