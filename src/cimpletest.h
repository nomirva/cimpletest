#ifndef CIMPLETEST_H
#define CIMPLETEST_H

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "memoria.h"

void cimpletest_test_begin(const char *name);
void cimpletest_test_end(void);
int  cimpletest_run_all(void);
void cimpletest_assert_fail(const char *expr, const char *file, int line);
void cimpletest_expect_fail(const char *expr, const char *file, int line);
int  cimpletest_test_is_active(void);

#define RUN_TESTING \
    void __ct_tests(void); \
    int main(void) { \
        __ct_tests(); \
        return cimpletest_run_all(); \
    } \
    void __ct_tests(void)

#define TEST(name)      for (cimpletest_test_begin(name); cimpletest_test_is_active(); cimpletest_test_end())

#define ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            cimpletest_assert_fail("ASSERT_TRUE(" #expr ")", __FILE__, __LINE__); \
            exit(1); \
        } \
    } while(0)

#define ASSERT_FALSE(expr) \
    do { \
        if ((expr)) { \
            cimpletest_assert_fail("ASSERT_FALSE(" #expr ")", __FILE__, __LINE__); \
            exit(1); \
        } \
    } while(0)

#define ASSERT_FAIL(msg) \
    do { \
        cimpletest_assert_fail("ASSERT_FAIL(" #msg ")", __FILE__, __LINE__); \
        exit(1); \
    } while(0)

#define EXPECT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            cimpletest_expect_fail("EXPECT_TRUE(" #expr ")", __FILE__, __LINE__); \
        } \
    } while(0)

