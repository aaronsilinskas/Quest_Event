#ifndef quest_eventencoder_h
#define quest_eventencoder_h

#include <Quest_BitWriter.h>
#include "Quest_Event.h"

enum EventEncodeResult
{
  EventEncoded,
  BufferSizeExceeded,
  TeamIDSizeExceeded,
  PlayerIDSizeExceeded,
  EventIDSizeExceeded,
};

class Quest_EventEncoder
{
public:
  uint16_t encodedBitCount;

  Quest_EventEncoder(uint8_t *buffer, uint8_t bufferLength);

  EventEncodeResult encodeToBuffer(Event *event);

private:
  Quest_BitWriter eventWriter;

  bool writeBits(uint32_t bits, uint8_t bitsToWrite);
  EventEncodeResult resetEncoder(EventEncodeResult reason);
};

#endif
