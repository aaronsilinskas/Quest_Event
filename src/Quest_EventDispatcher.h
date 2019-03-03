#ifndef quest_eventdispatcher_h
#define quest_eventdispatcher_h

#include "Quest_Event.h"

class Quest_EventDispatcher
{
  public:
    Quest_EventDispatcher(Event *queue, uint8_t queueSize, uint8_t defaultTeamID, uint8_t defaultPlayerID);

    bool offer(Event *e);
    bool offer(uint8_t eventID, uint8_t *data, uint8_t dataLengthInBits);
    bool offer(uint8_t eventID);
    bool poll(Event *out);
    uint8_t eventsWaiting();

  private:
    Event *queue;
    uint8_t queueSize;
    uint8_t defaultTeamID;
    uint8_t defaultPlayerID;

    uint8_t queuePosition;
    uint8_t pollPosition;

    void copyData(uint8_t *dest, uint8_t *src, uint8_t bitsToCopy);
    void copyEvent(Event *dest, Event *src);
};

#endif
