#include "Quest_EventDispatcher.h"

Quest_EventDispatcher::Quest_EventDispatcher(Event *queue, uint8_t queueSize)
{
    this->queue = queue;
    this->queueSize = queueSize;
    this->queuePosition = 0;
    this->pollPosition = 0;
}

bool Quest_EventDispatcher::offer(Event *e)
{
    if (queuePosition >= queueSize)
    {
        return false;
    }

    copyEvent(&queue[queuePosition], e);
    queuePosition++;
    return true;
}

uint8_t Quest_EventDispatcher::eventsWaiting()
{
    return queuePosition - pollPosition;
}

bool Quest_EventDispatcher::poll(Event *out)
{
    copyEvent(out, &queue[pollPosition]);
    pollPosition++;
    return true;
}

void Quest_EventDispatcher::copyEvent(Event *dest, Event *src)
{
    dest->teamID = src->teamID;
    dest->playerID = src->playerID;
    dest->eventID = src->eventID;
    dest->dataLengthInBits = src->dataLengthInBits;
    if (src->dataLengthInBits > 0)
    {
        uint8_t bytesToCopy = src->dataLengthInBits / 8;
        if (src->dataLengthInBits % 8 > 0)
        {
            bytesToCopy++;
        }
        memcpy(dest->data, src->data, bytesToCopy);
    }
}
