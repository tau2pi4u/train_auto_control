#pragma once

#include <Arduino.h>

#include "defines.h"
#include "enums.h"

PointsDirection GetXPointFeedbackStatus(uint16_t tries = POINT_TRIES);
PointsDirection GetYPointFeedbackStatus(uint16_t tries = POINT_TRIES);
bool PointsMatch();
PointsDirection GetCurrentPointDirection();
bool PointsSetCorrectly(TrainStatus current);
bool SetXPointsDirection(PointsDirection targetDirection);
bool SetYPointsDirection(PointsDirection targetDirection);
uint8_t SetPointsDirection(PointsDirection targetDirection);
const char* PointDirectionToString(PointsDirection direction);

extern PointsDirection g_targetXPointStatus;
extern PointsDirection g_targetYPointStatus;