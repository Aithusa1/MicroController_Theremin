#include <unity.h>
#include "../../src/filter_toonhoogte.h"

uint8_t filter_size = 5;
FilterElement filter_buffer[FILTER_MAX_SIZE];

void setUp(void) {
    filter_init();
}

void tearDown(void) {}

void test_filter_init_waarden(void) {
    TEST_ASSERT_EQUAL(800, filter_buffer[0].value);
    TEST_ASSERT_EQUAL(5, filter_size);
}

void test_filter_add_value(void) {
    filter_add_value(1000);
    uint16_t median = filter_get_median();
    TEST_ASSERT_TRUE(median >= 800 && median <= 1000);
}

void test_filter_mediaan_bij_oneven_grootte(void) {
    filter_set_size(5);
    filter_add_value(100);
    filter_add_value(200);
    filter_add_value(300);
    filter_add_value(400);
    filter_add_value(500);
    TEST_ASSERT_EQUAL(300, filter_get_median());
}

void test_filter_grootte_instelling(void) {
    filter_set_size(8);
    TEST_ASSERT_EQUAL(7, filter_get_size());
    
    filter_set_size(0);
    TEST_ASSERT_EQUAL(1, filter_get_size());
    
    filter_set_size(20);
    TEST_ASSERT_EQUAL(15, filter_get_size());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_filter_init_waarden);
    RUN_TEST(test_filter_add_value);
    RUN_TEST(test_filter_mediaan_bij_oneven_grootte);
    RUN_TEST(test_filter_grootte_instelling);
    return UNITY_END();
}