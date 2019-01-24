#include "Quest_EventDecoder.h"

Quest_EventDecoder::Quest_EventDecoder(uint8_t *buffer, uint8_t bufferLength) : eventReader(buffer, bufferLength)
{
}

EventDecodeResult Quest_EventDecoder::decodeEvent(uint16_t bitsAvailable, Event *event)
{
    return Decoded;
}
