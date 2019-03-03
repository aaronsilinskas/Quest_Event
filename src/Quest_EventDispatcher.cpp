#include "Quest_EventDispatcher.h"

Quest_EventDispatcher::Quest_EventDispatcher(Event *queue, uint8_t queueSize, uint8_t defaultTeamID, uint8_t defaultPlayerID)
{
    this->queue = queue;
    this->queueSize = queueSize;
    this->defaultTeamID = defaultTeamID;
    this->defaultPlayerID = defaultPlayerID;
    this->queuePosition = 0;
    this->pollPosition = 0;
}

bool Quest_EventDispatcher::offer(Event *e)
{
    if (eventsWaiting() == queueSize)
    {
        return false;
    }

    // circle to start of queue if needed
    if (queuePosition == queueSize)
    {
        queuePosition = 0;
    }

    copyEvent(&queue[queuePosition], e);
    queuePosition++;
    return true;
}

bool Quest_EventDispatcher::offer(uint8_t eventID, uint8_t *data, uint8_t dataLengthInBits)
{
    // TODO optimize to not need an Event (without hurting readability too badly)
    // TODO YUCK THIS COPIES DATA TO TEMP THEN TO BUFFER....>FIX THIS ^^^
    Event e;
    e.teamID = defaultTeamID;
    e.playerID = defaultPlayerID;
    e.eventID = eventID;
    e.dataLengthInBits = dataLengthInBits;
    if (dataLengthInBits > 0)
    {
        copyData(e.data, data, dataLengthInBits);
    }
    return this->offer(&e);
}

bool Quest_EventDispatcher::offer(uint8_t eventID)
{
    return this->offer(eventID, NULL, 0);
}

uint8_t Quest_EventDispatcher::eventsWaiting()
{
    if (pollPosition > queuePosition)
    {
        return queuePosition + queueSize - pollPosition;
    }

    return queuePosition - pollPosition;
}

bool Quest_EventDispatcher::poll(Event *out)
{
    if (eventsWaiting() == 0)
    {
        return false;
    }

    // circle to start of queue if needed
    if (pollPosition == queueSize)
    {
        pollPosition = 0;
    }

    copyEvent(out, &queue[pollPosition]);
    pollPosition++;
    return true;
}

void Quest_EventDispatcher::copyData(uint8_t *dest, uint8_t *src, uint8_t bitsToCopy)
{
    uint8_t bytesToCopy = bitsToCopy / 8;
    if (bitsToCopy % 8 > 0)
    {
        bytesToCopy++;
    }
    memcpy(dest, src, bytesToCopy);
}

void Quest_EventDispatcher::copyEvent(Event *dest, Event *src)
{
    dest->teamID = src->teamID;
    dest->playerID = src->playerID;
    dest->eventID = src->eventID;
    dest->dataLengthInBits = src->dataLengthInBits;
    if (src->dataLengthInBits > 0)
    {
        copyData(dest->data, src->data, src->dataLengthInBits);
    }
}
