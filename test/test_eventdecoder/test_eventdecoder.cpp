#include <Arduino.h>
#include <unity.h>

#include "Quest_EventDecoder.h"

#define BUFFER_SIZE 48
#define BUFFER_SIZE_IN_BITS BUFFER_SIZE * 8

uint8_t buffer[BUFFER_SIZE];

void test_new_instance()
{
    Event event;
    decodeEvent(buffer, BUFFER_SIZE, &event);
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
