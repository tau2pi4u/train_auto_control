#pragma once

// Input pins (all set to pull up)
// Input pins default to active low, except
// for those with invert defines below. Those
// can be set to either depending on said defines.

// Total size of input pin array
#define INPUT_COUNT              9

// Signals whether train A is in the platform
// using an infrared sensor under the track.
#define TRAIN_A_IN_PLATFORM_PIN  7
// Signals whether train B is in the platform
// using an infrared sensor under the track
#define TRAIN_B_IN_PLATFORM_PIN  8
// Signals whether a train is in the block of 
// track defined as the fast line using a current
// detector.
#define TRAIN_ON_LINE_PIN       10
// Signals whether a train is in the block of track
// defined as SLOW_X using a current detector. 
#define TRAIN_ON_SLOW_X_PIN     9
// Signals whether a train is in the block of track
// defined as SLOW_Y using a current detector
#define TRAIN_ON_SLOW_Y_PIN     11
// Signals the real position of the points labelled
// X in order to confirm correct behaviour.
#define POINT_X_PLAT_A_FEEDBACK_PIN 12
#define POINT_X_PLAT_B_FEEDBACK_PIN A0
// Signals the real position of the points labelled
// Y in order to confirm correct behaviour.
#define POINT_Y_PLAT_A_FEEDBACK_PIN A1
#define POINT_Y_PLAT_B_FEEDBACK_PIN A2

// Output pins. Will default to HIGH on startup

// Number of outputs for the output array
#define OUTPUT_COUNT 5

// Control the track power output, which in
// turn controls whether power is supplied 
// to the locomotive
#define TRACK_POWER_PIN      2
// Control the track direction output.
#define TRACK_DIRECTION_PIN  3
// Control the track fast pin, to decide the
// speed of the locomotive (binary fast or slow)
#define TRACK_FAST_PIN       4
// Control the direction of the X points
#define POINT_X_CONTROL_PIN  5
// Control the direction of the Y points
#define POINT_Y_CONTROL_PIN  6

// Error code pins (also output)
// Control a seven segment display to indicate
// error states. Uses ERROR_CODE_BITS pins starting
// at base ERROR_CODE_BASE
#define ERROR_CODE_BASE      A3
#define ERROR_CODE_BITS      3

// Control whether points are A or B when 0 or 1
// If CONTROL == 0, A == 0 and B == 1
// If CONTROL == 1, A == 1 and B == 0
#define INVERT_X_POINT_CONTROL  0
#define INVERT_Y_POINT_CONTROL  0
#define INVERT_X_PLAT_A_POINT_FEEDBACK 0
#define INVERT_X_PLAT_B_POINT_FEEDBACK 0
#define INVERT_Y_PLAT_A_POINT_FEEDBACK 0
#define INVERT_Y_PLAT_B_POINT_FEEDBACK 0

// Sets the value of TRACK_POWER_PIN which makes 
// the locomotive move
#define TRACK_POWER 0
// Sets the value of TRACK_DIRECTION_PIN which 
// makes the train A move in the forward
// direction
#define FORWARD     1
// Sets the value of TRACK_FAST_PIN which makes
// the locomotive move at the higher speed
#define TRACK_FAST  1

// How long points can take to change before assuming issue
// Will check every period, for count periods. 
#define POINT_WAIT_COUNT  100
#define POINT_WAIT_PERIOD 500

// Array of inputs for ease of setup code
// New inputs will need to be added here,
// with an appropriate increment to INPUT_COUNT
static const int input_pins[INPUT_COUNT] = {
  TRAIN_A_IN_PLATFORM_PIN,
  TRAIN_B_IN_PLATFORM_PIN,
  TRAIN_ON_LINE_PIN,
  TRAIN_ON_SLOW_X_PIN,
  TRAIN_ON_SLOW_Y_PIN,
  POINT_X_PLAT_A_FEEDBACK_PIN,
  POINT_X_PLAT_B_FEEDBACK_PIN,
  POINT_Y_PLAT_A_FEEDBACK_PIN,
  POINT_Y_PLAT_B_FEEDBACK_PIN
};

// Array of outputs for ease of setup code
// New outputs will need to be added here,
// with an appropriate increment to OUTPUT_COUNT
static const int output_pins[OUTPUT_COUNT] = {
  TRACK_POWER_PIN,
  TRACK_DIRECTION_PIN,
  TRACK_FAST_PIN,
  POINT_X_CONTROL_PIN,
  POINT_Y_CONTROL_PIN 
};

// Debug code. Uncomment the debug define to
// enable feedback via serial and slow down the 
// main loop a bit

//#define _DEBUG 1
#if defined(_DEBUG)
#define DEBUG_SERIAL(baud) Serial.begin(baud)
#define DEBUG_PRINT(to_print) Serial.print(to_print)
#define DEBUG_PRINTLN(to_print) Serial.println(to_print)
#define DEBUG_DELAY(delay_ms) delay(delay_ms)
#else
#define DEBUG_SERIAL(baud)
#define DEBUG_PRINT(to_print)
#define DEBUG_PRINTLN(to_print)
#define DEBUG_DELAY(delay_ms)
#endif
