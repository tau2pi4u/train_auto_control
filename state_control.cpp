#include "state_control.h"

TrainStatus g_previousStatus;
TrainStatus g_currentStatus;
TrainStatus g_nextStatus;

// Returns true if TRAIN_A_IN_PLATFORM_PIN is
// low (inputs are active low), otherwise false.
bool TrainAInPlatform()
{
  return !digitalRead(TRAIN_A_IN_PLATFORM_PIN);
}

// Returns true if TRAIN_B_IN_PLATFORM_PIN is
// low (inputs are active low), otherwise false.
bool TrainBInPlatform()
{
  return !digitalRead(TRAIN_B_IN_PLATFORM_PIN);
}

// Returns true if both train in platform inputs
// indicate they are there, else false.
bool BothTrainsInPlatform()
{
  return TrainAInPlatform() && TrainBInPlatform();
}

// Returns true if TRAIN_ON_LINE pin is low
// (inputs are active low), else false;
bool TrainOnLine()
{
  return !digitalRead(TRAIN_ON_LINE_PIN);
}

// Returns true if TRAIN_ON_SLOW_DEPART_PIN
// pin is low (inputs are active low), else false;
bool TrainOnSlowX()
{
    return !digitalRead(TRAIN_ON_SLOW_X_PIN);
}

// Returns true if TRAIN_ON_SLOW_ARRIVE_PIN
// pin is low (inputs are active low), else false;
bool TrainOnSlowY()
{
    return !digitalRead(TRAIN_ON_SLOW_Y_PIN);
}

// Function for figuring out the start up status.
// Assumes that it will find two trains
TrainStatus GetCurrentTrainStatus()
{
	// This is the expected state for startup 
	if (BothTrainsInPlatform())
	{
		return TrainStatus::BothInPlatform;
	}

	// We should always have at least one train in the platform!
	if (!(TrainAInPlatform() || TrainBInPlatform()))
	{
		return TrainStatus::TrainMissing;
	}

	// We know Train A is in the platform, Train B isn't,
	// and a train is on the line, so that must be train B.
	if (TrainAInPlatform() && TrainOnLine())
	{
		return TrainStatus::TrainBOnLine;
	}

	// We know Train B is in the platform and Train A isn't,
	// and a train is on the line, so that must be train B
	if (TrainBInPlatform() && TrainOnLine())
	{
		return TrainStatus::TrainAOnLine;
	}

	// Train B is in the platform, Train A isn't
	// Train A departs on Slow X so it is departing
	if (TrainBInPlatform() && TrainOnSlowX())
	{
		return TrainStatus::TrainADeparture;
	}

	// Train B is in the platform, Train A isn't
	// Train A arrives on Slow Y
	if (TrainBInPlatform() && TrainOnSlowY())
	{
		return TrainStatus::TrainAArrival;
	}

	// Train A is in the platform, Train B isn't
	// Train A arrives on Slow X
	if (TrainAInPlatform() && TrainOnSlowX())
	{
		return TrainStatus::TrainBArrival;
	}
	
	// Train A is in the platform, Train B isn't
	// Train A departs on Slow Y
	if(TrainAInPlatform() && TrainOnSlowY())
	{
		return TrainStatus::TrainBDeparture;
	}
    
	// We should have found one of these cases if
	// there are two trains!
  	return TrainStatus::TrainMissing;
}

// Resolve next status for situation where both are in the
// platform.
// Defaults to sending out opposite train to the one which just
// ran, but if that information isn't available, default to points
// If points are invalid, default to train A.
TrainStatus NextStatusForBothInPlatform()
{
	if (!TrainAInPlatform() || !TrainBInPlatform())
	{
		return TrainStatus::TrainMissing;
	}

	if (g_previousStatus == TrainStatus::TrainAArrival)
	{
		return TrainStatus::TrainBDeparture;
	}

	if (g_previousStatus == TrainStatus::TrainBArrival)
	{
		return TrainStatus::TrainADeparture;
	}

	if (g_previousStatus == TrainStatus::None || g_previousStatus >= TrainStatus::TrainErrorBase)
	{
		PointsDirection currentPointDirection = GetCurrentPointDirection();
		switch (currentPointDirection)
		{
			case PointsDirection::ForTrainA: return TrainStatus::TrainADeparture;
			case PointsDirection::ForTrainB: return TrainStatus::TrainBDeparture;
		}

		// default to TrainADeparture
		return TrainStatus::TrainADeparture;
	}

	return TrainStatus::InvalidState;
}

