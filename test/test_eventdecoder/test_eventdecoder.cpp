#include <Arduino.h>
#include <unity.h>

#include "Quest_EventDecoder.h"
#include <Quest_BitWriter.h>

#define BUFFER_SIZE 48
#define BUFFER_SIZE_IN_BITS BUFFER_SIZE * 8

uint8_t buffer[BUFFER_SIZE];
Quest_BitWriter rawData = Quest_BitWriter(buffer, BUFFER_SIZE);

uint32_t randomBits(uint8_t bits)
{
    uint64_t maxValue = 1 << bits;
    return random(maxValue);
}

uint16_t setupRandomEvent(Event *event)
{
    event->teamID = randomBits(QE_TEAM_ID_BITS);
    event->playerID = randomBits(QE_PLAYER_ID_BITS);
    event->eventID = randomBits(QE_EVENT_ID_BITS);
    event->dataLengthInBits = 0;

    rawData.reset();
    rawData.writeBits(event->teamID, QE_TEAM_ID_BITS);
    rawData.writeBits(event->playerID, QE_PLAYER_ID_BITS);
    rawData.writeBits(event->eventID, QE_EVENT_ID_BITS);

    return rawData.bitsWritten();
}

void test_decode_event_with_no_data()
{
    Event randomEvent;
    uint16_t bitsAvailable = setupRandomEvent(&randomEvent);

    Quest_EventDecoder ed = Quest_EventDecoder(buffer, BUFFER_SIZE);
    Event decodedEvent;
    EventDecodeResult result = ed.decodeEvent(bitsAvailable, &decodedEvent);

    TEST_ASSERT_EQUAL(Decoded, result);
    TEST_ASSERT_EQUAL(randomEvent.teamID, decodedEvent.teamID);
    TEST_ASSERT_EQUAL(randomEvent.playerID, decodedEvent.playerID);
    TEST_ASSERT_EQUAL(randomEvent.eventID, decodedEvent.eventID);
    TEST_ASSERT_EQUAL(randomEvent.dataLengthInBits, decodedEvent.dataLengthInBits);
}

// test event with data
// test not enough bits for header (team + player + event IDs)
// test too much event data

void setup()
{
    delay(4000);

    UNITY_BEGIN();

    RUN_TEST(test_decode_event_with_no_data);

    UNITY_END();
}

void loop()
{
}
