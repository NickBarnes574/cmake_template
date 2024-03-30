# FILENAME: tidy-checks.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Configures Clang-Tidy for static analysis, setting specific 
# checks and compiler options. Defines a function to configure build targets 
# with Clang-Tidy if available.
# -----------------------------------------------------------------------------

string(CONCAT CLANG_TIDY_CHECKS
    "*,-readability-function-cognitive-complexity,"
    "-altera*,"
    "-cert-dcl03-c,"
    "-misc-static-assert,"
    "-llvm-include-order,"
    "-llvmlibc-*,"
    "-hicpp-*,"
    "-concurrency-mt-unsafe,"
    "-bugprone-easily-swappable-parameters,"
    "-cppcoreguidelines-*,"
    "-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling,"
    "-readability-magic-numbers,"
    "-android-cloexec-accept,"
)

find_program(CLANG_TIDY_EXE NAMES "clang-tidy" REQUIRED)
set(CMAKE_C_CLANG_TIDY
    "clang-tidy-14" "-checks=${CLANG_TIDY_CHECKS}"
)

# *** end of file ***