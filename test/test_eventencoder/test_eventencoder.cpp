#include <Arduino.h>
#include <unity.h>

#include "Quest_EventEncoder.h"

#define BUFFER_SIZE 48
#define BUFFER_SIZE_IN_BITS BUFFER_SIZE * 8

uint8_t buffer[BUFFER_SIZE];

void test_new_instance_is_reset_to_no_encoded_data()
{
    for (uint16_t i = 0; i < BUFFER_SIZE; i++)
    {
        buffer[i] = i;
    }

    Quest_EventEncoder ee = Quest_EventEncoder(buffer, BUFFER_SIZE);
    TEST_ASSERT_EQUAL(None, ee.encodeState);
    TEST_ASSERT_EQUAL(0, ee.encodedBitCount);
    for (uint16_t i = 0; i < BUFFER_SIZE; i++)
    {
        TEST_ASSERT_EQUAL(0, buffer[i]);
    }
}

void test_event_without_data()
{
    Quest_EventEncoder ee = Quest_EventEncoder(buffer, BUFFER_SIZE);
    Event e;
    e.teamID = 2;   // 0b010
    e.playerID = 4; // 0b0100
    e.eventID = 21; // 0b10101
    e.dataLengthInBits = 0;

    bool result = ee.encodeToBuffer(&e);
    TEST_ASSERT_EQUAL(EventEncoded, ee.encodeState);
    TEST_ASSERT_EQUAL(true, result);
    TEST_ASSERT_EQUAL(12, ee.encodedBitCount);
    TEST_ASSERT_EQUAL(0b01000000, buffer[0] & 0b11100000); // team ID
    TEST_ASSERT_EQUAL(0b00001000, buffer[0] & 0b00011110); // player ID
    TEST_ASSERT_EQUAL(0b00000001, buffer[0] & 0b00000001); // event ID (1 bit)
    TEST_ASSERT_EQUAL(0b01010000, buffer[1] & 0b11110000); // event ID (4 bits)
}

void test_event_with_data()
{
    Quest_EventEncoder ee = Quest_EventEncoder(buffer, BUFFER_SIZE);
    Event e;
    e.teamID = 1;   // 0b001
    e.playerID = 2; // 0b0010
    e.eventID = 3;  // 0b00011
    e.data[0] = 0b10101010;
    e.data[1] = 0b01000000;
    e.dataLengthInBits = 10;

    bool result = ee.encodeToBuffer(&e);
    TEST_ASSERT_EQUAL(EventEncoded, ee.encodeState);
    TEST_ASSERT_EQUAL(true, result);
    TEST_ASSERT_EQUAL(22, ee.encodedBitCount);
    TEST_ASSERT_EQUAL(0b00100000, buffer[0] & 0b11100000); // team ID
    TEST_ASSERT_EQUAL(0b00000100, buffer[0] & 0b00011110); // player ID
    TEST_ASSERT_EQUAL(0b00000000, buffer[0] & 0b00000001); // event ID (1 bit)
    TEST_ASSERT_EQUAL(0b00110000, buffer[1] & 0b11110000); // event ID (4 bits)
    TEST_ASSERT_EQUAL(0b00001010, buffer[1] & 0b00001111); // data (4 bits)
    TEST_ASSERT_EQUAL(0b10100100, buffer[2] & 0b11111100); // data (6 bits)
}

void test_encode_clears_previous_buffer()
{
    Quest_EventEncoder ee = Quest_EventEncoder(buffer, BUFFER_SIZE);
    Event e;
    e.teamID = 1;
    e.playerID = 2;
    e.eventID = 3;
    // fill the event data with non-zero values
    for (uint8_t i = 0; i < EVENT_DATA_LENGTH; i++)
    {
        e.data[i] = 0b01010101;
    }
    e.dataLengthInBits = EVENT_DATA_LENGTH * 8;

    bool result = ee.encodeToBuffer(&e);
    TEST_ASSERT_EQUAL(true, result);
    // make sure the buffer has the event data
    for (uint8_t i = 2; i < EVENT_DATA_LENGTH + 1; i++)
    {
        Serial.println(i);
        TEST_ASSERT_NOT_EQUAL(0, buffer[i]);
    }

    // update the event to contain no data
    e.eventID = 4; // 0b0100
    e.dataLengthInBits = 0;
    result = ee.encodeToBuffer(&e);
    TEST_ASSERT_EQUAL(true, result);
    TEST_ASSERT_EQUAL(0b01000000, buffer[1] & 0b11111111); // event ID (4 bits)

    // the event data from the first event should be cleared out
    for (uint8_t i = 2; i < EVENT_DATA_LENGTH + 1; i++)
    {
        TEST_ASSERT_EQUAL(0, buffer[i]);
    }
}

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
    RUN_TEST(test_encode_clears_previous_buffer);

    UNITY_END();
}

void loop()
{
}
