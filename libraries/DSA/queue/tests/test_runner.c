#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

int main(void)
{
    CU_basic_set_mode(CU_BRM_VERBOSE);

    extern CU_SuiteInfo queue_test_suite;

    CU_SuiteInfo suites[] = { queue_test_suite, CU_SUITE_INFO_NULL };

    CU_initialize_registry();

    CU_register_suites(suites);

    CU_basic_run_tests();

    CU_cleanup_registry();
}

/*** end of file ***/
