#ifndef quest_eventdecoder_h
#define quest_eventdecoder_h

#include <Quest_BitReader.h>
#include "Quest_Event.h"

enum EventDecodeResult
{
  Decoded
};

class Quest_EventDecoder
{
public:
  Quest_EventDecoder(uint8_t *buffer, uint8_t bufferLength);

  EventDecodeResult decodeEvent(uint16_t bitsAvailable, Event *event);

private:
  Quest_BitReader eventReader;
};

#endif
