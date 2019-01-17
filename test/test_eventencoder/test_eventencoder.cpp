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

void test_encode_event()
{
    Quest_EventEncoder ee = Quest_EventEncoder(buffer, BUFFER_SIZE);
    Event e;
    e.teamID = 1;
    e.playerID = 2;
    e.eventID = 3;
    e.data[0] = 0b10101010;
    e.data[1] = 0b01000000;
    e.dataLengthInBits = 10;

    bool result = ee.encodeToBuffer(&e);
    TEST_ASSERT_EQUAL(EventEncoded, ee.encodeState);
    TEST_ASSERT_EQUAL(true, result);
    TEST_ASSERT_EQUAL(0b00100000, buffer[0] & 0b11100000); // team ID
    TEST_ASSERT_EQUAL(0b00000100, buffer[0] & 0b00011110); // player ID
    TEST_ASSERT_EQUAL(0b00000000, buffer[0] & 0b00000001); // event ID (1 bit)
    TEST_ASSERT_EQUAL(0b00110000, buffer[1] & 0b11110000); // event ID (4 bits)
    TEST_ASSERT_EQUAL(0b00001010, buffer[1] & 0b00001111); // data (4 bits)
    TEST_ASSERT_EQUAL(0b10100100, buffer[2] & 0b11111111); // data (6 bits)
}

void setup()
{
    delay(4000);

    UNITY_BEGIN();

    RUN_TEST(test_new_instance_is_reset_to_no_encoded_data);
    RUN_TEST(test_encode_event);

    UNITY_END();
}

void loop()
{
}
