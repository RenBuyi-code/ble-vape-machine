#include "test_framework.h"

int test_checks_run = 0;
int test_checks_failed = 0;
const char *test_current_name = "(none)";

static int tests_run = 0;
static int tests_failed = 0;

void test_run(const char *name, void (*fn)(void))
{
    int failed_before = test_checks_failed;

    /* Keep the log unbuffered so a crashing test still shows its context. */
    setvbuf(stdout, NULL, _IOLBF, 0);
    test_current_name = name;
    tests_run++;
    fn();
    if (test_checks_failed != failed_before) {
        tests_failed++;
        printf("[FAIL] %s\n", name);
    } else {
        printf("[ ok ] %s\n", name);
    }
    test_current_name = "(none)";
}

int test_summary(const char *suite)
{
    printf("%s: %d tests (%d failed), %d checks (%d failed)\n", suite,
           tests_run, tests_failed, test_checks_run, test_checks_failed);
    return tests_failed == 0 ? 0 : 1;
}