// Resolve next status when the current state is 
// train A departing. The expected next state for
// this is to move to Train A on line, however it
// must first validate that it knows where train B
// is, and that points are set for train A.
// There will be overlap between slow X and fast line
// Wait for the train to exit slow X before transitioning
TrainStatus NextStatusForTrainADeparture()
{
    if (!TrainBInPlatform())
    {
        return TrainStatus::TrainMissing;
    }

    if (GetXPointFeedbackStatus() != PointsDirection::ForTrainA)
    {
        return TrainStatus::XPointFailure;
    }

    if (GetYPointFeedbackStatus() != PointsDirection::ForTrainA)
    {
        return TrainStatus::YPointFailure;
    } 

    if (TrainOnSlowX() || TrainAInPlatform())
    {
        return TrainStatus::TrainADeparture;
    }

    if (TrainOnLine())
    {
        return TrainStatus::TrainAOnLine;
    }

    return TrainStatus::InvalidState;
}

// Resolve next status when the current state is
// train A on the fast line. The expected next state
// for this is train A arrival, however it must first
// validate that it knows where train B is and that
// the points are set for train A.
// There will be overlap between the line and slow Y
// transition as soon as it sees slow Y (to test:
// do we want to wait for it to have fully crossed?)
TrainStatus NextStatusForTrainAOnLine()
{
    if (!TrainBInPlatform())
    {
        return TrainStatus::TrainMissing;
    }

    if (GetXPointFeedbackStatus() != PointsDirection::ForTrainA)
    {
        return TrainStatus::XPointFailure;
    }

    if (GetYPointFeedbackStatus() != PointsDirection::ForTrainA)
    {
        return TrainStatus::YPointFailure;
    } 

    if (TrainOnSlowY())
    {
        return TrainStatus::TrainAArrival;
    }

    if (TrainOnLine())
    {
        return TrainStatus::TrainAOnLine;
    }

    return TrainStatus::InvalidState;
}

// Resolve next status for when the current state is 
// train A arriving into the station. First check that
// train B is in the platform and points are set for 
// train A. If the train reaches the "in platform" marker
// then stop and transition to both in platform.
TrainStatus NextStatusForTrainAArrival()
{
    if (!TrainBInPlatform())
    {
        return TrainStatus::TrainMissing;
    }

    if (GetXPointFeedbackStatus() != PointsDirection::ForTrainA)
    {
        return TrainStatus::XPointFailure;
    }

    if (GetYPointFeedbackStatus() != PointsDirection::ForTrainA)
    {
        return TrainStatus::YPointFailure;
    } 

    if (TrainAInPlatform())
    {
        return TrainStatus::BothInPlatform;
    }

    if (TrainOnSlowY())
    {
        return TrainStatus::TrainAArrival;
    }

    return TrainStatus::InvalidState;
}

// Resolve next status for when the current state is
// train B departing. First validate that Train A is
// in the platform and that the points are set for train B
// Waits for train to leave slow Y and be on line before
// it transitions to Train B on line. It expects overlap
// between the two inputs.
TrainStatus NextStatusForTrainBDeparture()
{
    if (!TrainAInPlatform())
    {
        return TrainStatus::TrainMissing;
    }

    if (GetYPointFeedbackStatus() != PointsDirection::ForTrainB)
    {
        return TrainStatus::YPointFailure;
    } 

    if (GetXPointFeedbackStatus() != PointsDirection::ForTrainB)
    {
        return TrainStatus::XPointFailure;
    }

    if (TrainOnSlowY() || TrainBInPlatform())
    {
        return TrainStatus::TrainBDeparture;
    }

    if (TrainOnLine())
    {
        return TrainStatus::TrainBOnLine;
    }

    return TrainStatus::InvalidState;
}


// Resolve the next status for train B on line
// First validates that train A is in the platform
// and that the points are set for train B
// When train reaches slow X, transitions to train
// b arrival. Assumes overlap between on line and 
// slow X inputs.
TrainStatus NextStatusForTrainBOnLine()
{
    if (!TrainAInPlatform())
    {
        return TrainStatus::TrainMissing;
    }

    if (GetYPointFeedbackStatus() != PointsDirection::ForTrainB)
    {
        return TrainStatus::YPointFailure;
    } 

    if (GetXPointFeedbackStatus() != PointsDirection::ForTrainB)
    {
        return TrainStatus::XPointFailure;
    }

    if (TrainOnSlowX())
    {
        return TrainStatus::TrainBArrival;
    }

    if (TrainOnLine())
    {
        return TrainStatus::TrainBOnLine;
    }

    return TrainStatus::InvalidState;
}

