#include <Arduino.h>
#include <unity.h>

#include "Quest_EventEncoder.h"
#include <Quest_BitReader.h>

#define BUFFER_SIZE 48
#define BUFFER_SIZE_IN_BITS BUFFER_SIZE * 8

uint8_t buffer[BUFFER_SIZE];

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
    event->dataLengthInBits = random(QE_DATA_LENGTH * 8);
    for (uint16_t i = 0; i < event->dataLengthInBits / 8; i++)
    {
        event->data[i] = random(256);
    }
}

void test_new_instance_is_reset_to_no_encoded_data()
{
    Quest_EventEncoder ee = Quest_EventEncoder(buffer, BUFFER_SIZE);
    TEST_ASSERT_EQUAL(None, ee.encodeState);
    TEST_ASSERT_EQUAL(0, ee.encodedBitCount);
}

void test_event_without_data()
{
    Quest_EventEncoder ee = Quest_EventEncoder(buffer, BUFFER_SIZE);
    Event e;
    randomizeEvent(&e);

    bool result = ee.encodeToBuffer(&e);

    TEST_ASSERT_EQUAL(EventEncoded, ee.encodeState);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(QE_HEADER_BITS, ee.encodedBitCount);

    Quest_BitReader qbr = Quest_BitReader(buffer, BUFFER_SIZE);
    TEST_ASSERT_EQUAL(e.teamID, qbr.readBits(QE_TEAM_ID_BITS));
    TEST_ASSERT_EQUAL(e.playerID, qbr.readBits(QE_PLAYER_ID_BITS));
    TEST_ASSERT_EQUAL(e.eventID, qbr.readBits(QE_EVENT_ID_BITS));
}

void test_event_with_data()
{
    Quest_EventEncoder ee = Quest_EventEncoder(buffer, BUFFER_SIZE);
    Event e;
    randomizeEventWithData(&e);

    bool result = ee.encodeToBuffer(&e);
    TEST_ASSERT_EQUAL(EventEncoded, ee.encodeState);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(QE_HEADER_BITS + e.dataLengthInBits, ee.encodedBitCount);

    Quest_BitReader qbr = Quest_BitReader(buffer, BUFFER_SIZE);
    TEST_ASSERT_EQUAL(e.teamID, qbr.readBits(QE_TEAM_ID_BITS));
    TEST_ASSERT_EQUAL(e.playerID, qbr.readBits(QE_PLAYER_ID_BITS));
    TEST_ASSERT_EQUAL(e.eventID, qbr.readBits(QE_EVENT_ID_BITS));

    uint8_t encodedData[QE_DATA_LENGTH];
    qbr.readBuffer(encodedData, e.dataLengthInBits);

    TEST_ASSERT_EQUAL_INT8_ARRAY(e.data, encodedData, e.dataLengthInBits / 8);
}

// test encode used multiple times
// test team too large
// test player too large
// test event ID too large
// test write buffer overflow for each field and data

void setup()
{
    delay(4000);

    UNITY_BEGIN();

    RUN_TEST(test_new_instance_is_reset_to_no_encoded_data);
    RUN_TEST(test_event_without_data);
    RUN_TEST(test_event_with_data);

    UNITY_END();
}

void loop()
{
}
