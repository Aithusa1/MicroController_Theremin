#include <unity.h>
#include "../../src/weergeven_filter.h"

void setUp(void) {}
void tearDown(void) {}

void test_segment_map_hex_waarden(void) {
    TEST_ASSERT_EQUAL_HEX8(0xc0, segment_map[0]);
    TEST_ASSERT_EQUAL_HEX8(0xf9, segment_map[1]);
    TEST_ASSERT_EQUAL_HEX8(0xa4, segment_map[2]);
    TEST_ASSERT_EQUAL_HEX8(0xb0, segment_map[3]);
    TEST_ASSERT_EQUAL_HEX8(0x99, segment_map[4]);
}

void test_display_hex_letters(void) {
    TEST_ASSERT_EQUAL_HEX8(0x88, segment_map[10]);
    TEST_ASSERT_EQUAL_HEX8(0x83, segment_map[11]);
    TEST_ASSERT_EQUAL_HEX8(0xc6, segment_map[12]);
    TEST_ASSERT_EQUAL_HEX8(0xa1, segment_map[13]);
    TEST_ASSERT_EQUAL_HEX8(0x86, segment_map[14]);
    TEST_ASSERT_EQUAL_HEX8(0x8e, segment_map[15]);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_segment_map_hex_waarden);
    RUN_TEST(test_display_hex_letters);
    return UNITY_END();
}