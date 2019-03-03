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
    Quest_EventDispatcher ed = Quest_EventDispatcher(eventQueue, EVENT_QUEUE_SIZE, 0, 0);
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

void test_offering_to_full_queue()
{
    Quest_EventDispatcher ed = Quest_EventDispatcher(eventQueue, EVENT_QUEUE_SIZE, 0, 0);
    // fill the queue
    for (uint8_t i = 0; i < EVENT_QUEUE_SIZE; i++)
    {
        TEST_ASSERT_TRUE(ed.offer(&randomEvents[i]));
    }
    TEST_ASSERT_EQUAL(EVENT_QUEUE_SIZE, ed.eventsWaiting());

    // now try to offer another event
    TEST_ASSERT_FALSE(ed.offer(&randomEvents[0]));
    TEST_ASSERT_EQUAL(EVENT_QUEUE_SIZE, ed.eventsWaiting());
}

void test_polling_when_no_events_are_queued()
{
    Quest_EventDispatcher ed = Quest_EventDispatcher(eventQueue, EVENT_QUEUE_SIZE, 0, 0);
    // offer and poll events so no events are queued
    uint8_t eventsToOffer = random(EVENT_QUEUE_SIZE);
    Event ignoredEvent;
    for (uint8_t i = 0; i < eventsToOffer; i++)
    {
        ed.offer(&randomEvents[i]);
        ed.poll(&ignoredEvent);
    }
    TEST_ASSERT_EQUAL(0, ed.eventsWaiting());
    Event next;
    TEST_ASSERT_FALSE(ed.poll(&next));
    TEST_ASSERT_EQUAL(0, ed.eventsWaiting());
}

void test_circular_queue()
{
    Quest_EventDispatcher ed = Quest_EventDispatcher(eventQueue, EVENT_QUEUE_SIZE, 0, 0);
    // offer and poll more events than the queue size to force a circle back to the start of the queue
    uint8_t eventsToOffer = EVENT_QUEUE_SIZE * 3;
    Event next;
    for (uint8_t i = 0; i < eventsToOffer; i++)
    {
        Event *randomEvent = &randomEvents[i % EVENT_QUEUE_SIZE];
        TEST_ASSERT_TRUE(ed.offer(randomEvent));
        TEST_ASSERT_EQUAL(1, ed.eventsWaiting());

        // peek into the queue to make sure circling is happening
        Event *peek = &eventQueue[i % EVENT_QUEUE_SIZE];
        compareEvents(randomEvent, peek);

        // polling must also circle
        TEST_ASSERT_TRUE(ed.poll(&next));
        TEST_ASSERT_EQUAL(0, ed.eventsWaiting());
        compareEvents(randomEvent, &next);
    }
}

void test_default_team_and_player_without_event_data()
{
    Event randomEvent;
    randomizeEvent(&randomEvent);
    uint8_t defaultTeamID = randomEvent.teamID;
    uint8_t defaultPlayerID = randomEvent.playerID;
    uint8_t eventID = randomEvent.eventID;

    Quest_EventDispatcher ed = Quest_EventDispatcher(eventQueue, EVENT_QUEUE_SIZE, defaultTeamID, defaultPlayerID);
    // offer an event with no player or team ID (useful for subcomponents of larger system)
    TEST_ASSERT_TRUE(ed.offer(eventID));

    Event next;
    TEST_ASSERT_TRUE(ed.poll(&next));
    TEST_ASSERT_EQUAL(defaultTeamID, next.teamID);
    TEST_ASSERT_EQUAL(defaultPlayerID, next.playerID);
    TEST_ASSERT_EQUAL(eventID, next.eventID);
    TEST_ASSERT_EQUAL(0, next.dataLengthInBits);
}

void test_default_team_and_player_with_event_data()
{
    Event randomEvent;
    randomizeEventWithData(&randomEvent);
    uint8_t defaultTeamID = randomEvent.teamID;
    uint8_t defaultPlayerID = randomEvent.playerID;
    uint8_t eventID = randomEvent.eventID;
    uint8_t *data = randomEvent.data;
    uint8_t dataLengthInBits = randomEvent.dataLengthInBits;

    Quest_EventDispatcher ed = Quest_EventDispatcher(eventQueue, EVENT_QUEUE_SIZE, defaultTeamID, defaultPlayerID);
    // offer an event with no player or team ID (useful for subcomponents of larger system)
    TEST_ASSERT_TRUE(ed.offer(eventID, data, dataLengthInBits));

    Event next;
    TEST_ASSERT_TRUE(ed.poll(&next));
    TEST_ASSERT_EQUAL(defaultTeamID, next.teamID);
    TEST_ASSERT_EQUAL(defaultPlayerID, next.playerID);
    TEST_ASSERT_EQUAL(eventID, next.eventID);
    TEST_ASSERT_EQUAL(dataLengthInBits, next.dataLengthInBits);
    TEST_ASSERT_EQUAL_INT8_ARRAY(data, next.data, dataLengthInBits / 8);
}

void setup()
{
    delay(4000);
    setupTests();

    UNITY_BEGIN();

    RUN_TEST(test_offering_and_polling_events);
    RUN_TEST(test_offering_to_full_queue);
    RUN_TEST(test_polling_when_no_events_are_queued);
    RUN_TEST(test_circular_queue);
    RUN_TEST(test_default_team_and_player_without_event_data);
    RUN_TEST(test_default_team_and_player_with_event_data);

    UNITY_END();
}

void loop()
{
}
