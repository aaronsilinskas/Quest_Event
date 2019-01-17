#ifndef quest_eventencoder_h
#define quest_eventencoder_h

#include <Quest_BitWriter.h>
#include "Quest_Event.h"

enum EventEncodeState
{
  None,
  BufferSizeExceeded,
  TeamIDSizeExceeded,
  PlayerIDSizeExceeded,
  EventIDSizeExceeded,
  EventEncoded
};

class Quest_EventEncoder
{
public:
  EventEncodeState encodeState = None;
  uint16_t encodedBitCount;

  Quest_EventEncoder(uint8_t *buffer, uint8_t bufferLength);

  bool encodeToBuffer(Event *event);

private:
  Quest_BitWriter eventWriter;

  bool writeBits(uint32_t bits, uint8_t bitsToWrite, EventEncodeState failureState);
  void setInvalidEncodeState(EventEncodeState state);
};

#endif
