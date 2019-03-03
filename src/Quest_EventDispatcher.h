#ifndef quest_eventdispatcher_h
#define quest_eventdispatcher_h

#include "Quest_Event.h"

class Quest_EventDispatcher
{
  public:
    Quest_EventDispatcher(Event *queue, uint8_t queueSize);

    bool offer(Event *e);
    bool poll(Event *out);
    uint8_t eventsWaiting();

  private:
    Event *queue;
    uint8_t queueSize;
    uint8_t queuePosition;
    uint8_t pollPosition;

    void copyEvent(Event *dest, Event *src);
};

#endif
