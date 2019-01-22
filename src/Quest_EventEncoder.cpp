#include "Quest_EventEncoder.h"

Quest_EventEncoder::Quest_EventEncoder(uint8_t *buffer, uint8_t bufferLength) : eventWriter(buffer, bufferLength)
{
}

bool Quest_EventEncoder::encodeToBuffer(Event *event)
{
    eventWriter.reset();
    bool successful = true;
    successful &= writeBits(event->teamID, QE_TEAM_ID_BITS, TeamIDSizeExceeded);
    successful &= writeBits(event->playerID, QE_PLAYER_ID_BITS, PlayerIDSizeExceeded);
    successful &= writeBits(event->eventID, QE_EVENT_ID_BITS, EventIDSizeExceeded);
    if (!successful)
    {
        return false;
    }

    if (!eventWriter.writeBuffer(event->data, event->dataLengthInBits))
    {
        setInvalidEncodeState(BufferSizeExceeded);
        return false;
    }

    encodedBitCount = eventWriter.bitsWritten();
    encodeState = EventEncoded;
    return true;
}

inline bool Quest_EventEncoder::writeBits(uint32_t bits, uint8_t bitsToWrite, EventEncodeState failureState)
{
    if (bitRangeCheck(bits, bitsToWrite))
    {
        if (eventWriter.writeBits(bits, bitsToWrite))
        {
            return true;
        }

        setInvalidEncodeState(BufferSizeExceeded);
        return false;
    }

    setInvalidEncodeState(failureState);
    return false;
}

inline void Quest_EventEncoder::setInvalidEncodeState(EventEncodeState state)
{
    encodeState = state;
    encodedBitCount = 0;
}
