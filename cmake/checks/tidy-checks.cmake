# FILENAME: tidy-checks.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Configures Clang-Tidy for static analysis, setting specific 
# checks and compiler options. Defines a function to configure build targets 
# with Clang-Tidy if available.
# -----------------------------------------------------------------------------

# General Exclusions
set(GENERAL_EXCLUSIONS
    "-readability-function-cognitive-complexity,"
    "-misc-static-assert,"
    "-llvm-include-order,"
    "-readability-magic-numbers,"
)

# Target-Specific and Library Exclusions
set(TARGET_SPECIFIC_EXCLUSIONS
    "-altera*,"
    "-android-cloexec-accept,"
)

# Security and Safety Exclusions
set(SECURITY_SAFETY_EXCLUSIONS
    "-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling,"
    "-cert-dcl03-c,"
)

# Concurrency and Multithreading Exclusions
set(CONCURRENCY_MT_EXCLUSIONS
    "-concurrency-mt-unsafe,"
)

# C++ Guidelines and Library Usage Exclusions
set(CPP_GUIDELINES_LIBRARY_EXCLUSIONS
    "-cppcoreguidelines-*,"
    "-hicpp-*,"
    "-llvmlibc-*,"
)

# Bug Prone Checks Exclusions
set(BUG_PRONE_CHECKS_EXCLUSIONS
    "-bugprone-easily-swappable-parameters,"
)

# Concatenate all check groups to form the final CLANG_TIDY_CHECKS string
string(CONCAT CLANG_TIDY_CHECKS
    "${GENERAL_EXCLUSIONS}"
    "${TARGET_SPECIFIC_EXCLUSIONS}"
    "${SECURITY_SAFETY_EXCLUSIONS}"
    "${CONCURRENCY_MT_EXCLUSIONS}"
    "${CPP_GUIDELINES_LIBRARY_EXCLUSIONS}"
    "${BUG_PRONE_CHECKS_EXCLUSIONS}"
)

find_program(CLANG_TIDY_EXE NAMES "clang-tidy" REQUIRED)
set(CMAKE_C_CLANG_TIDY
    "clang-tidy" "-checks=${CLANG_TIDY_CHECKS}"
)

# *** end of file ***
