#include "Quest_EventEncoder.h"

Quest_EventEncoder::Quest_EventEncoder(uint8_t *buffer, uint8_t bufferLength) : eventWriter(buffer, bufferLength)
{
}

EventEncodeResult Quest_EventEncoder::encodeToBuffer(Event *event)
{
    eventWriter.reset();
    if (!writeBits(event->teamID, QE_TEAM_ID_BITS))
    {
        return resetEncoder(TeamIDSizeExceeded);
    }
    if (!writeBits(event->playerID, QE_PLAYER_ID_BITS))
    {
        return resetEncoder(PlayerIDSizeExceeded);
    }
    if (!writeBits(event->eventID, QE_EVENT_ID_BITS))
    {
        return resetEncoder(EventIDSizeExceeded);
    }

    if (!eventWriter.writeBuffer(event->data, event->dataLengthInBits))
    {
        return resetEncoder(BufferSizeExceeded);
    }

    encodedBitCount = eventWriter.bitsWritten();
    return EventEncoded;
}

inline bool Quest_EventEncoder::writeBits(uint32_t bits, uint8_t bitsToWrite)
{
    if (bitRangeCheck(bits, bitsToWrite))
    {
        return eventWriter.writeBits(bits, bitsToWrite);
    }

    return false;
}

inline EventEncodeResult Quest_EventEncoder::resetEncoder(EventEncodeResult reason)
{
    encodedBitCount = 0;
    return reason;
}
