#include <unity.h>
#include "newMath.c"


void setUp(void) {  
    
}


void tearDown(void) {
    
}


void test_multiply(void) {
    TEST_ASSERT_EQUAL(20, multiply(5, 4));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_multiply);
    return UNITY_END();
} 

