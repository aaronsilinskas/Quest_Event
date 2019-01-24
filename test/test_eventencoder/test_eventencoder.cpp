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
    event->dataLengthInBits = (1 + random(QE_DATA_LENGTH - 1)) * 8;
    for (uint16_t i = 0; i < event->dataLengthInBits / 8; i++)
    {
        event->data[i] = random(256);
    }
}

void verifyEncoderAndBuffer(Quest_EventEncoder *ee, Event *event)
{
    TEST_ASSERT_EQUAL(QE_HEADER_BITS + event->dataLengthInBits, ee->encodedBitCount);

    Quest_BitReader qbr = Quest_BitReader(buffer, BUFFER_SIZE);
    TEST_ASSERT_EQUAL(event->teamID, qbr.readBits(QE_TEAM_ID_BITS));
    TEST_ASSERT_EQUAL(event->playerID, qbr.readBits(QE_PLAYER_ID_BITS));
    TEST_ASSERT_EQUAL(event->eventID, qbr.readBits(QE_EVENT_ID_BITS));

    uint8_t encodedData[QE_DATA_LENGTH];
    qbr.readBuffer(encodedData, event->dataLengthInBits);
    if (event->dataLengthInBits > 0)
    {
        TEST_ASSERT_EQUAL_INT8_ARRAY(event->data, encodedData, event->dataLengthInBits / 8);
    }
}

void test_new_instance_is_reset_to_no_encoded_data()
{
    Quest_EventEncoder ee = Quest_EventEncoder(buffer, BUFFER_SIZE);
    TEST_ASSERT_EQUAL(0, ee.encodedBitCount);
}

void test_event_without_data()
{
    Quest_EventEncoder ee = Quest_EventEncoder(buffer, BUFFER_SIZE);
    Event e;
    randomizeEvent(&e);
    TEST_ASSERT_EQUAL(EventEncoded, ee.encodeToBuffer(&e));
    verifyEncoderAndBuffer(&ee, &e);
}

void test_event_with_data()
{
    Quest_EventEncoder ee = Quest_EventEncoder(buffer, BUFFER_SIZE);
    Event e;
    randomizeEventWithData(&e);

    TEST_ASSERT_EQUAL(EventEncoded, ee.encodeToBuffer(&e));
    verifyEncoderAndBuffer(&ee, &e);
}

void test_encoding_multiple_times()
{
    Quest_EventEncoder ee = Quest_EventEncoder(buffer, BUFFER_SIZE);
    Event e;
    randomizeEventWithData(&e);

    TEST_ASSERT_EQUAL(EventEncoded, ee.encodeToBuffer(&e));
    verifyEncoderAndBuffer(&ee, &e);

    randomizeEvent(&e);

    TEST_ASSERT_EQUAL(EventEncoded, ee.encodeToBuffer(&e));
    verifyEncoderAndBuffer(&ee, &e);

    randomizeEventWithData(&e);

    TEST_ASSERT_EQUAL(EventEncoded, ee.encodeToBuffer(&e));
    verifyEncoderAndBuffer(&ee, &e);
}

void test_team_id_too_large()
{
    Quest_EventEncoder ee = Quest_EventEncoder(buffer, BUFFER_SIZE);
    Event e;
    randomizeEvent(&e);

    // set team ID to larger than allowed value
    e.teamID = 1 << QE_TEAM_ID_BITS;

    TEST_ASSERT_EQUAL(TeamIDSizeExceeded, ee.encodeToBuffer(&e));
    TEST_ASSERT_EQUAL(0, ee.encodedBitCount);
}

void test_player_id_too_large()
{
    Quest_EventEncoder ee = Quest_EventEncoder(buffer, BUFFER_SIZE);
    Event e;
    randomizeEvent(&e);

    // set team ID to larger than allowed value
    e.playerID = 1 << QE_PLAYER_ID_BITS;

    TEST_ASSERT_EQUAL(PlayerIDSizeExceeded, ee.encodeToBuffer(&e));
    TEST_ASSERT_EQUAL(0, ee.encodedBitCount);
}

void test_event_data_too_large()
{
    // make the buffer small enough to not hold any event data
    uint8_t smallBufferSize = (QE_HEADER_BITS) / 8;
    uint8_t smallBuffer[smallBufferSize];
    Quest_EventEncoder ee = Quest_EventEncoder(smallBuffer, smallBufferSize);
    Event e;

    // put data in the event, guaranteed to exceed the buffer
    randomizeEventWithData(&e);

    TEST_ASSERT_EQUAL(BufferSizeExceeded, ee.encodeToBuffer(&e));
    TEST_ASSERT_EQUAL(0, ee.encodedBitCount);
}

void setup()
{
    delay(4000);

    UNITY_BEGIN();

    RUN_TEST(test_new_instance_is_reset_to_no_encoded_data);
    RUN_TEST(test_event_without_data);
    RUN_TEST(test_event_with_data);
    RUN_TEST(test_encoding_multiple_times);
    RUN_TEST(test_team_id_too_large);
    RUN_TEST(test_player_id_too_large);
    // No need to test event ID, 8 bits allowed and field is a uint8_t
    RUN_TEST(test_event_data_too_large);

    UNITY_END();
}

void loop()
{
}
