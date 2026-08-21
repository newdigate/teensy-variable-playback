#include <boost/test/unit_test.hpp>
#include "AudioEventResponder.h"

using namespace newdigate;

static int callback1_count = 0;
static int callback2_count = 0;
static int callback3_count = 0;

static void test_callback_1(EventResponderRef r) { callback1_count++; }
static void test_callback_2(EventResponderRef r) { callback2_count++; }
static void test_callback_3(EventResponderRef r) { callback3_count++; }

BOOST_AUTO_TEST_SUITE(test_audio_event_responder)

    BOOST_AUTO_TEST_CASE(test_linked_list_removal_order) {
        callback1_count = 0;
        callback2_count = 0;
        callback3_count = 0;

        AudioEventResponder resp1;
        AudioEventResponder resp2;
        AudioEventResponder resp3;

        resp1.attachPolled(test_callback_1);
        resp2.attachPolled(test_callback_2);
        resp3.attachPolled(test_callback_3);

        // Trigger all three: list is resp1 -> resp2 -> resp3
        resp1.triggerEvent(1);
        resp2.triggerEvent(2);
        resp3.triggerEvent(3);

        // Detach resp2 (middle item)
        resp2.detach();

        // Run polled events: resp1 should run, then resp3 should run
        int result1 = AudioEventResponder::runPolled(); // runs resp1
        BOOST_CHECK_EQUAL(callback1_count, 1);
        BOOST_CHECK_EQUAL(callback2_count, 0);

        int result2 = AudioEventResponder::runPolled(); // runs resp3
        BOOST_CHECK_EQUAL(callback3_count, 1);

        // No more pending events
        int result3 = AudioEventResponder::runPolled();
        BOOST_CHECK_EQUAL(result3, -1);
    }

BOOST_AUTO_TEST_SUITE_END()
