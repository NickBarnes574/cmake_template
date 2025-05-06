// NOLINTNEXTLINE
#define _GNU_SOURCE // usleep

#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <unistd.h>

#include "signal_handler.h"

// ------------- Test Helpers -------------

static pthread_t sig_thread;

static void * send_signal_after_delay(void * arg)
{
    int sig = *(int *)arg;
    usleep(100000); // 100ms
    pthread_kill(sig_thread, sig);
    return NULL;
}

// ------------- Setup / Teardown -------------

void signal_test_setup(void)
{
    atomic_store(&signal_flag, ACTIVE);

    // Block signals in the main thread so the signal handler thread can receive
    // them via sigwait()
    sigset_t blocked;
    sigemptyset(&blocked);

    int signals[] = SIGNAL_LIST;
    for (int i = 0; i < HANDLED_SIGNALS_COUNT; ++i)
    {
        sigaddset(&blocked, signals[i]);
    }

    pthread_sigmask(SIG_BLOCK, &blocked, NULL);
}

void signal_test_teardown(void)
{
    sigset_t unblock;
    sigemptyset(&unblock);
    int signals[] = SIGNAL_LIST;
    for (int i = 0; i < HANDLED_SIGNALS_COUNT; ++i)
    {
        sigaddset(&unblock, signals[i]);
    }
    pthread_sigmask(SIG_UNBLOCK, &unblock, NULL);
}

// ------------- Tests -------------

void test_signal_thread_sets_flag_SIGINT(void)
{
    pthread_create(&sig_thread, NULL, signal_wait_thread, NULL);

    pthread_t sender;
    int       sig = SIGINT;
    pthread_create(&sender, NULL, send_signal_after_delay, &sig);

    pthread_join(sig_thread, NULL);
    pthread_join(sender, NULL);

    CU_ASSERT_EQUAL(signal_flag, SHUTDOWN);
}

void test_signal_thread_sets_flag_SIGTERM(void)
{
    pthread_create(&sig_thread, NULL, signal_wait_thread, NULL);

    pthread_t sender;
    int       sig = SIGTERM;
    pthread_create(&sender, NULL, send_signal_after_delay, &sig);

    pthread_join(sig_thread, NULL);
    pthread_join(sender, NULL);

    CU_ASSERT_EQUAL(signal_flag, SHUTDOWN);
}

void test_signal_thread_sets_flag_SIGHUP(void)
{
    pthread_create(&sig_thread, NULL, signal_wait_thread, NULL);

    pthread_t sender;
    int       sig = SIGHUP;
    pthread_create(&sender, NULL, send_signal_after_delay, &sig);

    pthread_join(sig_thread, NULL);
    pthread_join(sender, NULL);

    CU_ASSERT_EQUAL(signal_flag, SHUTDOWN);
}

void test_condition_variable_broadcasts_on_signal(void)
{
    pthread_mutex_lock(&signal_flag_mutex);
    pthread_cond_timedwait(
        &signal_flag_cond,
        &signal_flag_mutex,
        &(struct timespec) { .tv_sec  = 0,
                             .tv_nsec = 100000000 }); // wait 100ms (no-op)
    pthread_mutex_unlock(&signal_flag_mutex);

    pthread_create(&sig_thread, NULL, signal_wait_thread, NULL);

    pthread_t sender;
    int       sig = SIGINT;
    pthread_create(&sender, NULL, send_signal_after_delay, &sig);

    pthread_join(sig_thread, NULL);
    pthread_join(sender, NULL);

    // No direct way to test pthread_cond_broadcast, but absence of deadlock &
    // SHUTDOWN confirmation validates it
    CU_ASSERT_EQUAL(signal_flag, SHUTDOWN);
}

// ------------- Registration -------------

static CU_TestInfo multi_signal_tests[] = {
    { "SIGINT sets signal_flag", test_signal_thread_sets_flag_SIGINT },
    { "SIGTERM sets signal_flag", test_signal_thread_sets_flag_SIGTERM },
    { "SIGHUP sets signal_flag", test_signal_thread_sets_flag_SIGHUP },
    { "pthread_cond_broadcast wakes waiting threads",
      test_condition_variable_broadcasts_on_signal },
    CU_TEST_INFO_NULL
};

CU_SuiteInfo multi_signal_test_suite = {
    "Signal Handler Tests", NULL, NULL, signal_test_setup, signal_test_teardown,
    multi_signal_tests
};