// Resolve next status for train B arrival
// First check that train A is in the platform
// and that the points are set for train B.
// If train B reaches the in platform marker,
// transition to "both in platform", else 
TrainStatus NextStatusForTrainBArrival()
{
    if (!TrainAInPlatform())
    {
        return TrainStatus::TrainMissing;
    }

	if (GetYPointFeedbackStatus() != PointsDirection::ForTrainB)
    {
        return TrainStatus::YPointFailure;
    } 

    if (GetXPointFeedbackStatus() != PointsDirection::ForTrainB)
    {
        return TrainStatus::XPointFailure;
    }

    if (TrainBInPlatform())
    {
        return TrainStatus::BothInPlatform;
    }

    if (TrainOnSlowX())
    {
        return TrainStatus::TrainBArrival;
    }

    return TrainStatus::TrainMissing;
}

// We have a points failure. We should try to resolve this so
// we can move back to our previous state.
TrainStatus ResolveYPointFailure()
{
	DEBUG_PRINTLN("Trying to resolve Y point failure");
    // Try again to reset the points. If we succeed
	// return to previous state
	if (SetYPointsDirection(g_targetYPointStatus))
	{
		return g_previousStatus;
	}

	DEBUG_PRINTLN("Failed to set points, trying to switch them back and forth");
	// Try to move the points the wrong way, then back
	// again
	// Copy target direction because setting will overwrite it
	PointsDirection rightDirection = g_targetYPointStatus;
	PointsDirection wrongDirection = rightDirection == PointsDirection::ForTrainA ?
													   PointsDirection::ForTrainB :
													   PointsDirection::ForTrainA;

	SetYPointsDirection(wrongDirection);

	if(SetYPointsDirection(rightDirection))
	{
		DEBUG_PRINTLN("Points fixed!");
		return g_previousStatus;
	}    

	DEBUG_PRINTLN("Points still failed");
	return TrainStatus::YPointFailure;
}

// We have a points failure. We should try to resolve this so
// we can move back to our previous state.
TrainStatus ResolveXPointFailure()
{
	DEBUG_PRINTLN("Trying to resolve X point failure");
    // Try again to reset the points. If we succeed
	// return to previous state
	if (SetXPointsDirection(g_targetXPointStatus))
	{
		DEBUG_PRINTLN("Points fixed!");
		return g_previousStatus;
	}

	DEBUG_PRINTLN("Failed to set points, trying to switch them back and forth");
	// Try to move the points the wrong way, then back
	// again
	// Copy target direction because setting will overwrite it
	PointsDirection rightDirection = g_targetXPointStatus;
	PointsDirection wrongDirection = rightDirection == PointsDirection::ForTrainA ?
													   PointsDirection::ForTrainB :
													   PointsDirection::ForTrainA;

	SetXPointsDirection(wrongDirection);

	if(SetXPointsDirection(rightDirection))
	{
		DEBUG_PRINTLN("Points fixed!");
		return g_previousStatus;
	}    

	DEBUG_PRINTLN("Points still failed");
	return TrainStatus::XPointFailure;
}

// We have a missing train. We should try to figure out
// where it is. We can re-use the start up function here
TrainStatus ResolveTrainMissingFailure()
{
	DEBUG_PRINTLN("Trying to resolve train missing failure");
    return GetCurrentTrainStatus();
}

// We have an invalid state. Try to reset to start
TrainStatus ResolveInvalidState()
{
	DEBUG_PRINT("Trying to resolve invalid state: "); 
	DEBUG_PRINTLN(StateToString(g_currentStatus));
	return GetCurrentTrainStatus();
}

// We have an failed transition. Try to reset to start
TrainStatus ResolveFailedTransition()
{
	DEBUG_PRINT("Trying to resolve failed transition from "); 
	DEBUG_PRINT(StateToString(g_previousStatus));
	DEBUG_PRINT(" to ");
	DEBUG_PRINTLN(StateToString(g_currentStatus));
	return GetCurrentTrainStatus();
}

