#include <stdio.h>
#include <string.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

int clean_up() {
  printf("Suite is cleaned up.\n");
  return 0;
}

int start_up() {
  printf("Suite is started up.\n");
  return 0;
}

void test_plugin() {
  // Test case here
  char ten[10];
  char twenty[20];
  
  memset(ten, 0, 10);
  memset(twenty, 0, 20);
  
  strcpy(ten, "ten");
  CU_ASSERT_STRING_EQUAL(ten, "ten");
  
  strcpy(twenty, "twenty");
  CU_ASSERT_STRING_EQUAL(twenty, "twenty");
}

int main(void) {
  if (CUE_SUCCESS != CU_initialize_registry()) {
    return CU_get_error();
  }

  CU_pSuite ste = CU_add_suite("plugin_test", start_up, clean_up);
  if (NULL == ste) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (CU_get_error() != CUE_SUCCESS) {
    fprintf(stderr, "Error creating suite: (%d)%s\n", CU_get_error(), CU_get_error_msg());
    return CU_get_error();
  }

  if (!CU_add_test(ste, "Lest plugin()...", test_plugin)) {
    CU_cleanup_registry();
    return CU_get_error();
  }
  
  if (CU_get_error() != CUE_SUCCESS) {
    fprintf(stderr, "Error adding test: (%d)%s\n", CU_get_error(), CU_get_error_msg());
  }

  // run all test
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_ErrorCode run_errors = CU_basic_run_suite(ste);
  if (run_errors != CUE_SUCCESS) {
    fprintf(stderr, "Error running tests: (%d)%s\n", run_errors, CU_get_error_msg());
  }

  CU_basic_show_failures(CU_get_failure_list());
  CU_cleanup_registry();
  return CU_get_error();
}