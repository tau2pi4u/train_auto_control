#include "point_control.h"

// Global variables for monitoring the expected positions of points
// There is also a feedback input to ensure they are where they're meant to be.
PointsDirection g_targetXPointStatus;
PointsDirection g_targetYPointStatus;

// Reads the input assigned as the X direction
// point status. INVERT_X_POINT_FEEDBACK can be used to
// control whether a 0 input refers to being aligned for 
// train A or B. Returns which train the point is set for.
PointsDirection GetXPointFeedbackStatus()
{
  bool xPlatAPinFeedback = digitalRead(POINT_X_PLAT_A_FEEDBACK_PIN) ^ INVERT_X_PLAT_A_POINT_FEEDBACK;
  bool xPlatBPinFeedback = digitalRead(POINT_X_PLAT_B_FEEDBACK_PIN) ^ INVERT_X_PLAT_B_POINT_FEEDBACK;
  
  if (xPlatAPinFeedback && !xPlatBPinFeedback) { return PointsDirection::ForTrainA; }
  if (!xPlatAPinFeedback && xPlatBPinFeedback) { return PointsDirection::ForTrainB; }
  return PointsDirection::Invalid;
}

// Reads the input assigned as the Y direction
// point status. INVERT_Y_POINT_FEEDBACK can be used to
// control whether a 0 input refers to being aligned for 
// train A or B. Returns which train the point is set for.
PointsDirection GetYPointFeedbackStatus()
{
  bool yPlatAPinFeedback = digitalRead(POINT_Y_PLAT_A_FEEDBACK_PIN) ^ INVERT_Y_PLAT_A_POINT_FEEDBACK;
  bool yPlatBPinFeedback = digitalRead(POINT_Y_PLAT_B_FEEDBACK_PIN) ^ INVERT_Y_PLAT_B_POINT_FEEDBACK;

  if (yPlatAPinFeedback && !yPlatBPinFeedback) { return PointsDirection::ForTrainA; }
  if (!yPlatAPinFeedback && yPlatBPinFeedback) { return PointsDirection::ForTrainB; }
  return PointsDirection::Invalid;
}

// Returns true if the points are both set as expected 
// (target == feedback) and are set to the same thing as 
// each other. Returns false otherwise.
bool PointsMatch()
{
  PointsDirection feedbackXPointStatus = GetXPointFeedbackStatus();
  PointsDirection feedbackYPointStatus = GetYPointFeedbackStatus();

  bool xTargetMatchesFeedback = feedbackXPointStatus == g_targetXPointStatus;
  bool yTargetMatchesFeedback = feedbackYPointStatus == g_targetYPointStatus;

  bool xMatchesY = feedbackXPointStatus == feedbackYPointStatus;

  return xTargetMatchesFeedback && yTargetMatchesFeedback == xMatchesY;
}

// Checks if points are in a valid state (feedback == target
// && x == y). Returns which direction they are set if valid,
// otherwise returns PointsDirection::Invalid;
PointsDirection GetCurrentPointDirection()
{
  PointsDirection feedbackXPointStatus = GetXPointFeedbackStatus();
  PointsDirection feedbackYPointStatus = GetYPointFeedbackStatus();

  bool xTargetMatchesFeedback = feedbackXPointStatus == g_targetXPointStatus;
  bool yTargetMatchesFeedback = feedbackYPointStatus == g_targetYPointStatus;

  bool xMatchesY = feedbackXPointStatus == feedbackYPointStatus;

  if(xTargetMatchesFeedback && yTargetMatchesFeedback == xMatchesY)
  {
    return feedbackXPointStatus;
  }

  return PointsDirection::Invalid;
}

// Checks that the points are set as expected for the current
// state. Assumes false in error states.
bool PointsSetCorrectly(TrainStatus current)
{
  switch (current)
  {
    case TrainStatus::TrainADeparture:
    case TrainStatus::TrainAOnLine:
    case TrainStatus::TrainAArrival:
      return PointsDirection::ForTrainA == GetCurrentPointDirection();
    case TrainStatus::TrainBDeparture:
    case TrainStatus::TrainBOnLine:
    case TrainStatus::TrainBArrival:
      return PointsDirection::ForTrainB == GetCurrentPointDirection();
    case TrainStatus::BothInPlatform:
      return PointsMatch();
    default:
      return false;
  }
}

