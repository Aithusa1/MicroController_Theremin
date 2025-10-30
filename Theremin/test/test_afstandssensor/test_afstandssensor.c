#include <unity.h>
#include "../../src/bepaal_toonhoogte.h"

void setUp(void) {}
void tearDown(void) {}

void test_dist_to_freq_minimum_afstand(void) {
    TEST_ASSERT_EQUAL(1400, dist_to_freq(1));
}

void test_dist_to_freq_maximum_afstand(void) {
    TEST_ASSERT_EQUAL(230, dist_to_freq(65));
}

void test_dist_to_freq_buiten_bereik(void) {
    TEST_ASSERT_EQUAL(230, dist_to_freq(0));
    TEST_ASSERT_EQUAL(230, dist_to_freq(100));
}

void test_dist_to_freq_midden_afstand(void) {
    uint16_t freq = dist_to_freq(30);
    TEST_ASSERT_TRUE(freq > 230 && freq < 1400);
}

void test_dist_to_freq_lineaire_afname(void) {
    uint16_t freq1 = dist_to_freq(10);
    uint16_t freq2 = dist_to_freq(20);
    TEST_ASSERT_TRUE(freq1 > freq2);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_dist_to_freq_minimum_afstand);
    RUN_TEST(test_dist_to_freq_maximum_afstand);
    RUN_TEST(test_dist_to_freq_buiten_bereik);
    RUN_TEST(test_dist_to_freq_midden_afstand);
    RUN_TEST(test_dist_to_freq_lineaire_afname);
    return UNITY_END();
}