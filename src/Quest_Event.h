#ifndef quest_event_h
#define quest_event_h

#include <Arduino.h>

#define EVENT_DATA_LENGTH 24

#define TEAM_ID_BITS 3
#define PLAYER_ID_BITS 4
#define EVENT_ID_BITS 5

#define EID_PING 1
#define EID_PONG 2

#define bitRangeCheck(value, maxBits) ((value) < (1UL << maxBits) ? (true) : (false))

typedef struct event_t
{
  uint8_t teamID;
  uint8_t playerID;
  uint8_t eventID;
  uint8_t data[EVENT_DATA_LENGTH];
  uint8_t dataLengthInBits;
} Event;

#endif
