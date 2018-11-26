#ifndef quest_event_h
#define quest_event_h

#include <Arduino.h>

#define EVENT_DATA_LENGTH 32

typedef struct event_t
{
  uint8_t teamID;
  uint8_t playerID;
  uint8_t eventID;
  uint8_t data[EVENT_DATA_LENGTH];
  uint8_t dataLength;
} Event;

class Quest_Event
{
public:
  Event event;
  bool decodeEvent(uint8_t *buffer, uint8_t length);

private:
};

#endif