#include "Quest_EventQueue.h"

Quest_EventQueue::Quest_EventQueue(Event *queue, uint8_t queueSize, uint8_t defaultTeamID, uint8_t defaultPlayerID)
{
    this->queue = queue;
    this->queueSize = queueSize;
    this->defaultTeamID = defaultTeamID;
    this->defaultPlayerID = defaultPlayerID;
    this->queuePosition = 0;
    this->pollPosition = 0;
}

bool Quest_EventQueue::offer(Event *e)
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

bool Quest_EventQueue::offer(uint8_t eventID, uint8_t *data, uint8_t dataLengthInBits)
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

    Event *e = &queue[queuePosition];
    e->teamID = defaultTeamID;
    e->playerID = defaultPlayerID;
    e->eventID = eventID;
    e->dataLengthInBits = dataLengthInBits;
    if (dataLengthInBits > 0)
    {
        copyData(e->data, data, dataLengthInBits);
    }

    queuePosition++;
    return true;
}

bool Quest_EventQueue::offer(uint8_t eventID)
{
    return this->offer(eventID, NULL, 0);
}

uint8_t Quest_EventQueue::eventsWaiting()
{
    if (pollPosition > queuePosition)
    {
        return queuePosition + queueSize - pollPosition;
    }

    return queuePosition - pollPosition;
}

bool Quest_EventQueue::poll(Event *out)
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

void Quest_EventQueue::copyData(uint8_t *dest, uint8_t *src, uint8_t bitsToCopy)
{
    uint8_t bytesToCopy = bitsToCopy / 8;
    if (bitsToCopy % 8 > 0)
    {
        bytesToCopy++;
    }
    memcpy(dest, src, bytesToCopy);
}

void Quest_EventQueue::copyEvent(Event *dest, Event *src)
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
