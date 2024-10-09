#!/usr/bin/env python3

import re
import os
import argparse


class TestGenerator:
    def __init__(self, header_path):
        self.header_path = header_path
        self.module_name = os.path.splitext(os.path.basename(header_path))[0]
        self.functions = self.parse_header()

    def parse_header(self):
        with open(self.header_path, "r") as file:
            content = file.read()
        pattern = re.compile(r"(?<!typedef\s)([\w\*]+\s+)+\**(\w+)\s*\(([^;]*?)\)\s*;")
        matches = pattern.findall(content)
        functions = []
        for return_type, func, params in matches:
            if "(" in params or "typedef" in return_type or "*" in func:
                continue
            param_list = [param.strip() for param in params.split(",")]
            pointer_params = [p for p in param_list if "*" in p]
            functions.append((func, pointer_params))
        return functions

    def generate_tests(self):
        test_functions = []
        test_array_entries = []
        for func, pointer_params in self.functions:
            null_tests, null_entries = self.generate_null_tests(func, pointer_params)
            test_functions.extend(null_tests)
            test_array_entries.extend(null_entries)
            success_test_name = f"test_{func}_success"
            test_functions.append(
                f"void {success_test_name}(void)\n{{\n    // TODO: Add test logic here\n}}\n"
            )
            test_array_entries.append(
                f'    {{"{success_test_name}", {success_test_name}}},\n'
            )
        return self.format_test_suite(test_functions, test_array_entries)

    def generate_null_tests(self, func, pointer_params):
        null_tests = []
        null_entries = []
        for param in pointer_params:
            param_name = param.split()[-1].strip("*").strip()
            null_test_name = f"test_{func}_null_{param_name}"
            null_tests.append(
                f"void {null_test_name}(void)\n{{\n    // TODO: Add test logic here\n}}\n"
            )
            null_entries.append(f'    {{"{null_test_name}", {null_test_name}}},\n')
        return null_tests, null_entries

    def format_test_suite(self, test_functions, test_array_entries):
        test_array = (
            f"static CU_TestInfo {self.module_name}_tests[] = {{\n"
            + "".join(test_array_entries)
            + "    CU_TEST_INFO_NULL\n};\n"
        )
        suite_info = (
            f"CU_SuiteInfo {self.module_name}_test_suite = {{\n"
            f'    "{self.module_name} Tests",\n'
            "    NULL,       // Suite initialization function\n"
            "    NULL,       // Suite cleanup function\n"
            "    NULL,       // Suite setup function\n"
            "    NULL,       // Suite teardown function\n"
            f"    {self.module_name}_tests // The combined array of all tests\n"
            "};\n\n/*** end of file ***/\n"
        )
        return "\n".join(test_functions) + "\n" + test_array + "\n" + suite_info

    def generate_test_runner(self, output_dir):
        runner_code = (
            "#include <CUnit/Basic.h>\n"
            "#include <CUnit/CUnit.h>\n\n"
            "int main(void)\n"
            "{\n"
            "    CU_basic_set_mode(CU_BRM_VERBOSE);\n\n"
            f"    extern CU_SuiteInfo {self.module_name}_test_suite;\n\n"
            f"    CU_SuiteInfo suites[] = {{ {self.module_name}_test_suite, CU_SUITE_INFO_NULL }};\n\n"
            "    CU_initialize_registry();\n"
            "    CU_register_suites(suites);\n"
            "    CU_basic_run_tests();\n"
            "    CU_cleanup_registry();\n"
            "}\n\n/*** end of file ***/\n"
        )
        output_file = os.path.join(output_dir, f"test_runner.c")
        with open(output_file, "w") as f:
            f.write(runner_code)
        print(f"Test runner file created: {output_file}")


def main(header_path, output_dir):
    generator = TestGenerator(header_path)
    boilerplate_code = generator.generate_tests()
    header_file_name = os.path.basename(header_path)
    test_file = os.path.join(output_dir, generator.module_name + "_tests.c")
    with open(test_file, "w") as f:
        f.write("#include <CUnit/Basic.h>\n")
        f.write("#include <CUnit/CUnit.h>\n")
        f.write("#include <stdio.h>\n")
        f.write("#include <stdlib.h>\n\n")
        f.write(f'#include "{header_file_name}"\n\n')
        f.write(boilerplate_code)
    print(f"Boilerplate test file created: {test_file}")
    generator.generate_test_runner(output_dir)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Generate CUnit test boilerplate for a C header file."
    )
    parser.add_argument("input_file", help="Path to the C header file.")
    parser.add_argument(
        "output_directory",
        help="Directory where the test file and runner will be saved.",
    )
    args = parser.parse_args()

    if not os.path.exists(args.output_directory):
        os.makedirs(args.output_directory)

    main(args.input_file, args.output_directory)
