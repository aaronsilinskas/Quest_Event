#include <Arduino.h>
#include <unity.h>

#include "Quest_EventDecoder.h"
#include <Quest_BitWriter.h>

#define BUFFER_SIZE 48
#define BUFFER_SIZE_IN_BITS BUFFER_SIZE * 8

uint8_t buffer[BUFFER_SIZE];
Quest_BitWriter rawData = Quest_BitWriter(buffer, BUFFER_SIZE);

uint32_t randomBits(uint8_t bits)
{
    uint64_t maxValue = 1 << bits;
    return random(maxValue);
}

uint16_t setupEncodedEvent(Event *event)
{
    event->teamID = randomBits(QE_TEAM_ID_BITS);
    event->playerID = randomBits(QE_PLAYER_ID_BITS);
    event->eventID = randomBits(QE_EVENT_ID_BITS);
    event->dataLengthInBits = 0;

    rawData.reset();
    rawData.writeBits(event->teamID, QE_TEAM_ID_BITS);
    rawData.writeBits(event->playerID, QE_PLAYER_ID_BITS);
    rawData.writeBits(event->eventID, QE_EVENT_ID_BITS);

    return rawData.bitsWritten();
}

uint16_t setupEncodedEventWithData(Event *event)
{
    setupEncodedEvent(event);
    event->dataLengthInBits = (1 + random(QE_DATA_LENGTH - 1)) * 8;

    for (uint16_t i = 0; i < event->dataLengthInBits / 8; i++)
    {
        event->data[i] = random(256);
    }

    rawData.writeBuffer(event->data, event->dataLengthInBits);

    return rawData.bitsWritten();
}

void verifyDecoding(EventDecodeResult result, Event *expected, Event *actual)
{
    TEST_ASSERT_EQUAL(EventDecoded, result);
    TEST_ASSERT_EQUAL(expected->teamID, actual->teamID);
    TEST_ASSERT_EQUAL(expected->playerID, actual->playerID);
    TEST_ASSERT_EQUAL(expected->eventID, actual->eventID);
    TEST_ASSERT_EQUAL(expected->dataLengthInBits, actual->dataLengthInBits);
    if (expected->dataLengthInBits > 0)
    {
        uint8_t bytesWithData = (expected->dataLengthInBits / 8);
        if (expected->dataLengthInBits % 8 > 0)
        {
            bytesWithData++;
        }
        TEST_ASSERT_EQUAL_INT8_ARRAY(expected->data, actual->data, bytesWithData);
    }
}

void test_decode_event_with_no_data()
{
    Event encodedEvent;
    uint16_t bitsAvailable = setupEncodedEvent(&encodedEvent);

    Quest_EventDecoder ed = Quest_EventDecoder(buffer, BUFFER_SIZE);
    Event decodedEvent;
    EventDecodeResult result = ed.decodeEvent(bitsAvailable, &decodedEvent);

    verifyDecoding(result, &encodedEvent, &decodedEvent);
}

void test_decode_event_with_data()
{
    Event encodedEvent;
    uint16_t bitsAvailable = setupEncodedEventWithData(&encodedEvent);

    Quest_EventDecoder ed = Quest_EventDecoder(buffer, BUFFER_SIZE);
    Event decodedEvent;
    EventDecodeResult result = ed.decodeEvent(bitsAvailable, &decodedEvent);

    verifyDecoding(result, &encodedEvent, &decodedEvent);
}

void test_decode_multiple_times()
{
    Quest_EventDecoder ed = Quest_EventDecoder(buffer, BUFFER_SIZE);
    Event decodedEvent;
    Event encodedEvent;

    uint16_t bitsAvailable = setupEncodedEventWithData(&encodedEvent);
    EventDecodeResult result = ed.decodeEvent(bitsAvailable, &decodedEvent);
    verifyDecoding(result, &encodedEvent, &decodedEvent);

    bitsAvailable = setupEncodedEvent(&encodedEvent);
    result = ed.decodeEvent(bitsAvailable, &decodedEvent);
    verifyDecoding(result, &encodedEvent, &decodedEvent);

    bitsAvailable = setupEncodedEventWithData(&encodedEvent);
    result = ed.decodeEvent(bitsAvailable, &decodedEvent);
    verifyDecoding(result, &encodedEvent, &decodedEvent);
}

void test_not_enough_bits_for_header()
{
    // fill the buffer with less than the required header size
    rawData.reset();
    uint16_t bitsToWrite = random(QE_HEADER_BITS); // will always be less than QE_HEADER_BITS
    for (uint16_t i = 0; i < bitsToWrite; i++)
    {
        rawData.writeBit(random(2));
    }

    Quest_EventDecoder ed = Quest_EventDecoder(buffer, BUFFER_SIZE);
    Event decodedEvent;

    EventDecodeResult result = ed.decodeEvent(rawData.bitsWritten(), &decodedEvent);
    TEST_ASSERT_EQUAL(InvalidHeader, result);
}

void test_too_much_event_data()
{
    Event encodedEvent;
    setupEncodedEvent(&encodedEvent);
    // write more data than will fit in the event data
    for (uint16_t i = 0; i < QE_DATA_LENGTH + 1; i++)
    {
        rawData.writeBits(random(256), 8);
    }

    Quest_EventDecoder ed = Quest_EventDecoder(buffer, BUFFER_SIZE);
    Event decodedEvent;

    EventDecodeResult result = ed.decodeEvent(rawData.bitsWritten(), &decodedEvent);
    TEST_ASSERT_EQUAL(DataSizeExceeded, result);
}

void setup()
{
    delay(4000);

    UNITY_BEGIN();

    RUN_TEST(test_decode_event_with_no_data);
    RUN_TEST(test_decode_event_with_data);
    RUN_TEST(test_decode_multiple_times);
    RUN_TEST(test_not_enough_bits_for_header);
    RUN_TEST(test_too_much_event_data);

    UNITY_END();
}

void loop()
{
}
