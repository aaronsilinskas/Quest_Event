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
    for (uint16_t i = 0; i < BUFFER_SIZE; i++)
    {
        TEST_ASSERT_EQUAL(0, buffer[i]);
    }
}

void setup()
{
    delay(4000);

    UNITY_BEGIN();

    RUN_TEST(test_new_instance_is_reset_to_no_encoded_data);

    UNITY_END();
}

void loop()
{
}
