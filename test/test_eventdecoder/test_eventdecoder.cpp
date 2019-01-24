#include <Arduino.h>
#include <unity.h>

#include "Quest_EventDecoder.h"
#include <Quest_BitWriter.h>

#define BUFFER_SIZE 48
#define BUFFER_SIZE_IN_BITS BUFFER_SIZE * 8

uint8_t buffer[BUFFER_SIZE];

void test_new_instance()
{
    Quest_EventDecoder ed = Quest_EventDecoder(buffer, BUFFER_SIZE);
}

void setup()
{
    delay(4000);

    UNITY_BEGIN();

    RUN_TEST(test_new_instance);

    UNITY_END();
}

void loop()
{
}
