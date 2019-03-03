#include <Arduino.h>
#include <unity.h>

#include "Quest_EventDispatcher.h"

#define EVENT_QUEUE_SIZE 10
Event eventQueue[EVENT_QUEUE_SIZE];

Event randomEvents[EVENT_QUEUE_SIZE];

void setupTests()
{
    randomSeed(analogRead(0));
    for (uint8_t i = 0; i < EVENT_QUEUE_SIZE; i++)
    {
        randomizeEventWithData(&randomEvents[i]);
    }
}

void compareEvents(Event *event1, Event *event2)
{
    TEST_ASSERT_EQUAL(event1->teamID, event2->teamID);
    TEST_ASSERT_EQUAL(event1->playerID, event2->playerID);
    TEST_ASSERT_EQUAL(event1->eventID, event2->eventID);
    TEST_ASSERT_EQUAL(event1->dataLengthInBits, event2->dataLengthInBits);
    if (event1->dataLengthInBits > 0)
    {
        TEST_ASSERT_EQUAL_INT8_ARRAY(event1->data, event2->data, event1->dataLengthInBits / 8);
    }
}

void test_offering_and_polling_events()
{
    Quest_EventDispatcher ed = Quest_EventDispatcher(eventQueue, EVENT_QUEUE_SIZE);
    uint8_t eventsToOffer = random(EVENT_QUEUE_SIZE);

    // offer events to the dispatcher
    for (uint8_t i = 0; i < eventsToOffer; i++)
    {
        TEST_ASSERT_TRUE(ed.offer(&randomEvents[i]));
        TEST_ASSERT_EQUAL(i + 1, ed.eventsWaiting());
    }

    // poll the events
    Event next;
    for (uint8_t i = 0; i < eventsToOffer; i++)
    {
        TEST_ASSERT_EQUAL(eventsToOffer - i, ed.eventsWaiting());
        TEST_ASSERT_TRUE(ed.poll(&next));
        compareEvents(&randomEvents[i], &next);
    }
    TEST_ASSERT_EQUAL(0, ed.eventsWaiting());
}

// test offering event
// test offering event on full queue
// test pulling event off queue (FIFO)
// test pulling with no events
// test default team & player ID when not set

void setup()
{
    delay(4000);
    setupTests();

    UNITY_BEGIN();

    RUN_TEST(test_offering_and_polling_events);

    UNITY_END();
}

void loop()
{
}