TrainStatus GetNextTrainStatus()
{
  switch (g_currentStatus)
  {
    case TrainStatus::BothInPlatform:    return NextStatusForBothInPlatform();  
    case TrainStatus::TrainADeparture:   return NextStatusForTrainADeparture();
    case TrainStatus::TrainAOnLine:      return NextStatusForTrainAOnLine();    
    case TrainStatus::TrainAArrival:     return NextStatusForTrainAArrival();   
    case TrainStatus::TrainBDeparture:   return NextStatusForTrainBDeparture();
    case TrainStatus::TrainBOnLine:      return NextStatusForTrainBOnLine();    
    case TrainStatus::TrainBArrival:     return NextStatusForTrainBArrival();    
    case TrainStatus::YPointFailure:     return ResolveYPointFailure();
    case TrainStatus::XPointFailure:     return ResolveXPointFailure();
    case TrainStatus::TrainMissing:      return ResolveTrainMissingFailure();
    case TrainStatus::TransitionFailure: return ResolveFailedTransition();
	default:							 return ResolveInvalidState();
  }
}

bool TransitionFromNoneOrError()
{
	switch (g_nextStatus)
	{
		case TrainStatus::BothInPlatform:
		{
			return true;
		}   
		case TrainStatus::TrainADeparture:  
		{
			if(SetPointsDirection(PointsDirection::ForTrainA))
			{
				return false;
			}
			SetTrackPowerState(TrackPowerState::ForwardSlow);
			return true;
		}
		case TrainStatus::TrainAOnLine:  
		{
			if(SetPointsDirection(PointsDirection::ForTrainA))
			{
				return false;
			}
			SetTrackPowerState(TrackPowerState::ForwardFast);
			return true;
		}   
		case TrainStatus::TrainAArrival:  
		{
			if(SetPointsDirection(PointsDirection::ForTrainA))
			{
				return false;
			}
			SetTrackPowerState(TrackPowerState::ForwardSlow);
			return true;
		}  
		case TrainStatus::TrainBDeparture:  
		{
			if(SetPointsDirection(PointsDirection::ForTrainB))
			{
				return false;
			}
			SetTrackPowerState(TrackPowerState::ReverseSlow);
			return true;
		}
		case TrainStatus::TrainBOnLine:    
		{
			if(SetPointsDirection(PointsDirection::ForTrainB))
			{
				return false;
			}
			SetTrackPowerState(TrackPowerState::ReverseFast);
			return true;
		} 
		case TrainStatus::TrainBArrival:   
		{
			if(SetPointsDirection(PointsDirection::ForTrainB))
			{
				return false;
			}
			SetTrackPowerState(TrackPowerState::ReverseSlow);
			return true;
		} 
	}
	return false;
}

bool TransitionFromBothInPlatform()
{
    if (g_nextStatus == TrainStatus::TrainADeparture)
    {
        if (SetPointsDirection(PointsDirection::ForTrainA))
        {
			DEBUG_PRINTLN("Failed to set points for train B");
            return false;
        }

        SetTrackPowerState(TrackPowerState::ForwardSlow);
        return true;
    }

    if (g_nextStatus == TrainStatus::TrainBDeparture)
    {
        if (SetPointsDirection(PointsDirection::ForTrainB))
        {
            return false;
        }

        SetTrackPowerState(TrackPowerState::ReverseSlow);
        return true;
    }

	// we want to enter the error state, not transition failure.
	if (g_nextStatus >= TrainStatus::TrainErrorBase)
	{
		SetTrackPowerState(TrackPowerState::Stop);
		return true;
	}

    return false;
}

// A transitions
bool TransitionFromTrainADeparture()
{
    if (g_nextStatus == TrainStatus::TrainAOnLine)
    {
        SetTrackPowerState(TrackPowerState::ForwardFast);
        return true;
    }

	// we want to enter the error state, not transition failure.
	if (g_nextStatus >= TrainStatus::TrainErrorBase)
	{
		SetTrackPowerState(TrackPowerState::Stop);
		return true;
	}

    return false;
}

bool TransitionFromTrainAOnLine()
{
    if (g_nextStatus == TrainStatus::TrainAArrival)
    {
        SetTrackPowerState(TrackPowerState::ForwardSlow);
        return true;
    }

	// we want to enter the error state, not transition failure.
	if (g_nextStatus >= TrainStatus::TrainErrorBase)
	{
		SetTrackPowerState(TrackPowerState::Stop);
		return true;
	}

    return false;
}

bool TransitionFromTrainAArrival()
{
    if(g_nextStatus == TrainStatus::BothInPlatform)
    {
        SetTrackPowerState(TrackPowerState::Stop);
        return true;
    }

	// we want to enter the error state, not transition failure.
	if (g_nextStatus >= TrainStatus::TrainErrorBase)
	{
		SetTrackPowerState(TrackPowerState::Stop);
		return true;
	}

    return false;
}

