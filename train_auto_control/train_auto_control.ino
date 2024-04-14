#include "defines.h"
#include "enums.h"
#include "point_control.h"
#include "state_control.h"
#include "error.h"

#include <stdint.h>

void HandleNextState()
{
  DEBUG_PRINT("Previous: "); DEBUG_PRINTLN(StateToString(g_previousStatus));
  DEBUG_PRINT("Current:  "); DEBUG_PRINTLN(StateToString(g_currentStatus));
  g_nextStatus = GetNextTrainStatus();
  DEBUG_PRINT("Next:     "); DEBUG_PRINTLN(StateToString(g_nextStatus));
  TransitionState();

  DEBUG_DELAY(1000);
}

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < INPUT_COUNT; ++i)
  {
    pinMode(input_pins[i], INPUT_PULLUP);
  }

  for (int i = 0; i < OUTPUT_COUNT; ++i)
  {
    pinMode(output_pins[i], OUTPUT);
    digitalWrite(output_pins[i], !TRACK_POWER);
  }

  // If track power is changed to be active high
  // this will stop the train before it's had time
  // to really move.
  SetTrackPowerState(TrackPowerState::Stop);

  for (int i = 0; i < ERROR_CODE_BITS; ++i)
  {
    pinMode(ERROR_CODE_BASE + i, OUTPUT);
    digitalWrite(ERROR_CODE_BASE + i, LOW);
  }

  // Enables serial if _DEBUG is defined
  DEBUG_SERIAL(9600);

  // 7s delay to allow for startup of IR detectors
  delay(7000);

  g_previousStatus = TrainStatus::None;
  g_currentStatus  = TrainStatus::None;
  HandleNextState();
}

void loop() {
  // put your main code here, to run repeatedly:
  HandleNextState();
}
