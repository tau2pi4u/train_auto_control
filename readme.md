# Train Auto Control
## Introduction
Train Auto Control is a quick weekend project made for my Dad's model railway. It is based on a simple layout shown below.

![A diagram of a layout, which is based on a simple oval. Two sets of points (turnouts, switches) create a second line at the bottom of the layout. The bottom line is labelled platform A, and the top is lablled platform B. Platform A goes from right to left, platform B goes from left to right. The points on the left are labelled X and those on the right are labelled Y. The top half of the oval is coloured red an labelled "fast line". The bottom left corner up to X, plus platform B are coloured green and labelled "X Slow". The bottom right corner up to Y, plus platform A, are coloured blue and labelled "Y Slow"](line_model_diagram.png)

The line is broken into 3 sections: X Slow, Y Slow and Fast Line. It has two platforms at the bottom, controlled by two setsof points, X and Y. This allows for two trains, A and B. A moves from right to left (clockwise) and B moves from left to right (anti clockwise) through the platforms. Each train uses the platform matching its name.

Each section of track has its own current sensor, and each platform has an infrared sensor to indicate the presence of a train in the platform. 

Each set of switches also has an input to provide feedback of it's real position. 

The track power and state of the switches are controlled by the arduino.

## Setup
Clone the repository and use the arduino ide to compile. If you end up moving the ino, make sure that the all the .h and .cpp files are moved to the same folder as the .ino file.

## I/O
### Inputs
The arduino receives information about the state of the layout using 7 inputs. These are active low unless stated otherwise.
* TRAIN_A_IN_PLATFORM
* TRAIN_B_IN_PLATFORM
* TRAIN_ON_LINE
* TRAIN_ON_SLOW_X
* TRAIN_ON_SLOW_Y
* POINT_X_FEEDBACK
* POINT_Y_FEEDBACK

#### TRAIN_A_IN_PLATFORM
Provides feedback via an infrared sensor under the track in platform A. If the sensor detects an object above it, it goes low. The pin it is read from is controlled by `TRAIN_A_IN_PLATFORM_PIN` in `defines.h`.

#### TRAIN_B_IN_PLATFORM
Provides feedback via an infrared sensor under the track in platform B. If the sensor detects an object above it, it goes low. The pin it is read from is controlled by `TRAIN_B_IN_PLATFORM_PIN` in `defines.h`.

#### TRAIN_ON_LINE
Provides feedback via a current detector. If it detects current then it indicates that the track segment `FAST_LINE` is occupied by a locomotive. The input should be debounced in hardware and is required to remain active for sufficient time that it overlaps with adjacent track segments, i.e. when moving between both track segments, at least one of them should always be active. The pin it is read from is controlled by `TRAIN_ON_LINE_PIN` in `defines.h`.

#### TRAIN_ON_SLOW_X
Provides feedback via a current detector. If it detects current then it indicates that the track segment `SLOW_X` is occupied by a locomotive. The input should be debounced in hardware and is required to remain active for sufficient time that it overlaps with adjacent track segments, i.e. when moving between both track segments, at least one of them should always be active. The pin it is read from is controlled by `TRAIN_ON_SLOW_X_PIN` in `defines.h`.

#### TRAIN_ON_SLOW_Y
Provides feedback via a current detector. If it detects current then it indicates that the track segment `SLOW_Y` is occupied by a locomotive. The input should be debounced in hardware and is required to remain active for sufficient time that it overlaps with adjacent track segments, i.e. when moving between both track segments, at least one of them should always be active. The pin it is read from is controlled by `TRAIN_ON_SLOW_Y_PIN` in `defines.h`.

#### POINT_X_PLAT_A_FEEDBACK
Provides feedback on the current state of Points X. If the feedback when the points are set for platform A is low, then `INVERT_X_PLAT_A_POINT_FEEDBACK` in `defines.h` should be set to 0, else it should be set to 1. The pin from which it is read is set by `POINT_X_PLAT_A_FEEDBACK_PIN` in `defines.h`.

#### POINT_X_PLAT_B_FEEDBACK
Provides feedback on the current state of Points X. If the feedback when the points are set for platform B is low, then `INVERT_X_PLAT_B_POINT_FEEDBACK` in `defines.h` should be set to 0, else it should be set to 1. The pin from which it is read is set by `POINT_X_PLAT_B_FEEDBACK_PIN` in `defines.h`.

#### POINT_Y_PLAT_A_FEEDBACK
Provides feedback on the current state of Points Y. If the feedback when the points are set for platform A is low, then `INVERT_Y_PLAT_A_POINT_FEEDBACK` in `defines.h` should be set to 0, else it should be set to 1. The pin from which it is read is set by `POINT_Y_PLAT_A_FEEDBACK_PIN` in `defines.h`.

#### POINT_Y_PLAT_B_FEEDBACK
Provides feedback on the current state of Points Y. If the feedback when the points are set for platform B is low, then `INVERT_Y_PLAT_B_POINT_FEEDBACK` in `defines.h` should be set to 0, else it should be set to 1. The pin from which it is read is set by `POINT_Y_PLAT_B_FEEDBACK_PIN` in `defines.h`.

### Outputs

The arduino controls the state of the layout using 5 outputs: 
* TRACK_POWER
* TRACK_FORWARD
* TRACK_FAST
* POINT_X_CONTROL
* POINT_Y_CONTROL

#### TRACK_POWER 
Controls whether the locomotive should move. Whether it is active low or active high is set by `TRACK_POWER` in `defines.h`. Which pin it is assigned to is set by `TRACK_POWER_PIN` in `defines.h`.

#### TRACK_FORWARD
Controls the direction of the locomotive. Forward is defined as the forward direction for train A. Whether forward is active low or active high is set by `TRACK_FORWARD` in `defines.h`. Which pin it is assigned to is set by `TRACK_FORWARD_PIN` in `defines.h`.

#### TRACK_FAST
Controls whether the locomotive should move at high or low speed. Whether high speed is active low or active high is set by `TRACK_FAST` in `defines.h`. Which pin it is assigned to is set by `TRACK_FAST_PIN` in `defines.h`.

#### POINT_X_CONTROL
Controls whether Points X should be set for platform A or platform B. If setting the points for platform A requires a low output, `INVERT_X_POINT_CONTROL` in `defines.h` should be set to 0, otherwise it should be set to 1.

#### POINT_Y_CONTROL
Controls whether Points Y should be set for platform A or platform B. If setting the points for platform A requires a low output, `INVERT_Y_POINT_CONTROL` in `defines.h` should be set to 0, otherwise it should be set to 1.