#define MEMORIA_IMPLEMENTATION
#define CIMPLETEST_IMPLEMENTATION
#include "../src/cimpletest.h"

RUN_TESTING {

    TEST("malloc and free") {
        char *buffer = malloc(100);
        ASSERT_NOT_NULL(buffer);
        int *numbers = calloc(5, sizeof(int));
        ASSERT_NOT_NULL(numbers);
        for (int i = 0; i < 5; i++) numbers[i] = i * 10;
        int *bigger = realloc(numbers, 10 * sizeof(int));
        if (bigger) numbers = bigger;
        ASSERT_EQ(numbers[4], 40);
        free(buffer);
        free(numbers);
    }

    TEST("double free detection") {
        char *p = malloc(64);
        ASSERT_NOT_NULL(p);
        free(p);
        EXPECT_TRUE(memoria_stats_active_count() == 0);
        free(p);
        EXPECT_GT(memoria_stats_double_free_count(), 0);
    }

    TEST("leak detection") {
        malloc(256);
        EXPECT_TRUE(memoria_stats_has_leaks());
    }

    TEST("assert and expect") {
        int x = 42;
        ASSERT_EQ(x, 42);
        ASSERT_NE(x, 0);
        ASSERT_LT(x, 100);
        ASSERT_LE(x, 42);
        ASSERT_GT(x, 10);
        ASSERT_GE(x, 42);
        EXPECT_EQ(x, 42);
        EXPECT_NE(x, 0);
    }

    TEST("strings") {
        const char *a = "hello";
        const char *b = "hello";
        const char *c = "world";
        ASSERT_STREQ(a, b);
        ASSERT_STRNE(a, c);
        ASSERT_STREQN(a, b, 3);
        EXPECT_STREQ(a, b);
    }

    TEST("float") {
        double a = 3.14159;
        double b = 3.14160;
        ASSERT_NEAR(a, b, 0.001);
        EXPECT_NEAR(a, b, 0.001);
    }

    TEST("memory compare") {
        int a[] = {1, 2, 3, 4, 5};
        int b[] = {1, 2, 3, 4, 5};
        ASSERT_MEM_EQ(a, b, sizeof(a));
        EXPECT_MEM_EQ(a, b, sizeof(a));
    }

    TEST("null and bool") {
        void *p = NULL;
        int *q = malloc(10);
        ASSERT_NULL(p);
        ASSERT_NOT_NULL(q);
        EXPECT_NULL(p);
        EXPECT_NOT_NULL(q);
        ASSERT_TRUE(q != NULL);
        ASSERT_FALSE(p != NULL);
        free(q);
    }

    TEST("expect does not stop test") {
        int x = 42;
        EXPECT_EQ(x, 42);
        EXPECT_TRUE(x == 42);
    }
}