#define EXPECT_FALSE(expr) \
    do { \
        if ((expr)) { \
            cimpletest_expect_fail("EXPECT_FALSE(" #expr ")", __FILE__, __LINE__); \
        } \
    } while(0)

#define EXPECT_FAIL(msg) \
    do { \
        cimpletest_expect_fail("EXPECT_FAIL(" #msg ")", __FILE__, __LINE__); \
    } while(0)

#define ASSERT_NULL(ptr) \
    do { \
        if ((ptr) != NULL) { \
            cimpletest_assert_fail("ASSERT_NULL(" #ptr ")", __FILE__, __LINE__); \
            fprintf(stderr, "  " #ptr " = %p\n", (void*)(ptr)); \
            exit(1); \
        } \
    } while(0)

#define ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            cimpletest_assert_fail("ASSERT_NOT_NULL(" #ptr ")", __FILE__, __LINE__); \
            exit(1); \
        } \
    } while(0)

#define EXPECT_NULL(ptr) \
    do { \
        if ((ptr) != NULL) { \
            cimpletest_expect_fail("EXPECT_NULL(" #ptr ")", __FILE__, __LINE__); \
            fprintf(stderr, "  " #ptr " = %p\n", (void*)(ptr)); \
        } \
    } while(0)

#define EXPECT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            cimpletest_expect_fail("EXPECT_NOT_NULL(" #ptr ")", __FILE__, __LINE__); \
        } \
    } while(0)

#if defined(__clang__)
#define CT_DIAG_PUSH _Pragma("clang diagnostic push") \
                     _Pragma("clang diagnostic ignored \"-Wformat\"") \
                     _Pragma("clang diagnostic ignored \"-Wint-to-pointer-cast\"") \
                     _Pragma("clang diagnostic ignored \"-Wpointer-to-int-cast\"")
#define CT_DIAG_POP  _Pragma("clang diagnostic pop")
#elif defined(__GNUC__)
#define CT_DIAG_PUSH _Pragma("GCC diagnostic push") \
                     _Pragma("GCC diagnostic ignored \"-Wformat\"")
#define CT_DIAG_POP  _Pragma("GCC diagnostic pop")
#else
#define CT_DIAG_PUSH
#define CT_DIAG_POP
#endif

#if !defined(_MSC_VER)
#define CT_CHAR_TYPEOF_   char: "char",
#define CT_CHAR_PRINT_(n, v)  char: (void)fprintf(stderr, "  %s = %d\n", (n), (int)(v)),
#else
#define CT_CHAR_TYPEOF_
#define CT_CHAR_PRINT_(n, v)
#endif

#define CT_TYPEOF(val) \
    _Generic((val), \
        _Bool:              "_Bool", \
        CT_CHAR_TYPEOF_ \
        signed char:        "signed char", \
        unsigned char:      "unsigned char", \
        short:              "short", \
        unsigned short:     "unsigned short", \
        int:                "int", \
        unsigned:           "unsigned int", \
        long:               "long", \
        unsigned long:      "unsigned long", \
        long long:          "long long", \
        unsigned long long: "unsigned long long", \
        float:              "float", \
        double:             "double", \
        long double:        "long double", \
        const char*:        "const char*", \
        char*:              "char*", \
        const void*:        "const void*", \
        void*:              "void*", \
        default:            "?" \
    )

#define cimpletest_print_val(name, val) \
    do { \
        CT_DIAG_PUSH \
        _Generic((val), \
            _Bool:              (void)fprintf(stderr, "  %s = %d\n",              name, (int)(val)), \
            CT_CHAR_PRINT_(name, val) \
            signed char:        (void)fprintf(stderr, "  %s = %d\n",              name, (int)(val)), \
            unsigned char:      (void)fprintf(stderr, "  %s = %u\n",              name, (unsigned)(val)), \
            short:              (void)fprintf(stderr, "  %s = %d\n",              name, (int)(val)), \
            unsigned short:     (void)fprintf(stderr, "  %s = %u\n",              name, (unsigned)(val)), \
            int:                (void)fprintf(stderr, "  %s = %d\n",              name, val), \
            unsigned:           (void)fprintf(stderr, "  %s = %u\n",              name, val), \
            long:               (void)fprintf(stderr, "  %s = %ld\n",             name, val), \
            unsigned long:      (void)fprintf(stderr, "  %s = %lu\n",             name, val), \
            long long:          (void)fprintf(stderr, "  %s = %lld\n",            name, val), \
            unsigned long long: (void)fprintf(stderr, "  %s = %llu\n",            name, val), \
            float:              (void)fprintf(stderr, "  %s = %g\n",              name, (double)(val)), \
            double:             (void)fprintf(stderr, "  %s = %g\n",              name, val), \
            long double:        (void)fprintf(stderr, "  %s = %Lg\n",             name, val), \
            const char*:        (void)fprintf(stderr, "  %s = \"%s\"\n",          name, val), \
            char*:              (void)fprintf(stderr, "  %s = \"%s\"\n",          name, val), \
            const void*:        (void)fprintf(stderr, "  %s = %p\n",              name, val), \
            void*:              (void)fprintf(stderr, "  %s = %p\n",              name, val), \
            default:            (void)fprintf(stderr, "  %s = ?\n", name) \
        ); \
        CT_DIAG_POP \
    } while(0)

#define CT_ASSERT_OP(op_name, op, a, b) \
    do { \
        if (!((a) op (b))) { \
            cimpletest_assert_fail("ASSERT_" #op_name "(" #a ", " #b ")", __FILE__, __LINE__); \
            cimpletest_print_val(#a, (a)); \
            cimpletest_print_val(#b, (b)); \
            exit(1); \
        } \
    } while(0)

#define CT_EXPECT_OP(op_name, op, a, b) \
    do { \
        if (!((a) op (b))) { \
            cimpletest_expect_fail("EXPECT_" #op_name "(" #a ", " #b ")", __FILE__, __LINE__); \
            cimpletest_print_val(#a, (a)); \
            cimpletest_print_val(#b, (b)); \
        } \
    } while(0)

#define CT_ASSERT_STR_OP(op_name, op, s1, s2) \
    do { \
        if (strcmp((s1), (s2)) op 0) { \
            cimpletest_assert_fail("ASSERT_" #op_name "(" #s1 ", " #s2 ")", __FILE__, __LINE__); \
            fprintf(stderr, "  " #s1 " = \"%s\"\n", (s1)); \
            fprintf(stderr, "  " #s2 " = \"%s\"\n", (s2)); \
            exit(1); \
        } \
    } while(0)

#define CT_EXPECT_STR_OP(op_name, op, s1, s2) \
    do { \
        if (strcmp((s1), (s2)) op 0) { \
            cimpletest_expect_fail("EXPECT_" #op_name "(" #s1 ", " #s2 ")", __FILE__, __LINE__); \
            fprintf(stderr, "  " #s1 " = \"%s\"\n", (s1)); \
            fprintf(stderr, "  " #s2 " = \"%s\"\n", (s2)); \
        } \
    } while(0)

#define CT_ASSERT_STRN_OP(op_name, op, s1, s2, n) \
    do { \
        if (strncmp((s1), (s2), (n)) op 0) { \
            cimpletest_assert_fail("ASSERT_" #op_name "(" #s1 ", " #s2 ", " #n ")", __FILE__, __LINE__); \
            fprintf(stderr, "  " #s1 " = \"%s\"\n", (s1)); \
            fprintf(stderr, "  " #s2 " = \"%s\"\n", (s2)); \
            exit(1); \
        } \
    } while(0)

#define CT_EXPECT_STRN_OP(op_name, op, s1, s2, n) \
    do { \
        if (strncmp((s1), (s2), (n)) op 0) { \
            cimpletest_expect_fail("EXPECT_" #op_name "(" #s1 ", " #s2 ", " #n ")", __FILE__, __LINE__); \
            fprintf(stderr, "  " #s1 " = \"%s\"\n", (s1)); \
            fprintf(stderr, "  " #s2 " = \"%s\"\n", (s2)); \
        } \
    } while(0)

#define ASSERT_EQ(a, b)        CT_ASSERT_OP(EQ, ==, a, b)
#define ASSERT_NE(a, b)        CT_ASSERT_OP(NE, !=, a, b)
#define ASSERT_LT(a, b)        CT_ASSERT_OP(LT, <, a, b)
#define ASSERT_LE(a, b)        CT_ASSERT_OP(LE, <=, a, b)
#define ASSERT_GT(a, b)        CT_ASSERT_OP(GT, >, a, b)
#define ASSERT_GE(a, b)        CT_ASSERT_OP(GE, >=, a, b)

#define EXPECT_EQ(a, b)        CT_EXPECT_OP(EQ, ==, a, b)
#define EXPECT_NE(a, b)        CT_EXPECT_OP(NE, !=, a, b)
#define EXPECT_LT(a, b)        CT_EXPECT_OP(LT, <, a, b)
#define EXPECT_LE(a, b)        CT_EXPECT_OP(LE, <=, a, b)
#define EXPECT_GT(a, b)        CT_EXPECT_OP(GT, >, a, b)
#define EXPECT_GE(a, b)        CT_EXPECT_OP(GE, >=, a, b)

#define ASSERT_STREQ(s1, s2)   CT_ASSERT_STR_OP(STREQ, !=, s1, s2)
#define ASSERT_STRNE(s1, s2)   CT_ASSERT_STR_OP(STRNE, ==, s1, s2)
#define ASSERT_STREQN(s1, s2, n)  CT_ASSERT_STRN_OP(STREQN, !=, s1, s2, n)

#define EXPECT_STREQ(s1, s2)   CT_EXPECT_STR_OP(STREQ, !=, s1, s2)
#define EXPECT_STRNE(s1, s2)   CT_EXPECT_STR_OP(STRNE, ==, s1, s2)

#define ASSERT_NEAR(a, b, eps) \
    do { \
        double _ct_ndiff_ = (double)(a) - (double)(b); \
        if (_ct_ndiff_ < 0) _ct_ndiff_ = -_ct_ndiff_; \
        if (_ct_ndiff_ > (eps)) { \
            cimpletest_assert_fail("ASSERT_NEAR(" #a ", " #b ", " #eps ")", __FILE__, __LINE__); \
            fprintf(stderr, "  " #a " = %g\n", (double)(a)); \
            fprintf(stderr, "  " #b " = %g\n", (double)(b)); \
            fprintf(stderr, "  diff = %g, eps = %g\n", _ct_ndiff_, (double)(eps)); \
            exit(1); \
        } \
    } while(0)

#define EXPECT_NEAR(a, b, eps) \
    do { \
        double _ct_ndiff_ = (double)(a) - (double)(b); \
        if (_ct_ndiff_ < 0) _ct_ndiff_ = -_ct_ndiff_; \
        if (_ct_ndiff_ > (eps)) { \
            cimpletest_expect_fail("EXPECT_NEAR(" #a ", " #b ", " #eps ")", __FILE__, __LINE__); \
            fprintf(stderr, "  " #a " = %g\n", (double)(a)); \
            fprintf(stderr, "  " #b " = %g\n", (double)(b)); \
            fprintf(stderr, "  diff = %g, eps = %g\n", _ct_ndiff_, (double)(eps)); \
        } \
    } while(0)

#define ASSERT_MEM_EQ(p1, p2, n) \
    do { \
        if (memcmp((p1), (p2), (n)) != 0) { \
            cimpletest_assert_fail("ASSERT_MEM_EQ(" #p1 ", " #p2 ", " #n ")", __FILE__, __LINE__); \
            fprintf(stderr, "  " #p1 " and " #p2 " differ\n"); \
            exit(1); \
        } \
    } while(0)

#define ASSERT_MEM_NE(p1, p2, n) \
    do { \
        if (memcmp((p1), (p2), (n)) == 0) { \
            cimpletest_assert_fail("ASSERT_MEM_NE(" #p1 ", " #p2 ", " #n ")", __FILE__, __LINE__); \
            exit(1); \
        } \
    } while(0)

#define EXPECT_MEM_EQ(p1, p2, n) \
    do { \
        if (memcmp((p1), (p2), (n)) != 0) { \
            cimpletest_expect_fail("EXPECT_MEM_EQ(" #p1 ", " #p2 ", " #n ")", __FILE__, __LINE__); \
            fprintf(stderr, "  " #p1 " and " #p2 " differ\n"); \
        } \
    } while(0)

#define EXPECT_MEM_NE(p1, p2, n) \
    do { \
        if (memcmp((p1), (p2), (n)) == 0) { \
            cimpletest_expect_fail("EXPECT_MEM_NE(" #p1 ", " #p2 ", " #n ")", __FILE__, __LINE__); \
        } \
    } while(0)

#ifdef CIMPLETEST_IMPLEMENTATION

static struct {
    const char *current_name;
    int  current_failed;
    int  total;
    int  passed;
    int  failed;
} ct_state = {0};

void cimpletest_test_begin(const char *name) {
    if (ct_state.current_name)
        cimpletest_test_end();
    fflush(stderr);
    ct_state.current_name = name;
    ct_state.current_failed = 0;
    printf("\n=== %s ===\n", name);
    fflush(stdout);
    memoria_stats_reset();
}

void cimpletest_test_end(void) {
    fflush(stderr);
    ct_state.total++;
    if (ct_state.current_failed) {
        ct_state.failed++;
        printf("  FAILED\n\n");
    } else {
        ct_state.passed++;
        printf("  PASSED\n\n");
    }
    memoria_print_report();
    ct_state.current_name = NULL;
}

int cimpletest_run_all(void) {
    fflush(stderr);
    if (ct_state.current_name)
        cimpletest_test_end();
    printf("\n=== Results: %d/%d passed (%d failed) ===\n",
           ct_state.passed, ct_state.total, ct_state.failed);
    fflush(stdout);
    return ct_state.failed > 0;
}

int cimpletest_test_is_active(void) {
    return ct_state.current_name != NULL;
}

void cimpletest_assert_fail(const char *expr, const char *file, int line) {
    fflush(stdout);
    fprintf(stderr, "  FAIL: %s:%d: %s\n", file, line, expr);
}

void cimpletest_expect_fail(const char *expr, const char *file, int line) {
    fflush(stdout);
    fprintf(stderr, "  FAIL: %s:%d: %s\n", file, line, expr);
    ct_state.current_failed = 1;
}

#endif /* CIMPLETEST_IMPLEMENTATION */
#endif /* CIMPLETEST_H */
