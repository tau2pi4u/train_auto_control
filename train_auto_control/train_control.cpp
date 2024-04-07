#include "train_control.h"

// Set the TRACK_DIRECTION_PIN to FORWARD
// Change the define for FORWARD to control
// whether HIGH or LOW mean forward
static void SetTrackDirectionForward()
{
    digitalWrite(TRACK_DIRECTION_PIN, FORWARD);
}


// Set the TRACK_DIRECTION_PIN to !FORWARD
// Change the define for FORWARD to control
// whether HIGH or LOW mean reverse
static void SetTrackDirectionReverse()
{
    digitalWrite(TRACK_DIRECTION_PIN, !FORWARD);
}

// Set the TRACK_POWER_PIN to TRACK_POWER
// Change the define for TRACK_POWER to control
// whether HIGH or LOW mean enable track power
static void SetTrackPowerOn()
{
    digitalWrite(TRACK_POWER_PIN, TRACK_POWER);
}

// Set the TRACK_POWER_PIN to !TRACK_POWER
// Change the define for TRACK_POWER to control
// whether HIGH or LOW mean disable track power
static void SetTrackPowerOff()
{
    digitalWrite(TRACK_POWER_PIN, !TRACK_POWER);
}

// Set the TRACK_FAST pin to TRACK_FAST
// Change the define for TRACK_FAST to control
// whether HIGH or LOW mean set the track to fast
static void SetTrackFast()
{
    digitalWrite(TRACK_FAST_PIN, TRACK_FAST);
}

// Set the TRACK_FAST pin to !TRACK_FAST
// Change the define for TRACK_FAST to control
// whether HIGH or LOW mean set the track to slow
static void SetTrackSlow()
{
    digitalWrite(TRACK_FAST_PIN, !TRACK_FAST);
}

// Apply the relevant inputs to align with the desired
// track power state. This should be the only visible function
// for other parts of the code
void SetTrackPowerState(TrackPowerState nextTrackPowerState)
{
    switch(nextTrackPowerState)
    {
        case TrackPowerState::Stop:
        {
            SetTrackPowerOff();
            break;
        }
        case TrackPowerState::ForwardSlow:
        {
            SetTrackDirectionForward();
            SetTrackSlow();
            SetTrackPowerOn();
            break;
        }
        case TrackPowerState::ForwardFast:
        {
            SetTrackDirectionForward();
            SetTrackFast();
            SetTrackPowerOn();
            break;
        }
        case TrackPowerState::ReverseSlow:
        {
            SetTrackDirectionReverse();
            SetTrackSlow();
            SetTrackPowerOn();
            break;
        }
        case TrackPowerState::ReverseFast:
        {
            SetTrackDirectionReverse();
            SetTrackFast();
            SetTrackPowerOn();
            break;
        }
        default:
        {
            DEBUG_PRINTLN("Invalid power state!");
            SetTrackPowerOff();
        }
    }
}