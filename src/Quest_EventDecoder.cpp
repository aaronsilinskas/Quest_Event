#include "Quest_EventDecoder.h"

Quest_EventDecoder::Quest_EventDecoder(uint8_t *buffer, uint8_t bufferLength) : eventReader(buffer, bufferLength)
{
}

EventDecodeResult Quest_EventDecoder::decodeEvent(uint16_t bitsAvailable, Event *event)
{
    eventReader.reset(bitsAvailable);
    event->teamID = eventReader.readBits(QE_TEAM_ID_BITS);
    event->playerID = eventReader.readBits(QE_PLAYER_ID_BITS);
    event->eventID = eventReader.readBits(QE_EVENT_ID_BITS);
    event->dataLengthInBits = 0;

    return Decoded;
}
