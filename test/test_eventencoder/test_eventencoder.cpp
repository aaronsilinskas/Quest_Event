#include <Arduino.h>
#include <unity.h>

#include "Quest_EventEncoder.h"

#define BUFFER_SIZE 48
#define BUFFER_SIZE_IN_BITS BUFFER_SIZE * 8

uint8_t buffer[BUFFER_SIZE];

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
    e.teamID = 2;   // 0b010
    e.playerID = 4; // 0b0100
    e.eventID = 21; // 0b10101
    e.dataLengthInBits = 0;

    bool result = ee.encodeToBuffer(&e);
    TEST_ASSERT_EQUAL(EventEncoded, ee.encodeState);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(12, ee.encodedBitCount);
    TEST_ASSERT_BITS(0b11100000, 0b01000000, buffer[0]); // team ID
    TEST_ASSERT_BITS(0b00011110, 0b00001000, buffer[0]); // player ID
    TEST_ASSERT_BITS(0b00000001, 0b00000001, buffer[0]); // event ID (1 bit)
    TEST_ASSERT_BITS(0b11110000, 0b01010000, buffer[1]); // event ID (4 bits)
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
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(22, ee.encodedBitCount);
    TEST_ASSERT_BITS(0b11100000, 0b00100000, buffer[0]); // team ID
    TEST_ASSERT_BITS(0b00011110, 0b00000100, buffer[0]); // player ID
    TEST_ASSERT_BITS(0b00000001, 0b00000000, buffer[0]); // event ID (1 bit)
    TEST_ASSERT_BITS(0b11110000, 0b00110000, buffer[1]); // event ID (4 bits)
    TEST_ASSERT_BITS(0b00001111, 0b00001010, buffer[1]); // data (4 bits)
    TEST_ASSERT_BITS(0b11111100, 0b10100100, buffer[2]); // data (6 bits)
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
