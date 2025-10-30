#include <unity.h>
#include "../../src/bepaal_volume.h"

volatile uint8_t volume = 128;
volatile uint8_t sound_enabled = 1;

void setUp(void) {}
void tearDown(void) {}

void test_volume_binnen_bereik(void) {
    TEST_ASSERT_TRUE(volume >= 0 && volume <= 255);
}

void test_sound_enabled_bij_volume(void) {
    volume = 50;
    sound_enabled = (volume > 10);
    TEST_ASSERT_TRUE(sound_enabled == 1);
}

void test_sound_disabled_bij_laag_volume(void) {
    volume = 5;
    sound_enabled = (volume > 10);
    TEST_ASSERT_TRUE(sound_enabled == 0);
}

void test_volume_initialisatie(void) {
    TEST_ASSERT_EQUAL(128, volume);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_volume_binnen_bereik);
    RUN_TEST(test_sound_enabled_bij_volume);
    RUN_TEST(test_sound_disabled_bij_laag_volume);
    RUN_TEST(test_volume_initialisatie);
    return UNITY_END();
}