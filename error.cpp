#include "error.h"

// Write the error state to the output bits
// 0 is no error.
void WriteError(uint8_t error)
{
  for (int i = 0; i < ERROR_CODE_BITS; ++i)
  {
    digitalWrite(ERROR_CODE_BASE + i, (error >> i) & 1);
  }
}

// Calculate the error code for output. If state is 
// < TrainErrorBase then there is no error and output
// 0
// If state is >= TrainErrorBase then there is an error
// which should be state - TrainErrorBase.
void WriteError()
{
  uint8_t error = 0;
  if (g_currentStatus >= TrainStatus::TrainErrorBase)
  {
    DEBUG_PRINT("Error: "); DEBUG_PRINTLN(StateToString(g_currentStatus));
    error = static_cast<uint8_t>(g_currentStatus) - static_cast<uint8_t>(TrainStatus::TrainErrorBase);
  }

  WriteError(error);
}