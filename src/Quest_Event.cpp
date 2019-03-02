#include "Quest_Event.h"

uint32_t randomBits(uint8_t bits)
{
    uint64_t maxValue = 1 << bits;
    return random(maxValue);
}

void randomizeEvent(Event *event)
{
    event->teamID = randomBits(QE_TEAM_ID_BITS);
    event->playerID = randomBits(QE_PLAYER_ID_BITS);
    event->eventID = randomBits(QE_EVENT_ID_BITS);
    event->dataLengthInBits = 0;
}

void randomizeEventWithData(Event *event)
{
    randomizeEvent(event);
    event->dataLengthInBits = (1 + random(QE_DATA_LENGTH - 1)) * 8;
    for (uint16_t i = 0; i < event->dataLengthInBits / 8; i++)
    {
        event->data[i] = random(256);
    }
}
