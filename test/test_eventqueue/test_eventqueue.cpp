#include <Arduino.h>
#include <unity.h>

#include "Quest_EventQueue.h"

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

void compareEventWithValues(Event *event, uint8_t teamID, uint8_t playerID, uint8_t eventID, uint8_t *data, uint8_t dataLengthInBits)
{
    TEST_ASSERT_EQUAL(teamID, event->teamID);
    TEST_ASSERT_EQUAL(playerID, event->playerID);
    TEST_ASSERT_EQUAL(eventID, event->eventID);
    TEST_ASSERT_EQUAL(dataLengthInBits, event->dataLengthInBits);
    if (dataLengthInBits > 0)
    {
        TEST_ASSERT_EQUAL_INT8_ARRAY(data, event->data, event->dataLengthInBits / 8);
    }
}

void test_offering_and_polling_events()
{
    Quest_EventQueue eq = Quest_EventQueue(eventQueue, EVENT_QUEUE_SIZE, 0, 0);
    uint8_t eventsToOffer = random(EVENT_QUEUE_SIZE);

    // offer events to the dispatcher
    for (uint8_t i = 0; i < eventsToOffer; i++)
    {
        TEST_ASSERT_TRUE(eq.offer(&randomEvents[i]));
        TEST_ASSERT_EQUAL(i + 1, eq.eventsWaiting());
    }

    // poll the events
    Event next;
    for (uint8_t i = 0; i < eventsToOffer; i++)
    {
        TEST_ASSERT_EQUAL(eventsToOffer - i, eq.eventsWaiting());
        TEST_ASSERT_TRUE(eq.poll(&next));
        compareEvents(&randomEvents[i], &next);
    }
    TEST_ASSERT_EQUAL(0, eq.eventsWaiting());
}

void test_offering_to_full_queue()
{
    Quest_EventQueue eq = Quest_EventQueue(eventQueue, EVENT_QUEUE_SIZE, 0, 0);
    // fill the queue
    for (uint8_t i = 0; i < EVENT_QUEUE_SIZE; i++)
    {
        TEST_ASSERT_TRUE(eq.offer(&randomEvents[i]));
    }
    TEST_ASSERT_EQUAL(EVENT_QUEUE_SIZE, eq.eventsWaiting());

    // now try to offer more events when queue is full
    Event *randomEvent = &randomEvent[0];
    TEST_ASSERT_FALSE(eq.offer(randomEvent));
    TEST_ASSERT_FALSE(eq.offer(randomEvent->eventID, randomEvent->data, randomEvent->dataLengthInBits));
    TEST_ASSERT_FALSE(eq.offer(randomEvent->eventID));
    TEST_ASSERT_EQUAL(EVENT_QUEUE_SIZE, eq.eventsWaiting());
}

void test_polling_when_no_events_are_queued()
{
    Quest_EventQueue eq = Quest_EventQueue(eventQueue, EVENT_QUEUE_SIZE, 0, 0);
    // offer and poll events so no events are queued
    uint8_t eventsToOffer = random(EVENT_QUEUE_SIZE);
    Event ignoredEvent;
    for (uint8_t i = 0; i < eventsToOffer; i++)
    {
        eq.offer(&randomEvents[i]);
        eq.poll(&ignoredEvent);
    }
    TEST_ASSERT_EQUAL(0, eq.eventsWaiting());
    Event next;
    TEST_ASSERT_FALSE(eq.poll(&next));
    TEST_ASSERT_EQUAL(0, eq.eventsWaiting());
}

void test_circular_queue()
{
    Quest_EventQueue eq = Quest_EventQueue(eventQueue, EVENT_QUEUE_SIZE, 0, 0);
    // offer and poll more events than the queue size to force a circle back to the start of the queue
    uint8_t eventsToOffer = EVENT_QUEUE_SIZE * 3;
    Event next;
    for (uint8_t i = 0; i < eventsToOffer; i++)
    {
        Event *randomEvent = &randomEvents[i % EVENT_QUEUE_SIZE];
        TEST_ASSERT_TRUE(eq.offer(randomEvent));
        TEST_ASSERT_EQUAL(1, eq.eventsWaiting());

        // peek into the queue to make sure circling is happening
        Event *peek = &eventQueue[i % EVENT_QUEUE_SIZE];
        compareEvents(randomEvent, peek);

        // polling must also circle
        TEST_ASSERT_TRUE(eq.poll(&next));
        TEST_ASSERT_EQUAL(0, eq.eventsWaiting());
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

    Quest_EventQueue eq = Quest_EventQueue(eventQueue, EVENT_QUEUE_SIZE, defaultTeamID, defaultPlayerID);
    // offer an event with no player or team ID (useful for subcomponents of larger system)
    TEST_ASSERT_TRUE(eq.offer(eventID));

    Event next;
    TEST_ASSERT_TRUE(eq.poll(&next));
    compareEventWithValues(&next, defaultTeamID, defaultPlayerID, eventID, NULL, 0);
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

    Quest_EventQueue eq = Quest_EventQueue(eventQueue, EVENT_QUEUE_SIZE, defaultTeamID, defaultPlayerID);
    // offer an event with no player or team ID (useful for subcomponents of larger system)
    TEST_ASSERT_TRUE(eq.offer(eventID, data, dataLengthInBits));

    Event next;
    TEST_ASSERT_TRUE(eq.poll(&next));
    compareEventWithValues(&next, defaultTeamID, defaultPlayerID, eventID, data, dataLengthInBits);
}

void test_circular_queue_with_default_team_and_player()
{
    Event randomEvent;
    randomizeEvent(&randomEvent);
    uint8_t defaultTeamID = randomEvent.teamID;
    uint8_t defaultPlayerID = randomEvent.playerID;

    Quest_EventQueue eq = Quest_EventQueue(eventQueue, EVENT_QUEUE_SIZE, defaultTeamID, defaultPlayerID);
    // offer and poll more events than the queue size to force a circle back to the start of the queue
    uint8_t eventsToOffer = EVENT_QUEUE_SIZE * 3;
    Event next;
    for (uint8_t i = 0; i < eventsToOffer; i++)
    {
        Event *randomEvent = &randomEvents[i % EVENT_QUEUE_SIZE];
        TEST_ASSERT_TRUE(eq.offer(randomEvent->eventID, randomEvent->data, randomEvent->dataLengthInBits));
        TEST_ASSERT_EQUAL(1, eq.eventsWaiting());

        // peek into the queue to make sure circling is happening
        Event *peek = &eventQueue[i % EVENT_QUEUE_SIZE];
        compareEventWithValues(peek, defaultTeamID, defaultPlayerID, randomEvent->eventID, randomEvent->data, randomEvent->dataLengthInBits);

        // polling must also circle
        TEST_ASSERT_TRUE(eq.poll(&next));
        TEST_ASSERT_EQUAL(0, eq.eventsWaiting());
        compareEventWithValues(&next, defaultTeamID, defaultPlayerID, randomEvent->eventID, randomEvent->data, randomEvent->dataLengthInBits);
    }
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
    RUN_TEST(test_circular_queue_with_default_team_and_player);

    UNITY_END();
}

void loop()
{
}
