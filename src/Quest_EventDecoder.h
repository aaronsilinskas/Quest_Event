#ifndef quest_eventdecoder_h
#define quest_eventdecoder_h

#include "Quest_Event.h"

enum DecodeResult
{
    Decoded
};

DecodeResult decodeEvent(uint8_t *buffer, uint8_t length, Event *event);

#endif
