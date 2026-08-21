#include <boost/test/unit_test.hpp>
#include "interpolation.h"
#include <cmath>

BOOST_AUTO_TEST_SUITE(test_interpolation_suite)

    BOOST_AUTO_TEST_CASE(test_fastinterpolate_exact_knots) {
        int16_t d1 = 1000;
        int16_t d2 = 2000;
        int16_t d3 = 3000;
        int16_t d4 = 4000;

        // At t = 0.0, the interpolation should exactly equal d2
        int16_t val_at_0 = fastinterpolate(d1, d2, d3, d4, 0.0f);
        BOOST_CHECK_EQUAL(val_at_0, d2);

        // At t = 1.0, the interpolation should exactly equal d3
        int16_t val_at_1 = fastinterpolate(d1, d2, d3, d4, 1.0f);
        BOOST_CHECK_EQUAL(val_at_1, d3);
    }

    BOOST_AUTO_TEST_CASE(test_fastinterpolate_non_trivial_curve) {
        int16_t d1 = 12000;
        int16_t d2 = -8000;
        int16_t d3 = 16000;
        int16_t d4 = -4000;

        // At t = 0.0 -> d2
        BOOST_CHECK_EQUAL(fastinterpolate(d1, d2, d3, d4, 0.0f), d2);

        // At t = 1.0 -> d3
        BOOST_CHECK_EQUAL(fastinterpolate(d1, d2, d3, d4, 1.0f), d3);

        // Exact Lagrange at t = 0.5:
        int16_t val_mid = fastinterpolate(d1, d2, d3, d4, 0.5f);
        BOOST_CHECK_EQUAL(val_mid, 4000);
    }

    BOOST_AUTO_TEST_CASE(test_fastinterpolate_clamping) {
        int16_t d1 = -30000;
        int16_t d2 = 32000;
        int16_t d3 = 32000;
        int16_t d4 = -30000;

        // Peak between 0 and 1 could overshoot 32767
        int16_t clamped = fastinterpolate(d1, d2, d3, d4, 0.5f);
        BOOST_CHECK(clamped <= 32767);
        BOOST_CHECK(clamped >= -32768);
    }

BOOST_AUTO_TEST_SUITE_END()
