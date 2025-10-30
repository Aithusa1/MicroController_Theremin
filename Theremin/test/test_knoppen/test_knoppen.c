#include <unity.h>
#include "../../src/filter_buttons.h"
#include "../../src/filter_toonhoogte.h"

void setUp(void) {
    filter_init();
}

void tearDown(void) {}

void test_filter_buttons_init_size(void) {
    filter_buttons_init();
    uint8_t size = filter_buttons_get_size();
    TEST_ASSERT_EQUAL(5, size);
}

void test_filter_size_oneven_waarden(void) {
    filter_set_size(3);
    TEST_ASSERT_EQUAL(3, filter_buttons_get_size());
    
    filter_set_size(7);
    TEST_ASSERT_EQUAL(7, filter_buttons_get_size());
}

void test_filter_size_begrenzing(void) {
    filter_set_size(1);
    TEST_ASSERT_EQUAL(1, filter_buttons_get_size());
    
    filter_set_size(15);
    TEST_ASSERT_EQUAL(15, filter_buttons_get_size());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_filter_buttons_init_size);
    RUN_TEST(test_filter_size_oneven_waarden);
    RUN_TEST(test_filter_size_begrenzing);
    return UNITY_END();
}