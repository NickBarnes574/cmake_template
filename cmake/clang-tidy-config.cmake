# ----------------------------------------------------------------------------
# BRIEF -- clang-tidy-config.cmake
# ----------------------------------------------------------------------------
# Configures Clang-Tidy static analysis tool with suppressed checks.
#
# Features:
# - Defines a set of warnings to suppress
# - Enables `clang-tidy` when `ENABLE_CLANG_TIDY` is ON
# - Treats warnings as errors optionally
#
# Usage:
# - Enable with `-DENABLE_CLANG_TIDY=ON`
# - Override path with `-DCMAKE_C_CLANG_TIDY_PATH=<path>`
# ----------------------------------------------------------------------------

include(${CMAKE_CURRENT_LIST_DIR}/color-utils.cmake)

# Define clang-tidy checks
string(
    CONCAT
        CLANG_TIDY_SUPPRESSED_CHECKS
        "*,-readability-function-cognitive-complexity," # --------------------------Suppress complex func warnings; refactoring may be deferred.
        "-altera*," # --------------------------------------------------------------Suppress Altera-specific checks; not relevant for most projects.
        "-cert-dcl03-c," # ---------------------------------------------------------Suppress CERT rule for block scope declarations.
        "-misc-static-assert," # ---------------------------------------------------Suppress warnings on static asserts; may be intentional.
        "-llvm-include-order," # ---------------------------------------------------Suppress LLVM-specific include order warnings.
        "-llvmlibc-*," # -----------------------------------------------------------Suppress LLVM libc checks; likely unused in most cases.
        "-hicpp-*," # --------------------------------------------------------------Suppress strict High Integrity C++ checks.
        "-concurrency-mt-unsafe," # ------------------------------------------------Suppress multi-threading safety warnings.
        "-bugprone-easily-swappable-parameters," # ---------------------------------Suppress warnings on swappable params.
        "-cppcoreguidelines-*," # --------------------------------------------------Suppress C++ Core Guidelines warnings.
        "-readability-magic-numbers," # --------------------------------------------Suppress warnings on hardcoded values.
        "-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling," # -Suppress deprecated buffer usage warnings.
        "-misc-no-recursion," # ----------------------------------------------------Suppress recursion-related warnings.
        "-performance-no-int-to-ptr" # ---------------------------------------------Suppress int-to-pointer cast warnings.
)

# Clang-tidy setup
if(NOT CLANG_TIDY_PROG)
    find_program(CLANG_TIDY_PROG NAMES ${CMAKE_C_CLANG_TIDY_PATH} clang-tidy)
endif()

if(ENABLE_CLANG_TIDY)
    if(CLANG_TIDY_PROG)
        set(CMAKE_C_CLANG_TIDY "${CLANG_TIDY_PROG};-checks=${CLANG_TIDY_SUPPRESSED_CHECKS}")
        if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR ENABLE_WARN_AS_ERR)
            set(CMAKE_C_CLANG_TIDY "${CMAKE_C_CLANG_TIDY};-warnings-as-errors=*")
        endif()
    else()
        message_color(WARNING "Clang-Tidy not found, but ENABLE_CLANG_TIDY is ON.")
    endif()
endif()

# *** END OF FILE ***