// Tries to change the Y points to target direction.
// Due to this being a physical system, it may take time
// for the feedback to report that it has actually changed.
// Time before it gives up and reports an error can be
// set by changing POINT_WAIT_PERIOD and POINT_WAIT_COUNT.
// Whether ForTrainA is 0 or 1 can be set by changing
// INVERT_Y_POINT_FEEDBACK
bool SetYPointsDirection(PointsDirection targetDirection)
{
  DEBUG_PRINT("Changing Y points from "); 
  DEBUG_PRINT(PointDirectionToString(g_targetYPointStatus));
  DEBUG_PRINT(" to ");
  DEBUG_PRINTLN(PointDirectionToString(targetDirection));

  g_targetYPointStatus = targetDirection;

  bool targetPinValue = (g_targetYPointStatus == PointsDirection::ForTrainB) ^ INVERT_Y_POINT_CONTROL;

  digitalWrite(POINT_Y_CONTROL_PIN, targetPinValue);

  DEBUG_PRINT("Waiting on Y feedback...");
  int i = 0;
  do 
  {
    delay(POINT_WAIT_PERIOD);
    ++i;
  } while (GetYPointFeedbackStatus() != g_targetYPointStatus && i < POINT_WAIT_COUNT);

  bool success = GetYPointFeedbackStatus() == g_targetYPointStatus;

  if(success)
  {
    DEBUG_PRINTLN("Success");
  }
  else
  {
    DEBUG_PRINTLN("Failure");
  }
  return success;
}

// Tries to change the X points to target direction.
// Due to this being a physical system, it may take time
// for the feedback to report that it has actually changed.
// Time before it gives up and reports an error can be
// set by changing POINT_WAIT_PERIOD and POINT_WAIT_COUNT.
// Whether ForTrainA is 0 or 1 can be set by changing
// INVERT_X_POINT_FEEDBACK
bool SetXPointsDirection(PointsDirection targetDirection)
{
  DEBUG_PRINT("Changing X points from "); 
  DEBUG_PRINT(PointDirectionToString(g_targetXPointStatus));
  DEBUG_PRINT(" to ");
  DEBUG_PRINTLN(PointDirectionToString(targetDirection));

  g_targetXPointStatus = targetDirection;

  bool targetPinValue = (g_targetXPointStatus == PointsDirection::ForTrainB) ^ INVERT_X_POINT_CONTROL;

  digitalWrite(POINT_X_CONTROL_PIN, targetPinValue);

  DEBUG_PRINT("Waiting on X feedback...");
  int i = 0;
  do 
  {
    delay(POINT_WAIT_PERIOD);
    ++i;
  } while (GetXPointFeedbackStatus() != g_targetXPointStatus && i < POINT_WAIT_COUNT);

  bool success = GetXPointFeedbackStatus() == g_targetXPointStatus;
  if(success)
  {
    DEBUG_PRINTLN("Success");
  }
  else
  {
    DEBUG_PRINTLN("Failure");
  }
  return success;
}

// Convert enum to string for debug prints.
const char* PointDirectionToString(PointsDirection direction)
{
    switch (direction)
    {
        case PointsDirection::ForTrainA: return "For Train A";
        case PointsDirection::ForTrainB: return "For Train B";
        case PointsDirection::Invalid:   return "Invalid";
    }
}

// Try to set both points directions to the same thing.
// Returns: 
// 0 - Success
// 1 - Y point failed
// 2 - X point failed
// 3 - Both points failed
uint8_t SetPointsDirection(PointsDirection targetDirection)
{
  bool xSuccess = SetXPointsDirection(targetDirection);
  bool ySuccess = SetYPointsDirection(targetDirection);
  return (!xSuccess << 1) | !ySuccess;
}