// B transitions
bool TransitionFromTrainBDeparture()
{
    if (g_nextStatus == TrainStatus::TrainBOnLine)
    {
        SetTrackPowerState(TrackPowerState::ReverseFast);
        return true;
    }

	// we want to enter the error state, not transition failure.
	if (g_nextStatus >= TrainStatus::TrainErrorBase)
	{
		SetTrackPowerState(TrackPowerState::Stop);
		return true;
	}

    return false;
}

bool TransitionFromTrainBOnLine()
{
    if (g_nextStatus == TrainStatus::TrainBArrival)
    {
        SetTrackPowerState(TrackPowerState::ReverseSlow);
        return true;
    }

	// we want to enter the error state, not transition failure.
	if (g_nextStatus >= TrainStatus::TrainErrorBase)
	{
		SetTrackPowerState(TrackPowerState::Stop);
		return true;
	}

    return false;
}

bool TransitionFromTrainBArrival()
{
    if (g_nextStatus == TrainStatus::BothInPlatform)
    {
        SetTrackPowerState(TrackPowerState::Stop);
        return true;
    }

	// we want to enter the error state, not transition failure.
	if (g_nextStatus >= TrainStatus::TrainErrorBase)
	{
		SetTrackPowerState(TrackPowerState::Stop);
		return true;
	}

    return false;
}

bool TransitionFromYPointFailure()
{
    return TransitionFromNoneOrError();
}

bool TransitionFromXPointFailure()
{
	return TransitionFromNoneOrError();
}

bool TransitionFromTrainMissing()
{
    return TransitionFromNoneOrError();
}

bool TransitionFromTransitionFailure()
{
	return TransitionFromNoneOrError();
}

bool _TransitionState()
{
    switch (g_currentStatus)
    {
        case TrainStatus::BothInPlatform:    return TransitionFromBothInPlatform();  
        case TrainStatus::TrainADeparture:   return TransitionFromTrainADeparture();
        case TrainStatus::TrainAOnLine:      return TransitionFromTrainAOnLine();    
        case TrainStatus::TrainAArrival:     return TransitionFromTrainAArrival();   
        case TrainStatus::TrainBDeparture:   return TransitionFromTrainBDeparture();
        case TrainStatus::TrainBOnLine:      return TransitionFromTrainBOnLine();    
        case TrainStatus::TrainBArrival:     return TransitionFromTrainBArrival();    
        case TrainStatus::YPointFailure:     return TransitionFromYPointFailure();
        case TrainStatus::XPointFailure:     return TransitionFromXPointFailure();
        case TrainStatus::TrainMissing:      return TransitionFromTrainMissing();
		case TrainStatus::TransitionFailure: return TransitionFromTransitionFailure();
		case TrainStatus::None:              return TransitionFromNoneOrError();
        default: 
		{
			DEBUG_PRINT("Unknown transition base"); 
			DEBUG_PRINTLN(StateToString(g_currentStatus));
			return false;
		}
    }
}

bool TransitionState()
{
    if (g_currentStatus == g_nextStatus)
    {
        return;
    }

    if(!_TransitionState())
    {
        SetTrackPowerState(TrackPowerState::Stop);
        g_previousStatus = g_currentStatus;
        g_currentStatus = TrainStatus::TransitionFailure;
        return false;
    }

    g_previousStatus = g_currentStatus;
    g_currentStatus = g_nextStatus;

    return true;
}

const char* StateToString(TrainStatus status)
{
    switch (status)
    {
        case TrainStatus::None:              return "None";
        case TrainStatus::BothInPlatform:    return "BothInPlatform"; 
        case TrainStatus::TrainADeparture:   return "TrainADeparture";
        case TrainStatus::TrainAOnLine:      return "TrainAOnLine"; 
        case TrainStatus::TrainAArrival:     return "TrainAArrival"; 
        case TrainStatus::TrainBDeparture:   return "TrainBDeparture";
        case TrainStatus::TrainBOnLine:      return "TrainBOnLine"; 
        case TrainStatus::TrainBArrival:     return "TrainBArrival";  
        case TrainStatus::YPointFailure:     return "YPointFailure";
        case TrainStatus::XPointFailure:     return "XPointFailure";
        case TrainStatus::TrainMissing:      return "TrainMissing";
        case TrainStatus::InvalidState:      return "InvalidState";
        case TrainStatus::TransitionFailure: return "TransitionFailure";
        default:                             return "Unknown";            
    }
}