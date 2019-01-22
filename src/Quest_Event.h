#ifndef quest_event_h
#define quest_event_h

#include <Arduino.h>

#define QE_DATA_LENGTH 24

#define QE_TEAM_ID_BITS 4
#define QE_PLAYER_ID_BITS 4
#define QE_EVENT_ID_BITS 8

#define QE_HEADER_BITS QE_TEAM_ID_BITS + QE_PLAYER_ID_BITS + QE_EVENT_ID_BITS

#define QE_ID_PING 1
#define QE_ID_PONG 2

#define bitRangeCheck(value, maxBits) ((value) < (1UL << maxBits) ? (true) : (false))

typedef struct event_t
{
  uint8_t teamID;
  uint8_t playerID;
  uint8_t eventID;
  uint8_t data[QE_DATA_LENGTH];
  uint8_t dataLengthInBits;
} Event;

#endif
