#pragma once

#include <Arduino.h>

#include "defines.h"
#include "enums.h"
#include "point_control.h"
#include "train_control.h"

#define INVALID_DEPARTURE_TIME 0xFFFFFFFF

TrainStatus GetCurrentTrainStatus();
TrainStatus GetNextTrainStatus();
bool TransitionState();

const char* StateToString(TrainStatus status);

extern TrainStatus g_previousStatus;
extern TrainStatus g_currentStatus;
extern TrainStatus g_nextStatus;