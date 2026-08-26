#ifndef _TEST_FRAMEWORK_H_
#define _TEST_FRAMEWORK_H_

#include <stdio.h>
#include <string.h>

extern int test_checks_run;
extern int test_checks_failed;
extern const char *test_current_name;

void test_run(const char *name, void (*fn)(void));
int test_summary(const char *suite);

#define CHECK(cond)                                                            \
    do {                                                                       \
        test_checks_run++;                                                     \
        if (!(cond)) {                                                         \
            test_checks_failed++;                                              \
            printf("  FAIL %s:%d in %s: %s\n", __FILE__, __LINE__,             \
                   test_current_name, #cond);                                  \
        }                                                                      \
    } while (0)

#define CHECK_EQ_INT(actual, expected)                                         \
    do {                                                                       \
        long _a = (long)(actual);                                              \
        long _e = (long)(expected);                                            \
        test_checks_run++;                                                     \
        if (_a != _e) {                                                        \
            test_checks_failed++;                                              \
            printf("  FAIL %s:%d in %s: %s == %ld, expected %ld\n", __FILE__,  \
                   __LINE__, test_current_name, #actual, _a, _e);              \
        }                                                                      \
    } while (0)

#define CHECK_EQ_STR(actual, expected)                                         \
    do {                                                                       \
        const char *_a = (const char *)(actual);                               \
        const char *_e = (const char *)(expected);                             \
        test_checks_run++;                                                     \
        if (_a == NULL || strcmp(_a, _e) != 0) {                               \
            test_checks_failed++;                                              \
            printf("  FAIL %s:%d in %s: %s == \"%s\", expected \"%s\"\n",      \
                   __FILE__, __LINE__, test_current_name, #actual,             \
                   _a ? _a : "(null)", _e);                                    \
        }                                                                      \
    } while (0)

#define CHECK_EQ_MEM(actual, expected, len)                                    \
    do {                                                                       \
        test_checks_run++;                                                     \
        if (memcmp((actual), (expected), (len)) != 0) {                        \
            test_checks_failed++;                                              \
            printf("  FAIL %s:%d in %s: %s differs from %s\n", __FILE__,       \
                   __LINE__, test_current_name, #actual, #expected);           \
        }                                                                      \
    } while (0)

#define RUN_TEST(fn) test_run(#fn, fn)

#endif
