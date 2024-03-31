#pragma once

// Options for the points (switches, turnouts) on the layout
// Settings are defined by which train they are set for,
// inputs are expected to be converted such that the main
// logic doesn't need to consider which direction that is.
enum class PointsDirection
{
  ForTrainA,
  ForTrainB,
  Invalid
};

// Options for controlling the movement of the train. There
// are 3 outputs for this:
// Track power, which controls whether the locomotive moves or not
// Fast/Slow, which controls whether the speed of the locomotive
// Forward/Reverse, which controls the direction of the locomotive
enum class TrackPowerState
{
    Stop,
    ForwardSlow,
    ForwardFast,
    ReverseSlow,
    ReverseFast
};

// The states the train can be in. 
// States above TrainErrorBase are error states.
// Expected behaviour is
// 1. None            2. BothInPlatform 3. TrainADeparture
// 4. TrainAOnLine    3. TrainAArrival  4. BothInPlatform
// 5. TrainBDeparture 6. TrainBOnLine   7. TrainBArrival
// then repeat from 2.
// Error states are expected to return to the previous state
// once the issue has been resolved. None state should only
// be seen on startup.
// When changing this enum, remember to update the error codes
// in the readme
enum class TrainStatus
{
  None,
  BothInPlatform,
  TrainADeparture,
  TrainAOnLine,
  TrainAArrival,
  TrainBDeparture,
  TrainBOnLine,
  TrainBArrival,
  TrainErrorBase,
  TrainMissing,
  XPointFailure,
  YPointFailure,
  InvalidState,
  TransitionFailure
};