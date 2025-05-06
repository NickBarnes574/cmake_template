// NOLINTNEXTLINE
#define _XOPEN_SOURCE 700 // prevent incomplete class type "struct sigaction"

#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "signal_handler.h"

// ------------- Setup / Teardown -------------

void single_setup(void)
{
    signal_flag = ACTIVE;
}

void single_teardown(void)
{
    // Reset signals to default behavior after each test
    struct sigaction sig_act;
    memset(&sig_act, 0, sizeof(sig_act));
    sig_act.sa_handler = SIG_DFL;

    int signals[] = SIGNAL_LIST;
    for (int i = 0; i < HANDLED_SIGNALS_COUNT; ++i)
    {
        sigaction(signals[i], &sig_act, NULL);
    }

    signal_flag = ACTIVE;
}

// ------------- Tests -------------

void test_signal_action_setup_success(void)
{
    int exit_code = signal_action_setup();
    CU_ASSERT_EQUAL(exit_code, 0);
}

void test_signal_SIGINT_sets_flag(void)
{
    signal_action_setup();
    // NOLINTNEXTLINE
    raise(SIGINT); // triggers our handler

    CU_ASSERT_EQUAL(signal_flag, SHUTDOWN);
}

void test_signal_SIGTERM_sets_flag(void)
{
    signal_action_setup();
    // NOLINTNEXTLINE
    raise(SIGTERM);

    CU_ASSERT_EQUAL(signal_flag, SHUTDOWN);
}

void test_signal_SIGHUP_does_not_shutdown(void)
{
    signal_action_setup();
    // NOLINTNEXTLINE
    raise(SIGHUP);

    CU_ASSERT_EQUAL(signal_flag, ACTIVE);
}

// ------------- Registration -------------

static CU_TestInfo single_signal_tests[] = {
    { "signal_action_setup() returns success",
      test_signal_action_setup_success },
    { "SIGINT sets signal_flag", test_signal_SIGINT_sets_flag },
    { "SIGTERM sets signal_flag", test_signal_SIGTERM_sets_flag },
    { "SIGHUP does not set signal_flag", test_signal_SIGHUP_does_not_shutdown },
    CU_TEST_INFO_NULL
};

CU_SuiteInfo single_signal_test_suite = {
    "Single-threaded Signal Handler Tests",
    NULL,
    NULL,
    single_setup,
    single_teardown,
    single_signal_tests
};
